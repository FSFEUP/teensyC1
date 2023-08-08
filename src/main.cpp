#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>
#include "logging.h"


#define AVG_SAMPLES 20

#define BRAKE_LIGHT 2
#define BRAKE_LIGHT_LOWER_THRESH 165       // 165/1023 * 3.3V = 0.532V
#define BRAKE_LIGHT_UPPER_THRESH 510       // /1023 * 3.3V = 0.78V
#define BRAKE_LIGHT_BRIGHTNESS 150         // 0-255
#define BRAKE_LIGHT_MIN_ACTIVE_PERIOD 200  // ms

#define BRAKE_SENSOR_PIN A5
#define CURRENT_SENSOR_PIN A4

#define SENSOR_SAMPLE_PERIOD 20  // ms

#define CAN_BAUD_RATE 500000
#define CAN_TRANSMISSION_PERIOD 100  // ms

// uncomment these lines to enable debugging
#define DEBUG_BL
// #define DEBUG_CAN

uint16_t brake_val = 0;

elapsedMillis canTimer;
elapsedMillis brake_sensor_timer;
elapsedMillis brake_light_active_timer;

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;

CAN_message_t brake_sensor_c3;

bool R2D = false;

int avgBuffer1[AVG_SAMPLES] = {0};

int average(int* buffer, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }
    return sum / n;
}

void bufferInsert(int* buffer, int n, int value) {
    for (int i = 0; i < n - 1; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[n - 1] = value;
}

void initMessages() {
    brake_sensor_c3.id = 0x123;
    brake_sensor_c3.len = 3;
    brake_sensor_c3.buf[0] = 0x90;
}

void canSniffer(const CAN_message_t& msg) {
    Logging loggingInstance;
    int tmp = 0;
    int tmp2 = 0;
    int tmp3 = 0;
    //Serial.println("CAN message received");
    //Serial.print("Message ID: ");
    //Serial.println(msg.id, HEX);
    if(msg.id == BAMO_RESPONSE_ID){
        switch (msg.buf[0]) {
            case REGID_NACT:
                tmp = (msg.buf[2] << 8) | msg.buf[1];
                loggingInstance.set_Nact(tmp);
                break;

            case REGID_VOUT:
                loggingInstance.set_Vout((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_ACTUAL_IQ:
                loggingInstance.set_Iq_actual((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_CMD_IQ:
                loggingInstance.set_Iq_cmd((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_MOUT:
                loggingInstance.set_Mout((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_I_LIM_INUSE:
                loggingInstance.set_I_lim_inuse((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_I_ACT_FILTERED:
                loggingInstance.set_I_actual_filtered((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_T_PEAK:
                loggingInstance.set_Tpeak((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_I_MAX_PEAK:
                loggingInstance.set_Imax_peak((msg.buf[2] << 8) | msg.buf[1]);
                break;

            case REGID_I_CON_EFF:
                loggingInstance.set_I_con_eff((msg.buf[2] << 8) | msg.buf[1]);
                break;
        
            case REGID_IGBT:
                tmp2 = (msg.buf[2] << 8) | msg.buf[1];
                tmp2 = (int)(tmp2 / 103.969 - 158.29);
                loggingInstance.set_powerStageTemp(tmp2);
                break;

            case REGID_AC_CURRENT:
                loggingInstance.set_ACCurrent((msg.buf[2] << 8) | msg.buf[1]);
                //ACCurrent = (ACCurrent * MAX_I) / ADC_MAX;  -> tenho que me relembrar do porquê disto MAX_I = 250; ADC_MAX = 65536
                break;

            case REGID_MOTOR_TEMP:
                tmp3 = (msg.buf[2] << 8) | msg.buf[1];
                tmp3 = tmp3 * 0.0194 - 160;
                loggingInstance.set_motorTemp(tmp3);
                break;

            default:
                break;
        }
    }
    if(msg.id == BMS_ID) {
        loggingInstance.set_current(((msg.buf[1] << 8) | msg.buf[0]) / 10);
        loggingInstance.set_soc(msg.buf[2] / 2);
        loggingInstance.set_packVoltage(((msg.buf[6] << 8) | msg.buf[5]) / 10);
    }
}

void canbusSetup() {
    can2.begin();
    can2.setBaudRate(500000);
    can2.enableFIFO();
    can2.enableFIFOInterrupt();
    can2.setFIFOFilter(REJECT_ALL);
    can2.setFIFOFilter(0, 0x111, STD);
    can2.setFIFOFilter(1, BMS_ID, STD);
    can2.setFIFOFilter(2, BAMO_RESPONSE_ID, STD);
    can2.onReceive(canSniffer);    
    initMessages();
}

void sendMsg(uint16_t brake_value) {
    brake_sensor_c3.buf[2] = (brake_value >> 8) & 0xFF;  // MSBn
    brake_sensor_c3.buf[1] = brake_value & 0xFF;         // LSB

    can2.write(brake_sensor_c3);
}

bool brakeLightControl(int brake_val) {
    if (brake_val >= BRAKE_LIGHT_LOWER_THRESH and brake_val <= BRAKE_LIGHT_UPPER_THRESH) {
        brake_light_active_timer = 0;
        analogWrite(BRAKE_LIGHT, BRAKE_LIGHT_BRIGHTNESS);
        return true;
    } else if (brake_light_active_timer > BRAKE_LIGHT_MIN_ACTIVE_PERIOD) {
        analogWrite(BRAKE_LIGHT, 0);
        return false;
    }
    return false;
}

void setup() {
    Logging loggingInstance;

    loggingInstance.set_CAN_messages();
    canbusSetup();
    loggingInstance.setup_log();
    pinMode(BRAKE_SENSOR_PIN, INPUT);
    pinMode(BRAKE_LIGHT, OUTPUT);

}

void loop() {
    Logging loggingInstance;
    loggingInstance.write_to_file();

    if (brake_sensor_timer > SENSOR_SAMPLE_PERIOD) {
        brake_sensor_timer = 0;
        brake_val = analogRead(BRAKE_SENSOR_PIN);
        bufferInsert(avgBuffer1, AVG_SAMPLES, brake_val);
        brake_val = average(avgBuffer1, AVG_SAMPLES);
#ifdef DEBUG_BL
        Serial.println(brake_val);
#endif
        if (brakeLightControl(brake_val)) {
#ifdef DEBUG_BL
            Serial.println("Brake Light ON");
#endif
            if (canTimer > CAN_TRANSMISSION_PERIOD) {
#ifdef DEBUG_CAN
                Serial.println("Message sent");
#endif
                sendMsg(brake_val);
                canTimer = 0;
            }
        }
    }
}