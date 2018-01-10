#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>

#include "Handlers.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "servoShield.hpp"
#include "Util.hpp"

static bool checkNumArgs(Request *req, Response *resp, uint8_t required) {
  if (req->numArguments() >= required) {
    return true;
  }
  else {
    resp->setFailureRetry(Response::FailureReason::NOT_ENOUGH_ARGUMENTS);
    return false;
  }
}

static bool checkArgs(Response *resp, bool condition) {
  if (condition) {
    return true;
  }
  else {
    resp->setFailureRetry(Response::FailureReason::INVALID_ARGUMENT);
    return false;
  }
}

static bool validAnalogPin(uint16_t pin) {
  // Pins 4 and 5 reserved for I²C comms with servo shield.
  return pin <= 3;
}

static bool validDigitalPin(uint16_t pin) {
  // Pins 0 and 1 reserved for USB serial comms.
  return pin >= 2 && pin <= 13;
}

void Handlers::handleAnalogRead(Request *req, Response *resp) {
  if (!checkNumArgs(req, resp, 1)) { return; }
  uint16_t pin = req->argument(0);
  if (!checkArgs(resp, validAnalogPin(pin))) { return; }

  uint16_t reading = (uint16_t) analogRead(pin);

  resp->setSuccess();
  resp->appendUint16(reading);
}

void Handlers::handleDigitalWrite(Request *req,
                                  Response *resp,
                                  uint8_t mode,
                                  uint8_t outputVal) {
  if (!checkNumArgs(req, resp, 1)) { return; }
  uint16_t pin = req->argument(0);
  if (!checkArgs(resp, validDigitalPin(pin))) { return; }

  pinMode(pin, mode);
  if (mode == OUTPUT) {
    digitalWrite(pin, outputVal);
  }

  resp->setSuccess();
}

void Handlers::handleDigitalRead(Request *req, Response *resp) {
  if (!checkNumArgs(req, resp, 1)) { return; }
  uint16_t pin = req->argument(0);
  if (!checkArgs(resp, validDigitalPin(pin))) { return; }

  char reading = digitalRead(pin) ? 'h' : 'l';

  resp->setSuccess();
  resp->appendChar(reading);
}

void Handlers::handleServoWrite(Request *req, Response *resp) {
  if (!checkNumArgs(req, resp, 2)) { return; }
  uint16_t servo = req->argument(0);
  uint16_t value = req->argument(1);
  if (!checkArgs(resp, servo <= 15 && value <= 4095)) { return; }

  servoShield.setPin(servo, value, false);

  resp->setSuccess();
}

void Handlers::handleUltrasoundRead(Request *req, Response *resp) {
  if (!checkNumArgs(req, resp, 2)) { return; }
  uint16_t triggerPin = req->argument(0);
  uint16_t echoPin = req->argument(1);
  if (!checkArgs(resp, validDigitalPin(triggerPin)
                    && validDigitalPin(echoPin))) { return; }

  uint16_t reading = Util::readUltrasound(triggerPin, echoPin);

  resp->setSuccess();
  resp->appendUint16(reading);
}

void Handlers::handleVersion(Request *req, Response *resp) {
  resp->setSuccess();
  resp->appendPgmString(Util::VERSION);
}

void Handlers::handleReset(Request *req, Response *resp) {
  Util::resetIO();
  resp->setSuccess();
}

void Handlers::dispatch(Request *req, Response *resp) {
  switch (req->command()) {
    case Request::Command::ANALOG_READ: {
      Handlers::handleAnalogRead(req, resp);
      break;
    }
    case Request::Command::NO_OPERATION: {
      // Do nothing, successfully.
      resp->setSuccess();
      break;
    }
    case Request::Command::DIGITAL_WRITE_HIGH: {
      Handlers::handleDigitalWrite(req, resp, OUTPUT, HIGH);
      break;
    }
    case Request::Command::DIGITAL_WRITE_INPUT: {
      Handlers::handleDigitalWrite(req, resp, INPUT, 0);
      break;
    }
    case Request::Command::DIGITAL_WRITE_LOW: {
      Handlers::handleDigitalWrite(req, resp, OUTPUT, LOW);
      break;
    }
    case Request::Command::DIGITAL_WRITE_PULLUP: {
      Handlers::handleDigitalWrite(req, resp, INPUT_PULLUP, 0);
      break;
    }
    case Request::Command::DIGITAL_READ: {
      Handlers::handleDigitalRead(req, resp);
      break;
    }
    case Request::Command::SERVO_WRITE: {
      Handlers::handleServoWrite(req, resp);
      break;
    }
    case Request::Command::ULTRASOUND_READ: {
      Handlers::handleUltrasoundRead(req, resp);
      break;
    }
    case Request::Command::VERSION: {
      Handlers::handleVersion(req, resp);
      break;
    }
    case Request::Command::RESET: {
      Handlers::handleReset(req, resp);
      break;
    }
    default: {
      // Invalid command.
      resp->setFailureRetry(Response::FailureReason::INVALID_COMMAND);
      break;
    }
  }
}
