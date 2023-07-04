#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#define CAN_BAUD_RATE 500000
#define AVG_SAMPLES 20

#define brake_light A1
#define brake_light_brightness 100  // 0-255
#define brake_sensor A17
#define brake_sensor_ref 158               // 202/1023 * 3.3V = 0.65V
#define brake_sensor_read_period 5         // ms
#define brake_light_min_active_period 200  // ms

uint16_t brake_val = 0;

elapsedMillis brake_sensor_timer;
elapsedMillis brake_light_active_timer;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t brake_sensor_c3;

int avgBuffer1[AVG_SAMPLES] = {0};

int average(int* buffer, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }
    return sum / n;
}

void buffer_insert(int* buffer, int n, int value) {
    for (int i = 0; i < n - 1; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[n - 1] = value;
}

void init_message() {
    brake_sensor_c3.id = 0x123;
    brake_sensor_c3.len = 3;
    brake_sensor_c3.buf[0] = 0x90;
}

void canbus_listener(const CAN_message_t& msg) {
    Serial.println("CAN message received");
    Serial.print("Message ID: ");
    Serial.println(msg.id, HEX);
}

void canbus_setup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, 0x123, STD);
    can1.onReceive(canbus_listener);

    init_message();
}

void send_msg(uint16_t brake_value) {
    uint8_t byte1 = (brake_value >> 8) & 0xFF;  // MSB
    uint8_t byte2 = brake_value & 0xFF;         // LSB

    brake_sensor_c3.buf[1] = byte2;
    brake_sensor_c3.buf[2] = byte1;

    can1.write(brake_sensor_c3);
}

void brake_light_control(int brake_val) {
    if (brake_val >= brake_sensor_ref) {
        brake_light_active_timer = 0;
        analogWrite(brake_light, brake_light_brightness);
    } else if (brake_light_active_timer > brake_light_min_active_period) {
        analogWrite(brake_light, 0);
    }
}

void setup() {
    canbus_setup();
    pinMode(brake_sensor, INPUT);
    pinMode(brake_light, OUTPUT);
}

void loop() {
    // delay(200);

    if (brake_sensor_timer > brake_sensor_read_period) {
        brake_sensor_timer = 0;
        brake_val = analogRead(brake_sensor);
        buffer_insert(avgBuffer1, AVG_SAMPLES, brake_val);
        brake_val = average(avgBuffer1, AVG_SAMPLES);
        Serial.println(brake_val);

        brake_light_control(brake_val);
        if (brake_val >= brake_sensor_ref) {
            Serial.println("Brake Light ON");
            send_msg(brake_val);
        }
    }
}