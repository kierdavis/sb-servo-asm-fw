#include <stdbool.h>
#include <stdint.h>

#include "Request.hpp"

Request::Request(Request::Command command) {
  reset(command);
}

Request::Command Request::command() {
  return _command;
}

uint8_t Request::numArguments() {
  return _numArguments;
}

Request::Argument Request::argument(uint8_t index) {
  if (index < _numArguments) {
    return _arguments[index];
  } else {
    return 0;
  }
}

void Request::reset(Request::Command command) {
  _command = command;
  _numArguments = 0;
  // Set all arguments to zero.
  for (uint8_t i = 0; i < Request::MAX_NUM_ARGUMENTS; i++) {
    _arguments[i] = 0;
  }
}

bool Request::addArgument(Argument value) {
  uint8_t n = _numArguments;
  if (n < Request::MAX_NUM_ARGUMENTS) {
    _arguments[n] = value;
    _numArguments = n + 1;
    return true;
  } else {
    return false;
  }
}
