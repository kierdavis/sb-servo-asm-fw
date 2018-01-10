#ifndef __SERVO_ASM_FW_HANDLERS_HPP
#define __SERVO_ASM_FW_HANDLERS_HPP

#include <stdint.h>

#include "Request.hpp"
#include "Response.hpp"

namespace Handlers {
  void handleAnalogRead(Request *req, Response *resp);
  void handleDigitalWrite(Request *req, Response *resp,
                          uint8_t mode, uint8_t val);
  void handleDigitalRead(Request *req, Response *resp);
  void handleServoWrite(Request *req, Response *resp);
  void handleUltrasoundRead(Request *req, Response *resp);
  void handleVersion(Request *req, Response *resp);
  void handleReset(Request *req, Response *resp);
  void dispatch(Request *req, Response *resp);
}

#endif
