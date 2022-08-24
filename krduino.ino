#define IRMP_INPUT_PIN A3
#define IRMP_PROTOCOL_NAMES 1
#include <irmpSelectMain15Protocols.h>

#include <irmp.hpp>

IRMP_DATA irmp_data;

byte R_MOTOR_PWM_PIN = 6;  // 0 - 255
byte L_MOTOR_PWM_PIN = 5;  // 0 - 255
byte R_MOTOR_DIR_PIN = 7;  // 0 B, 1 F
byte L_MOTOR_DIR_PIN = 4;  // 0 B, 1 F
short MAX_SPEED = 255;

void setup() {
  Serial.begin(115200);
  pinMode(R_MOTOR_PWM_PIN, OUTPUT);
  pinMode(L_MOTOR_PWM_PIN, OUTPUT);
  pinMode(R_MOTOR_DIR_PIN, OUTPUT);
  pinMode(L_MOTOR_DIR_PIN, OUTPUT);

  irmp_init();
}

short throttle = 0;
short yaw = 0;

void loop() {
  if (irmp_get_data(&irmp_data)) {
    switch (irmp_data.command) {
      case 0x9F60:  // cursor up
        throttle = min(10, throttle + 1);
        yaw = throttle == 0 ? 0 : yaw;
        break;
      case 0x9E61:  // cursor down
        throttle = max(-10, throttle - 1);
        yaw = throttle == 0 ? 0 : yaw;
        break;
      case 0x9768:  // cursor center
        throttle = 0;
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
  move(throttle, yaw);
}

void move(short throttle, short yaw) {
  bool direction = throttle >= 0;
  digitalWrite(R_MOTOR_DIR_PIN, direction);
  digitalWrite(L_MOTOR_DIR_PIN, direction);

  double speed = (double)MAX_SPEED * (double)abs(throttle) / 10;
  double steering_percentage = (double)abs(yaw) / 10;
  double right_speed = yaw > 0 ? speed * (1 - steering_percentage) : speed;
  double left_speed = yaw < 0 ? speed * (1 - steering_percentage) : speed;
  analogWrite(R_MOTOR_PWM_PIN, right_speed);
  analogWrite(L_MOTOR_PWM_PIN, left_speed);
}