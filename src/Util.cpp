#include <stdint.h>
#include <avr/wdt.h>
#include <Arduino.h>

#include "config.hpp"
#include "servoShield.hpp"
#include "Util.hpp"

// Read a character from the serial port, blocking if no data is available.
uint8_t Util::readSerialBlocking() {
  int x;
  do {
    // Reset the watchdog timer, since we may be spinning in this loop for a
    // few seconds at the least.
    wdt_reset();
    // Serial.read returns a value between 0 and 255 if at least one byte is
    // available in the receive buffer or -1 if no data is available.
    x = Serial.read();
  } while (x < 0);
  return (uint8_t) x;
}

void Util::resetIO() {
  // Set all digital pins to inputs.
  for (uint8_t pin = 2; pin <= 13; pin++) {
    pinMode(pin, INPUT);
  }
  // Disable all servos.
  for (uint8_t servo = 0; servo <= 15; servo++) {
    servoShield.setPin(servo, 0, false);
  }
}

uint16_t Util::readUltrasound(uint8_t triggerPin, uint8_t echoPin) {
  // Reset trigger pin.
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Pulse trigger pin.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Set echo pin to input now. We don't do it earlier since it's allowable for
  // triggerPin and echoPin to be the same pin.
  pinMode(echoPin, INPUT);

  // Read return pulse.
  // Due to the long duration of this function call, we reset the watchdog
  // timer before and after to prevent an unintentional CPU reset.
  wdt_reset();
  uint16_t pulseDuration = pulseInLong(echoPin, HIGH, ULTRASOUND_TIMEOUT);
  wdt_reset();

  return pulseDuration;
}
