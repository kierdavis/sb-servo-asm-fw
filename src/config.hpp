#ifndef __SERVO_ASM_FW_CONFIG_HPP
#define __SERVO_ASM_FW_CONFIG_HPP

#include <stdbool.h>
#include <stdint.h>
#include <avr/pgmspace.h>

// Baud rate for serial communication.
// The current value of 9600 is pretty slow and could probably be increased.
// Then again, we're not sending/receiving that much data.
static const unsigned long SERIAL_BAUD_RATE = 9600;

// The pulse-width modulation frequency that will be used by the servo shield
// for controlling servos, in Hertz. 50 Hz is standard.
static const float SERVO_PWM_FREQ = 50;

// The maximum time we will spend waiting for the return pulse from an
// ultrasound sensor, in microseconds.
// A value of 50000 (50ms) gives a maximum range of 8.5m, far beyond the
// capability of the type of ultrasound module supported by the kit. In this
// time, at most 48 characters might be received over the serial connection
// (assuming 9600 baud), well within the receive buffer length of 256 bytes.
// However if the baud rate is increased, this timeout may need to be decreased
// in order to avoid dropping bytes.
static const unsigned long ULTRASOUND_TIMEOUT = 50000;

// Whether to echo received serial characters back to the client. Useful when
// being used interactively.
static const bool ECHO = true;

// The maximum number of arguments that may be included in a request. This
// determines the size of the arguments array.
static const uint8_t MAX_REQUEST_NUM_ARGUMENTS = 2;

// The maximum number of bytes that may be included in a response's payload.
// This determines the size of the payload buffer.
static const uint8_t MAX_RESPONSE_PAYLOAD_LENGTH = 32;

// Version string. Value defined in config.cpp.
extern const char * const VERSION PROGMEM;

#endif
