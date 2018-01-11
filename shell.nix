{ pkgs ? import <nixpkgs> {}
, withUploadSupport ? true
, withMonitorSupport ? true
}:

with pkgs;

let
  buildDeriv = callPackage ./default.nix {};

  pythonWithPyserial = python.withPackages (pyPkgs: [
    pyPkgs.pyserial
  ]);
in

buildDeriv.overrideDerivation (oldArgs: {
  buildInputs = oldArgs.buildInputs
    ++ stdenv.lib.optional withUploadSupport pythonWithPyserial
    ++ stdenv.lib.optional withMonitorSupport screen;

  AVRDUDE = if withUploadSupport then "${avrdude}/bin/avrdude" else null;
  AVRDUDE_CONF = if withUploadSupport then "${avrdude}/etc/avrdude.conf" else null;

  # This hook sets up the environment so that avrgcc can find avrlibc.
  shellHook = oldArgs.preBuild;
})
