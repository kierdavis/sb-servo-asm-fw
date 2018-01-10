#ifndef __SERVO_ASM_FW_RESPONSE_HPP
#define __SERVO_ASM_FW_RESPONSE_HPP

#include <stdint.h>

namespace Response {
  enum class Status : char {
    // Success.
    SUCCESS = 's',
    // Failure. Server recommends the client retry the request, in case the
    // failure condition is transient. However, if the same request fails three
    // times in a row, the client should give up.
    FAILURE_RETRY = 'r',
    // Failure. Server recommends the client give up immediately.
    FAILURE_PANIC = 'p',
  };

  enum class FailureReason : char {
    // Not enough arguments for given command.
    NOT_ENOUGH_ARGUMENTS = 'a',
    // Invalid command character.
    INVALID_COMMAND = 'c',
    // Other request syntax error.
    SYNTAX = 's',
  };

  void send(Status status, char *payloadPointer, uint8_t payloadLength);

  void sendFailure(Status status, FailureReason reason);
}

#endif
