#include <stdint.h>
#include <Arduino.h>

#include "RequestParser.hpp"
#include "Response.hpp"

// Baud rate for serial communication.
static const unsigned long SERIAL_BAUD_RATE = 9600;

// Read a character from the serial port, blocking if no data is available.
uint8_t readSerialBlocking() {
  int x;
  do {
    // Serial.read returns a value between 0 and 255 if at least one byte is
    // available in the receive buffer or -1 if no data is available.
    x = Serial.read();
  } while (x == -1);
  return (uint8_t) x;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  RequestParser::FeedResult result;

  // Read and parse characters from serial port until a complete request is
  // parsed.
  RequestParser::reset();
  do {
    char c = (char) readSerialBlocking();
    result = RequestParser::feed(c);
  } while (result == RequestParser::FeedResult::CONTINUE);

  // result is now either SUCCESS or FAILURE.
  if (result == RequestParser::FeedResult::SUCCESS) {
    // Dispatch the request to the appropriate handler.
    Request req;
    RequestParser::copyRequestTo(&req);
    // Handlers::dispatch(&req);
    Response::send(Response::Status::SUCCESS, nullptr, 0);
  }
  else {
    // Parsing failed, send a failure response.
    // In production, there are three common root causes for syntax errors:
    // - The client is autonomous software with a bug in its request formation.
    //   Retrying won't solve anything here.
    // - The client is a human who mistyped the request. They will almost
    //   certainly want to try again (but correctly this time).
    // - A transient communication error occurred (e.g. bit flip). Retrying will
    //   probably solve this.
    // Conclusion: recommend that the client retry, but give up if failure is
    // persistent.
    Response::sendFailure(Response::Status::FAILURE_RETRY,
                          Response::FailureReason::SYNTAX);
  }
}
