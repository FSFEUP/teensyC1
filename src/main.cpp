#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#define AVG_SAMPLES 20

#define BRAKE_LIGHT 2
#define BRAKE_LIGHT_BRIGHTNESS 150         // 0-255
#define BRAKE_LIGHT_MIN_ACTIVE_PERIOD 200  // ms

#define BRAKE_SENSOR A5
#define BRAKE_SENSOR_REF 165  // 202/1023 * 3.3V = 0.65V

#define CAN_BAUD_RATE 500000
#define CAN_TRANSMISSION_PERIOD 200  // ms

#define CURRENT_SENSOR A4

#define SENSOR_SAMPLE_PERIOD 20  // ms

// uncomment these lines to enable debugging
// #define DEBUG_BL
// #define DEBUG_CAN

uint16_t brake_val = 0;

elapsedMillis canTimer;
elapsedMillis brake_sensor_timer;
elapsedMillis brake_light_active_timer;

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
#ifdef DEBUG_CAN
    Serial.println("CAN message received");
    Serial.print("Message ID: ");
    Serial.println(msg.id, HEX);
#endif
    if (msg.id == 0x111)
        R2D = !R2D;
}

void canbusSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, 0x111, STD);
    can1.onReceive(canbusSniffer);

    initMessages();
}

void sendMsg(uint16_t brake_value) {
    uint8_t byte1 = (brake_value >> 8) & 0xFF;  // MSB
    uint8_t byte2 = brake_value & 0xFF;         // LSB

    brake_sensor_c3.buf[1] = byte2;
    brake_sensor_c3.buf[2] = byte1;

    can1.write(brake_sensor_c3);
}

bool brakeLightControl(int brake_val) {
    if (brake_val >= BRAKE_SENSOR_REF) {
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
    pinMode(BRAKE_SENSOR, INPUT);
    pinMode(BRAKE_LIGHT, OUTPUT);
}

void loop() {
    if (brake_sensor_timer > SENSOR_SAMPLE_PERIOD) {
        brake_sensor_timer = 0;
        brake_val = analogRead(BRAKE_SENSOR);
        bufferInsert(avgBuffer1, AVG_SAMPLES, brake_val);
        brake_val = average(avgBuffer1, AVG_SAMPLES);
#ifdef DEBUG_BL
        Serial.println(brake_val);
#endif
        if (brakeLightControl(brake_val)) {
#ifdef DEBUG_BL
            Serial.println("Brake Light ON");
#endif
            if (canTimer > CAN_TRANSMISSION_PERIOD and not R2D) {
#ifdef DEBUG_CAN
                Serial.println("Message sent");
#endif
                sendMsg(brake_val);
                canTimer = 0;
            }
        }
    }
}