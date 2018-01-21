#include <avr/wdt.h>
#include <Arduino.h>

#include "config.hpp"
#include "Handlers.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "servoShield.hpp"
#include "Util.hpp"

void setup() {
  // Turn on watchdog timer, which resets the CPU unless wdt_reset is
  // periodically called (i.e. it detects CPU hangs).
  // The ultrasound sensor measurement may take up to 50ms, so we pick the
  // lowest watchdog timeout that is greater than that.
  wdt_enable(WDTO_60MS);

  // Initialise serial communication with client (Raspberry Pi).
  Serial.begin(SERIAL_BAUD_RATE);

  // Initialise the servo shield.
  initServoShield();

  // Set all GPIO pins to input mode, and switch off all servos.
  Util::resetIO();
}

void loop() {
  wdt_reset();

  Response resp;
  RequestParser::FeedResult result;

  // Reset request parser state for this request.
  RequestParser::reset();

  // Read and parse characters from serial port until a complete request is
  // parsed.
  do {
    char c = (char) Util::readSerialBlocking();
    if (ECHO) { Serial.write(c); }
    result = RequestParser::feed(c);
  } while (result == RequestParser::FeedResult::CONTINUE);

  // result is now either SUCCESS or FAILURE.
  if (result == RequestParser::FeedResult::SUCCESS) {
    // Dispatch the request to the appropriate handler.
    Request *req = RequestParser::request();
    Handlers::dispatch(req, &resp);
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
