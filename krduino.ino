#define IRMP_INPUT_PIN A3
#define IRMP_PROTOCOL_NAMES 1
#include <irmpSelectMain15Protocols.h>

#include <irmp.hpp>

IRMP_DATA irmp_data;

byte R_MOTOR_PWM_PIN = 6;  // 0 - 255
byte L_MOTOR_PWM_PIN = 5;  // 0 - 255
byte R_MOTOR_DIR_PIN = 7;  // 0 B, 1 F
byte L_MOTOR_DIR_PIN = 4;  // 0 B, 1 F

void setup() {
  pinMode(R_MOTOR_PWM_PIN, OUTPUT);
  pinMode(L_MOTOR_PWM_PIN, OUTPUT);
  pinMode(R_MOTOR_DIR_PIN, OUTPUT);
  pinMode(L_MOTOR_DIR_PIN, OUTPUT);

  // delay(4000);

  irmp_init();
}

short speed = 0;
short yaw = 0;

void loop() {
  if (irmp_get_data(&irmp_data)) {
    switch (irmp_data.command) {
      case 0x9F60:  // cursor up
        speed = min(250, speed + 10);
        yaw = speed == 0 ? 0 : yaw;
        break;
      case 0x9E61:  // cursor down
        speed = max(-250, speed - 10);
        yaw = speed == 0 ? 0 : yaw;
        break;
      case 0x9768:  // cursor center
        speed = 0;
        yaw = 0;
        break;
      case 0x9D62:  // cursor right
        yaw = min(10, yaw + 1);
        break;
      case 0x9A65:  // cursor left
        yaw = max(-10, yaw - 1);
        break;
      default:
        break;
    }
    irmp_get_data(&irmp_data);
  }
  move(speed, yaw);
}

void move(short speed, short yaw) {
  // Lots of room for improvement here. Bias should take away the percentage
  // differential to the wheel than needs to rotate less
  double left_bias = (double(yaw) + 10) / 20;
  double right_bias = 1 - left_bias;
  short left_speed = abs(speed) * left_bias;
  short right_speed = abs(speed) * right_bias;
  
  bool direction = speed >= 0;
  digitalWrite(R_MOTOR_DIR_PIN, direction);
  digitalWrite(L_MOTOR_DIR_PIN, direction);
  analogWrite(R_MOTOR_PWM_PIN, right_speed);
  analogWrite(L_MOTOR_PWM_PIN, left_speed);
}