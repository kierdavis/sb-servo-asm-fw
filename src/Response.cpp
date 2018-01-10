#include <stdint.h>
#include <Arduino.h>

#include "Response.hpp"

void Response::send(Response::Status status,
                    char *payloadPointer,
                    uint8_t payloadLength) {
  // Items of Response::Status enum have values equal to the corresponding
  // character codes used in the wire encoding, so serialisation is simply
  // a cast.
  Serial.write((char) status);
  for (uint8_t i = 0; i < payloadLength; i++) {
    Serial.write(payloadPointer[i]);
  }
  Serial.write('\n'); // End of response line.
}

void Response::sendFailure(Response::Status status,
                           Response::FailureReason reason) {
  char encodedReason = (char) reason;
  Response::send(status, &encodedReason, 1);
}
