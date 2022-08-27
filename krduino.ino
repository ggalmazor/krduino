#include "Arduino.h"
#include "Remote.h"

typedef enum class Mode { MOVE,
                          IN_PLACE } Mode;

byte R_MOTOR_PWM_PIN = 6;  // 0 - 255
byte L_MOTOR_PWM_PIN = 5;  // 0 - 255
byte R_MOTOR_DIR_PIN = 7;  // 0 B, 1 F
byte L_MOTOR_DIR_PIN = 4;  // 0 B, 1 F
short MAX_SPEED = 255;
short throttle = 0;
short yaw = 0;
Mode mode;

uint32_t t;

Remote remote(A3, Mapping::GENERIC_4_BY_6);

void setup() {
  Serial.begin(115200);

  remote.init();

  mode = Mode::MOVE;

  pinMode(R_MOTOR_PWM_PIN, OUTPUT);
  pinMode(L_MOTOR_PWM_PIN, OUTPUT);
  pinMode(R_MOTOR_DIR_PIN, OUTPUT);
  pinMode(L_MOTOR_DIR_PIN, OUTPUT);

  t = millis();
}

void loop() {
  uint32_t now = millis();
  switch (remote.read_button()) {
    case Button::UP:
      if (mode != Mode::MOVE) {
        mode = Mode::MOVE;
        throttle = 0;
        yaw = 0;
      }
      throttle = min(10, throttle + 1);
      yaw = throttle == 0 ? 0 : yaw;
      break;
    case Button::DOWN:
      if (mode != Mode::MOVE) {
        mode = Mode::MOVE;
        throttle = 0;
        yaw = 0;
      }
      throttle = max(-10, throttle - 1);
      yaw = throttle == 0 ? 0 : yaw;
      break;
    case Button::CENTER:
      if (mode != Mode::MOVE) {
        mode = Mode::MOVE;
        throttle = 0;
        yaw = 0;
      }
      throttle = 0;
      yaw = 0;
      break;
    case Button::RIGHT:
      if (mode != Mode::MOVE) {
        mode = Mode::MOVE;
        throttle = 0;
        yaw = 0;
      }
      yaw = min(10, yaw + 1);
      t = millis();
      break;
    case Button::LEFT:
      if (mode != Mode::MOVE) {
        mode = Mode::MOVE;
        throttle = 0;
        yaw = 0;
      }
      yaw = max(-10, yaw - 1);
      t = millis();
      break;
    case Button::HARD_LEFT:
      mode = Mode::IN_PLACE;
      throttle = min(10, throttle + 1);
      yaw = -10;
      t = millis();
      break;
    case Button::HARD_RIGHT:
      mode = Mode::IN_PLACE;
      throttle = min(10, throttle + 1);
      yaw = 10;
      t = millis();
      break;
    default:
      if (now - t > 500) {
        if (mode != Mode::MOVE) {
          mode = Mode::MOVE;
          throttle = 0;
          yaw = 0;
          t = millis();
        } else {
          if (yaw != 0) {
            Serial.println("Centering wheel...");
            if (yaw != 0)
              yaw = yaw * .2;
          }
        }
        t = millis();
      }
      break;
  }
  move(mode, throttle, yaw);
}

void move(Mode mode, short throttle, short yaw) {
  if (mode == Mode::MOVE) {
    bool direction = throttle >= 0;
    digitalWrite(R_MOTOR_DIR_PIN, direction);
    digitalWrite(L_MOTOR_DIR_PIN, direction);

    double speed = (double)MAX_SPEED * (double)abs(throttle) / 10;
    double steering_percentage = (double)abs(yaw) / 10;
    double right_speed = yaw > 0 ? speed * (1 - steering_percentage) : speed;
    double left_speed = yaw < 0 ? speed * (1 - steering_percentage) : speed;
    analogWrite(R_MOTOR_PWM_PIN, right_speed);
    analogWrite(L_MOTOR_PWM_PIN, left_speed);
  } else if (mode == Mode::IN_PLACE) {
    double speed = (double)MAX_SPEED * (double)abs(throttle) / 10;
    digitalWrite(R_MOTOR_DIR_PIN, yaw < 0 ? 0 : 1);
    digitalWrite(L_MOTOR_DIR_PIN, yaw > 0 ? 0 : 1);
    analogWrite(R_MOTOR_PWM_PIN, speed);
    analogWrite(L_MOTOR_PWM_PIN, speed);
  }
}