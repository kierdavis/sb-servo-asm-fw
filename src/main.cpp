#include <stdint.h>
#include <Arduino.h>

#include "Handlers.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "servoShield.hpp"
#include "Util.hpp"

// Baud rate for serial communication.
static const unsigned long SERIAL_BAUD_RATE = 9600;

void setup() {
  Util::resetIO();

  Serial.begin(SERIAL_BAUD_RATE);

  servoShield.begin();
  servoShield.setPWMFreq(SERVO_PWM_FREQ);
}

void loop() {
  // Store the Request and Response in static memory rather than on the stack.
  // This can improve optimisation of the code, and should be safe because
  // loop() is never called recursively.
  static Request req;
  static Response resp;

  RequestParser::FeedResult result;

  // Reset states for this request.
  RequestParser::reset();
  resp.reset();

  // Read and parse characters from serial port until a complete request is
  // parsed.
  do {
    char c = (char) Util::readSerialBlocking();
    result = RequestParser::feed(c);
  } while (result == RequestParser::FeedResult::CONTINUE);

  // result is now either SUCCESS or FAILURE.
  if (result == RequestParser::FeedResult::SUCCESS) {
    // Dispatch the request to the appropriate handler.
    RequestParser::copyRequestTo(&req);
    Handlers::dispatch(&req, &resp);
  }
  else {
    // Parsing failed, prepare a failure response.
    // In production, there are three common root causes for syntax errors:
    // - The client is autonomous software with a bug in its request formation.
    //   Retrying won't solve anything here.
    // - The client is a human who mistyped the request. They will almost
    //   certainly want to try again (but correctly this time).
    // - A transient communication error occurred (e.g. bit flip). Retrying will
    //   probably solve this.
    // Conclusion: recommend that the client retry, but give up if failure is
    // persistent.
    resp.setFailureRetry(Response::FailureReason::SYNTAX);
  }

  // Send the response to the client.
  // Due to the structure of this function, every request (or, in the face of
  // bad request syntax, every incoming newline character) must result in a
  // response being sent to the client, even if the response is "no useful
  // response was generated" (i.e. Response::BugReason::RESPONSE_UNINITIALISED).
  // This should aid protocol synchronisation.
  resp.send();
}
