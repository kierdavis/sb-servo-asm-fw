#ifndef __SERVO_ASM_FW_RESPONSE_HPP
#define __SERVO_ASM_FW_RESPONSE_HPP

#include <stdint.h>
#include <avr/pgmspace.h>

#include "config.hpp"

class Response {
public:
  enum class Status : char {
    // Success.
    SUCCESS = 's',
    // Failure. Server recommends the client retry the request, in case the
    // failure condition is transient. However, if the same request fails three
    // times in a row, the client should give up.
    FAILURE_RETRY = 'r',
    // Failure. Server recommends the client give up immediately.
    FAILURE_PANIC = 'p',
    // If received by the client, indicates a bug in the firmware. Can be
    // considered alike to a failed assertion.
    BUG = '!',
  };

  enum class FailureReason : char {
    // Invalid argument.
    INVALID_ARGUMENT = 'a',
    // Invalid command character.
    INVALID_COMMAND = 'c',
    // Not enough arguments for given command.
    NOT_ENOUGH_ARGUMENTS = 'n',
    // Other request syntax error.
    SYNTAX = 's',
  };

  enum class BugReason : char {
    RESPONSE_UNINITIALISED = 'u',
    RESPONSE_PAYLOAD_TOO_LONG = 'l',
  };

  Response();
  void reset();

  void set(Status status);
  void setSuccess();
  void setFailureRetry(FailureReason reason);
  void setFailurePanic(FailureReason reason);
  void setBug(BugReason reason);

  void appendChar(char character);
  void appendUint16(uint16_t value);
  void appendPgmString(PGM_P string);

  void send();

protected:
  Status _status;
  uint8_t _payloadLength;
  char _payload[MAX_RESPONSE_PAYLOAD_LENGTH];
};

#endif
