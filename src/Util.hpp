#ifndef __SERVO_ASM_FW_UTIL_HPP
#define __SERVO_ASM_FW_UTIL_HPP

#include <stdint.h>
#include <avr/pgmspace.h>

namespace Util {
  // Version string.
  extern const char * const VERSION PROGMEM;

  void resetIO();

  // Measures a distance using an ultrasonic sensor module connected to the
  // given pins, returning the delay between transmitted and reflected
  // ultrasonic waves in microseconds. To convert this to a distance:
  // 1) Multiply by 1e-6, to convert to seconds.
  // 2) Multiply by the speed of sound (343) to get round-trip distance in
  //    metres.
  // 3) Divide by 2, to get the distance between the sensor and the point of
  //    reflection.
  uint16_t readUltrasound(uint8_t triggerPin, uint8_t echoPin);
}

#endif
