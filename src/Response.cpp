#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <Arduino.h>

#include "config.hpp"
#include "Response.hpp"

Response::Response() {
  reset();
}

void Response::reset() {
  setBug(Response::BugReason::RESPONSE_UNINITIALISED);
}

void Response::set(Response::Status status) {
  _status = status;
  _payloadLength = 0;
}

void Response::setSuccess() {
  set(Response::Status::SUCCESS);
}

void Response::setFailureRetry(Response::FailureReason reason) {
  set(Response::Status::FAILURE_RETRY);
  appendChar((char) reason);
}

void Response::setFailurePanic(Response::FailureReason reason) {
  set(Response::Status::FAILURE_PANIC);
  appendChar((char) reason);
}

void Response::setBug(Response::BugReason reason) {
  set(Response::Status::BUG);
  appendChar((char) reason);
}

void Response::appendChar(char c) {
  uint8_t length = _payloadLength;
  if (length < MAX_RESPONSE_PAYLOAD_LENGTH) {
    _payload[length] = c;
    _payloadLength = length + 1;
  }
  else {
    setBug(Response::BugReason::RESPONSE_PAYLOAD_TOO_LONG);
  }
}

void Response::appendUint16(uint16_t value) {
  // Sorry, but printf carries a lot of overhead for something as simple as
  // converting a single unsigned number to a string.
  // Quantification: replacing the following implementation with a call to
  // snprintf acting directly on the _payload buffer increased the nonvolatile
  // (Flash) memory usage by 1428 bytes. It did not however affect the
  // volatile (RAM) usage.
  // `foundFirstNonzeroDigit` is false until we find the first non-zero digit.
  // It controls whether digits are printed. This causes leading zeros to be
  // omitted.
  bool foundFirstNonzeroDigit = false;
  char digit;
  static const uint16_t weights[4] PROGMEM = {10000, 1000, 100, 10};
  for (uint8_t i = 0; i < 4; i++) {
    uint16_t weight = pgm_read_word(&weights[i]);
    digit = '0';
    while (value >= weight) {
      digit++;
      value -= weight;
      foundFirstNonzeroDigit = true;
    }
    // Now, `value < weight` and `digit` holds the ASCII character for the digit
    // representing the multiple of `weight` in the original number.
    if (foundFirstNonzeroDigit) {
      appendChar(digit);
    }
  }
  // `value` is now left holding the units (least significant) digit.
  digit = '0' + value;
  appendChar(digit);
}

void Response::appendStringP(PGM_P string) {
  while (1) {
    char c = pgm_read_byte(string);
    if (c == '\0') { break; }
    appendChar(c);
    string++;
  }
}

void Response::send() {
  // Since it may take up to 38ms to send a maximum length response (36
  // bytes) over a 9600 baud serial connection, we reset the watchdog timer
  // before and after.
  wdt_reset();
  // A '>' marks the start of a response line. Lines without this character are
  // ignored by the client, allowing diagnostic output to be interspersed.
  Serial.write('>');
  // Items of Response::Status enum have values equal to the corresponding
  // character codes used in the wire encoding, so serialisation is simply
  // a cast.
  Serial.write((char) _status);
  for (uint8_t i = 0; i < _payloadLength; i++) {
    Serial.write(_payload[i]);
  }
  Serial.write('\r');
  Serial.write('\n'); // End of response line.
  wdt_reset();
}
