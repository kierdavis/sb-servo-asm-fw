# Any variable defined using the ?= operator can be overidden by defining an
# environment variable with the same name.

# Name of project (determines name of generated .elf and .hex files).
TARGET = servo-asm-fw

# Libraries used by the project, and their dependencies.
# Note that Arduino-Makefile only auto-detects direct dependencies of the
# project, not indirect dependencies.
ARDUINO_LIBS = Adafruit_PWMServoDriver Wire

# Pathname of this Makefile.
# See also: https://www.gnu.org/software/make/manual/make.html#Special-Variables
SELF := $(realpath $(lastword $(MAKEFILE_LIST)))

# Path to the firmware repository root, and its src subdirectory.
FIRMWARE_DIR ?= $(dir $(SELF))
SRC_DIR ?= $(FIRMWARE_DIR)/src

# Path to the checkout of the Arduino-Makefile repository.
ARDMK_DIR ?= $(FIRMWARE_DIR)/Arduino-Makefile

# Directory containing Arduino libraries used by this project.
USER_LIB_PATH ?= $(FIRMWARE_DIR)/libraries

# Path to Arduino installation.
ARDUINO_DIR ?= /usr/share/arduino

# Path to the directory containing the avrgcc & avrbinutils installations.
# The "bin" subdirectory of this path will be searched for executables.
AVR_TOOLS_DIR ?= /usr

# Alternatively, defining the environment variable OVERRIDE_EXECUTABLES allows
# the path to each executable to be specified individually.
ifdef OVERRIDE_EXECUTABLES
    CC      = $(AVR_CC)
    CXX     = $(AVR_CXX)
    AS      = $(AVR_AS)
    OBJCOPY = $(AVR_OBJCOPY)
    OBJDUMP = $(AVR_OBJDUMP)
    AR      = $(AVR_AR)
    SIZE    = $(AVR_SIZE)
    NM      = $(AVR_NM)
endif

# Arduino board model name.
BOARD_TAG ?= uno

# C/C++ preprocessor flags.
# Add the source directory to the search path for header files.
CPPFLAGS += -I$(SRC_DIR)
# Increase the size of the Arduino Serial receive buffer to 256 bytes.
CPPFLAGS += -DSERIAL_RX_BUFFER_SIZE=256

# C compiler flags
# -Wno-deprecated-declarations is there to work around a quirk of
# Arduino-Makefile and avr/pgmspace.h.
# When the macro __PROG_TYPES_COMPAT__ defined, pgmspace.h exposes an older
# version of its API, which raises deprecation warnings when it is used. Some
# symbols (e.g. PGM_P) are shared between both the old and new APIs, which means
# that code written against the new API will raise bogus deprecation warnings if
# these symbols are used while __PROG_TYPES_COMPAT__ is defined. Annoyingly,
# Arduino-Makefile defines __PROG_TYPES_COMPAT__ and does not currently
# provide a way to disable it, causing us to experience warnings when we use
# PGM_P and thus resulting in compilation failure because of -Werror.
# An upstream PR, https://github.com/sudar/Arduino-Makefile/pull/546, resolves
# this by removing __PROG_TYPES_COMPAT__ from Arduino-Makefile's default
# behaviour.
CFLAGS += -Wall -Werror -pedantic -Wno-deprecated-declarations

# C++ compiler flags
CXXFLAGS += -Wall -Werror -pedantic -Wno-deprecated-declarations

# C language standard
CFLAGS_STD ?= -std=gnu11

# C++ language standard
CXXFLAGS_STD ?= -std=gnu++14

# Linker flags
#LDFLAGS += -Lexample

# Source files
LOCAL_CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Serial baud rate for "make monitor".
# This must match SERIAL_BAUD_RATE in main.cpp.
MONITOR_BAUDRATE := 9600

include $(ARDMK_DIR)/Arduino.mk
