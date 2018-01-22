#include <stdbool.h>
#include <stdint.h>

#include "Request.hpp"
#include "RequestParser.hpp"

static RequestParser::State state;
static Request currentRequest;
static Request::Argument currentArgument;
static bool failed;

void RequestParser::reset() {
  state = RequestParser::State::COMMAND;
  currentRequest.reset();
  currentArgument = 0;
  failed = false;
}

static void flagFailure() {
  failed = true;
}

static void feedPartial(char c) {
  // If a syntax error occurred earlier in the parsing of this command, drop
  // all characters until the next newline, then report it at the end.
  if (failed) {
    return;
  }

  // Skip whitespace.
  if (c == ' ' || c == '\t' || c == '\r') {
    return;
  }

  switch (state) {
    case RequestParser::State::COMMAND: {
      // This character should be the command letter.
      if (c < 'a' || c > 'z') {
        // Not a lowercase letter, hence invalid.
        flagFailure();
        return;
      }
      // Request::Command enum items have same values as their corresponding
      // character, so conversion is just a cast.
      Request::Command command = (Request::Command) c;
      // Reinitialise fields of currentRequest, using this command and an empty
      // argument list.
      currentRequest.reset(command);
      // Also reinitialise currentArgument to zero.
      currentArgument = 0;
      // Go to the ARGUMENTS state.
      state = RequestParser::State::ARGUMENTS;
      break;
    }

    case RequestParser::State::ARGUMENTS: {
      // This character should be a digit or comma.
      if (c >= '0' && c <= '9') {
        uint16_t digit = c - '0';
        // Append digit to currentArgument. Overflow is ignored.
        currentArgument = (currentArgument * 10) + digit;
        // Stay in the ARGUMENTS state.
      }
      else if (c == ',') {
        // End of one argument, start of another.
        if (!currentRequest.addArgument(currentArgument)) {
          // More than MAX_NUM_ARGUMENTS arguments received.
          flagFailure();
          return;
        }
        currentArgument = 0;
        // Stay in the ARGUMENTS state.
      }
      else {
        // Invalid character.
        flagFailure();
        return;
      }
      break;
    }
  }
}

static void finish() {
  // If a syntax error occurred earlier in the parsing of this command, we're
  // going to throw it away, so no point "finishing" it.
  if (failed) {
    return;
  }

  // currentArgument still holds the last argument of the command.
  if (!currentRequest.addArgument(currentArgument)) {
    // More than MAX_NUM_ARGUMENTS arguments received.
    flagFailure();
    return;
  }
}

RequestParser::FeedResult RequestParser::feed(char c) {
  if (c == '\0') {
    // Reset the parser state.
    RequestParser::reset();
    return RequestParser::FeedResult::CONTINUE;
  }

  if (c == '\n') {
    // End of a request.
    finish();
    return failed ? RequestParser::FeedResult::FAILURE
                  : RequestParser::FeedResult::SUCCESS;
  }
  else {
    feedPartial(c);
    return RequestParser::FeedResult::CONTINUE;
  }
}

Request *RequestParser::request() {
  return &currentRequest;
}
