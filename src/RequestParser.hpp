#ifndef __SERVO_ASM_FW_REQUEST_PARSER_HPP
#define __SERVO_ASM_FW_REQUEST_PARSER_HPP

#include "Request.hpp"

/*
Usage example:

char inputChar;
RequestParser::FeedResult result;

while (1) {
  RequestParser::reset();
  do {
    inputChar = getNextCharOfInput();
    result = RequestParser::feed(inputChar);
  } while (result == RequestParser::FeedResult::CONTINUE);

  if (result == RequestParser::FeedResult::SUCCESS) {
    Request *req = RequestParser::request();
    handleRequest(req);
  }
  else {
    sendFailureResponse();
  }
}
*/

namespace RequestParser {
  enum class State {
    // Next character is the command letter.
    COMMAND,
    // Next character is an argument digit or comma.
    ARGUMENTS,
  };

  enum class FeedResult {
    // A request was successfully parsed. User can call copyRequestTo to get a
    // copy of it. User must then call reset.
    SUCCESS,
    // Request parsing failed. User must then call reset.
    FAILURE,
    // Parsing incomplete, feed must be called with more input.
    CONTINUE,
  };

  // Reset the parser state. Must be called before the first call to feed, and
  // after every call to feed that returned SUCCESS or FAILURE.
  void reset();

  // Feed the given character to the parser. When a complete request has been
  // read (successfully or not), SUCCESS or FAILURE is returned correspondingly.
  // Otherwise, CONTINUE is returned to indicate that more input needs to be fed
  // to the parser.
  FeedResult feed(char c);

  // Returns a pointer to the successfully parsed Request. The data pointed to
  // should only be considered valid if the most recent call to feed returned
  // SUCCESS.
  Request *request();
}

#endif
