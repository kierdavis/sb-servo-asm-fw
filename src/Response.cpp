#include <stdint.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

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
  if (length < Response::MAX_PAYLOAD_LENGTH) {
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
  static const uint16_t weights[4] PROGMEM = {10000, 1000, 100, 10};
  // foundNonzeroDigit is false until we find the first non-zero digit. It
  // controls whether digits are printed. This causes leading zeros to be
  // omitted.
  bool foundNonzeroDigit = false;
  for (uint8_t i = 0; i < 4; i++) {
    char digit = '0';
    uint16_t weight = pgm_read_word(&weights[i]);
    while (value >= weight) {
      digit++;
      value -= weight;
      foundNonzeroDigit = true;
    }
    if (foundNonzeroDigit) {
      appendChar(digit);
    }
  }
  appendChar('0' + value);
}

void Response::appendPgmString(const char *string) {
  while (1) {
    char c = pgm_read_byte(string);
    if (c == '\0') { break; }
    appendChar(c);
    string++;
  }
}

void Response::send() {
  // Items of Response::Status enum have values equal to the corresponding
  // character codes used in the wire encoding, so serialisation is simply
  // a cast.
  // A '>' marks the start of a response line. Lines without this character are
  // ignored by the client, allowing diagnostic output to be interspersed,
  Serial.write('>');
  Serial.write((char) _status);
  for (uint8_t i = 0; i < _payloadLength; i++) {
    Serial.write(_payload[i]);
  }
  Serial.write('\r');
  Serial.write('\n'); // End of response line.
}
