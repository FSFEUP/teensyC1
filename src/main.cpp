#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#define CAN_BAUD_RATE 500000

uint8_t brake_light = A1;
uint8_t brake_light_brightness = 20;
uint8_t brake_sensor = A14;
uint8_t brake_sensor_ref = 202; 
uint8_t brake_sensor_read_period = 20;  // ms
elapsedMillis brake_sensor_timer;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t brake_sensor_c3;

void init_message() {
    brake_sensor_c3.id = 0x123;
    brake_sensor_c3.len = 3;
    brake_sensor_c3.buf[0] = 0x00;
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
    can1.onReceive(canbus_listener);
}
void send_msg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    brake_sensor_c3.buf[1] = byte2;
    brake_sensor_c3.buf[2] = byte1;

    can1.write(brake_sensor_c3);
}

void brake_light_control(int brake_val)
{
  if(brake_val >= brake_sensor_ref){   
    analogWrite(brake_light, brake_light_brightness);
  }else{ 
    analogWrite(brake_light, 0);
  }
}
void setup() {
    canbus_setup();
    pinMode(brake_sensor, INPUT);
    pinMode(brake_light, OUTPUT);
}

void loop() {
    if (brake_sensor_timer > brake_sensor_read_period) {
        brake_sensor_timer = 0;
        int brake_val = analogRead(brake_sensor);
        Serial.println(brake_val);     
        
        brake_light_control(brake_val);
        send_msg(brake_val);
    }
}
