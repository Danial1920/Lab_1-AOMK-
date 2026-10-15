#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
if [ -z "${ECO_TOOLCHAIN:-}" ]; then
  echo "PLATFORM MATRIX SKIPPED: set ECO_TOOLCHAIN to an Eco Framework GNU toolchain"
  exit 0
fi
for target in 0 1; do
  for debug in 0 1; do
    make -C "$ROOT/AssemblyFiles/Linux/gcc_v132" TARGET="$target" DEBUG="$debug" ARCH=x86_64
  done
done
echo "PLATFORM MATRIX PASSED: Linux x86_64 TARGET=0/1 DEBUG=0/1"
