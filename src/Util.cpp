#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <Arduino.h>

#include "servoShield.hpp"
#include "Util.hpp"

const char * const Util::VERSION PROGMEM = "SourceBots GPIO/servo v0.1.0";

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
  } while (x == -1);
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
  // Timeout is in microseconds. A value of 50000 (50ms) gives a maximum range
  // of 8.5m, far beyond the capability of the type of ultrasound module
  // supported by the kit. In this time, at most 48 characters might be
  // received over the serial connection (assuming 9600 baud), well within the
  // receive buffer length of 256 bytes. However if the baud rate is increase,
  // this timeout may need to be decreased in order to avoid dropping bytes.
  // Due to the long duration of this function call, we reset the watchdog
  // timer before and after to prevent an unintentional CPU reset.
  static const unsigned long TIMEOUT = 50000;
  wdt_reset();
  uint16_t pulseDuration = pulseInLong(echoPin, HIGH, TIMEOUT);
  wdt_reset();

  return pulseDuration;
}
