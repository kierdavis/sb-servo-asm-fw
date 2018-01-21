#include <Adafruit_PWMServoDriver.h>

#include "config.hpp"
#include "servoShield.hpp"

Adafruit_PWMServoDriver servoShield;

void initServoShield() {
  servoShield.begin();
  servoShield.setPWMFreq(SERVO_PWM_FREQ);
}
