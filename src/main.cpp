#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>
#include <logging.h>

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


uint16_t brake_val = 0;

elapsedMillis canTimer;
elapsedMillis brake_sensor_timer;
elapsedMillis brake_light_active_timer;

int current = 0;
int voltage = 0;
int mintmp = 0;
int maxtmp = 0;
int avgtmp = 0;
int apps1 = 0;
int apps2 = 0;
int brake = 0;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

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

void canbusSniffer(const CAN_message_t& msg) {
    //Serial.println("CAN message received");
    //Serial.print("Message ID: ");
    //Serial.println(msg.id, HEX);
    switch(msg.id) {
        case BMS_ID:
        current = ((msg.buf[1] << 8) | msg.buf[0]) / 10;
        voltage = ((msg.buf[3] << 8) | msg.buf[2]) / 10;
        mintmp = msg.buf[4];
        maxtmp = msg.buf[5];
        avgtmp = msg.buf[6];
        break;
        
        case 0x111:
        apps1 = ((msg.buf[1] << 8) | msg.buf[0]);
        apps2 = ((msg.buf[3] << 8) | msg.buf[2]);
        break;
    }
}

void canbusSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, 0x111, STD);
    can1.setFIFOFilter(1, BMS_ID, STD);
    can1.onReceive(canbusSniffer);
    initMessages();
}

void sendMsg(uint16_t brake_value) {
    brake_sensor_c3.buf[2] = (brake_value >> 8) & 0xFF;  // MSB
    brake_sensor_c3.buf[1] = brake_value & 0xFF;         // LSB

    can1.write(brake_sensor_c3);
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
    canbusSetup();
    pinMode(BRAKE_SENSOR_PIN, INPUT);
    pinMode(BRAKE_LIGHT, OUTPUT);
}

void loop() {
    if (brake_sensor_timer > SENSOR_SAMPLE_PERIOD) {
        brake_sensor_timer = 0;
        brake_val = analogRead(BRAKE_SENSOR_PIN);
        bufferInsert(avgBuffer1, AVG_SAMPLES, brake_val);
        brake_val = average(avgBuffer1, AVG_SAMPLES);
        brake = brake_val;
        //Serial.println(brake_val);
        if (brakeLightControl(brake_val)) {
            //Serial.println("Brake Light ON");
            if (canTimer > CAN_TRANSMISSION_PERIOD) {
                //Serial.println("Message sent");
                sendMsg(brake_val);
                canTimer = 0;
            }
        }
    }
    write_to_file(current, voltage, mintmp, maxtmp, avgtmp, apps1, apps2, brake);

}