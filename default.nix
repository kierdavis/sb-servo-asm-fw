let defaultNixpkgs = import <nixpkgs> {}; in

{ arduino-core ? defaultNixpkgs.arduino-core
, avrbinutils  ? defaultNixpkgs.avrbinutils
, avrgcc       ? defaultNixpkgs.avrgcc
, avrlibc      ? defaultNixpkgs.avrlibc
, stdenv       ? defaultNixpkgs.stdenv
}:

let
  # Nix's AVR packages don't automatically tell avrgcc how to find the avrlibc
  # headers and libraries, so we have to do that bit ourself. We define two
  # variables CPPFLAGS and LDFLAGS, which should be passed to avr-gcc
  # when preprocessing (copmiling) and linking respectively.
  avrlibcSetup = ''
    # There are a number of subdirectories under ${avrlibc}/avr/lib. We need
    # to select the one corresponding to the Uno's microcontroller (atmega328p).
    device="atmega328p"
    devicelib=$(find ${avrlibc}/avr/lib -name lib$device.a)
    if [ -z "$devicelib" ]; then
      echo >&2 "shell.nix: could not find lib$device.a"
      exit 1
    fi
    libdir=$(dirname $devicelib)
    export CPPFLAGS="-isystem ${avrlibc}/avr/include"
    export LDFLAGS="-B$libdir -L$libdir"
  '';
in

stdenv.mkDerivation {
  name = "servo-asm-fw";
  src = ./.;

  buildInputs = [
    # avr-gcc-ar from avrgcc is a wrapper around avr-ar from avrbinutils, and
    # needs to locate it at runtime.
    avrbinutils
  ];

  phases = "unpackPhase patchPhase buildPhase installPhase";

  # Clean up the build directory, if the project was build in a nix-shell and
  # not cleaned up.
  postUnpack = ''
    rm -rf $sourceRoot/build-*
  '';

  # Initialise environment variables for avrlibc.
  preBuild = avrlibcSetup;

  # Copy build artifacts to output directory.
  installPhase = ''
    runHook preInstall
    install -D -m 0644 build-*/*.{eep,elf,hex} -t $out
    runHook postInstall
  '';

  ARDUINO_DIR = "${arduino-core}/share/arduino";

  # Since the AVR tools needed by Arduino-Makefile are split across three
  # packages, we can't use AVR_TOOLS_DIR and instead must specify the path
  # to each tool explicitly.
  OVERRIDE_EXECUTABLES = "1";
  AVR_CC = "${avrgcc}/bin/avr-gcc";
  AVR_CXX = "${avrgcc}/bin/avr-g++";
  AVR_AS = "${avrbinutils}/bin/avr-as";
  AVR_OBJCOPY = "${avrbinutils}/bin/avr-objcopy";
  AVR_OBJDUMP = "${avrbinutils}/bin/avr-objdump";
  AVR_AR = "${avrgcc}/bin/avr-gcc-ar";
  AVR_SIZE = "${avrbinutils}/bin/avr-size";
  AVR_NM = "${avrgcc}/bin/avr-gcc-nm";
  AVR_RANLIB = "${avrgcc}/bin/avr-gcc-ranlib";
}
