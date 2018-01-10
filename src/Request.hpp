#ifndef __SERVO_ASM_FW_REQUEST_HPP
#define __SERVO_ASM_FW_REQUEST_HPP

#include <stdbool.h>
#include <stdint.h>

// A Request models a request sent by the client (the Raspberry Pi) to the
// server (this Arduino) asking the latter to perform some action. Requests
// consists of a command, which specifies which action to perform, and
// arguments, which are additional data (e.g. pin numbers) required to perform
// said action.
class Request {
public:
  // An enumeration of valid commands. The value of each enum item is set to
  // the character code used to encode the command in the wire protocol. This
  // means that parsing a Command is just a cast (a zero-cost operation).
  enum class Command : char {
    NO_OPERATION = 'n',
    DIGITAL_READ = 'r',
    DIGITAL_WRITE_HIGH = 'h',
    DIGITAL_WRITE_LOW = 'l',
  };

  // The type of a parsed request argument (an usigned 16-bit integer).
  typedef uint16_t Argument;

  // The maximum number of arguments that may be included in a request. This
  // determines the size of the arguments array.
  static const uint8_t MAX_NUM_ARGUMENTS = 2;

  // Create a new Request with the specified command and no arguments.
  // Arguments can later be added with the addArgument method.
  Request(Command command = Command::NO_OPERATION);

  // Access the command and arguments of the request.
  Command command();
  uint8_t numArguments();
  Argument argument(uint8_t index); // Returns 0 if index out of range.

  // Resets the properties of the Request, akin to calling its constructor.
  // `r.reset()` is equivalent to and slightly more efficient than,
  // `r = Request()`, assuming `r` is an existing instance of `Request`.
  void reset(Command command = Command::NO_OPERATION);

  // Add an argument with the given value. Should only be called by the request
  // parser. Returns true if there was space to add the new argument (i.e.
  // there were fewer than MAX_NUM_ARGUMENTS arguments already attached to the
  // Request), false otherwise.
  bool addArgument(Argument value);

protected:
  Command _command;
  uint8_t _numArguments;
  Argument _arguments[MAX_NUM_ARGUMENTS];
};

#endif
