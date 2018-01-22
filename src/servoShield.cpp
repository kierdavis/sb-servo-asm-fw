#include <Adafruit_PWMServoDriver.h>

#include "config.hpp"
#include "servoShield.hpp"

Adafruit_PWMServoDriver servoShield(SERVO_SHIELD_ADDR);

void initServoShield() {
  servoShield.begin();
  servoShield.setPWMFreq(SERVO_PWM_FREQ);
}
