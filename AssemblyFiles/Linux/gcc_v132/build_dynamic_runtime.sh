#!/usr/bin/env bash
set -eu
DIR=$(cd "$(dirname "$0")"; pwd)
ROOT=$(cd "$DIR/../../.."; pwd)
ARCH=${ARCH:-x86_64}
DEBUG=${DEBUG:-0}
OUT_DIR="$ROOT/BuildFiles/Linux/$ARCH/Dynamic$(if [ "$DEBUG" = 1 ]; then echo Debug; else echo Release; fi)"
mkdir -p "$OUT_DIR"

make -C "$DIR" TARGET=0 DEBUG="$DEBUG" ARCH="$ARCH" \
    OUT_TARGET=4D572607A26A48C1BBC94AB2CB6CFF86.so \
    EXTRA_DEFINES= 
make -C "$DIR" TARGET=0 DEBUG="$DEBUG" ARCH="$ARCH" \
    OUT_TARGET=4D572607A26A48C1BBC94AB2CB6CFF86QR.so \
    EXTRA_DEFINES=-DPOLYFIT_QR_ONLY

echo "Dynamic runtime artifacts:"
echo "  $OUT_DIR/4D572607A26A48C1BBC94AB2CB6CFF86.so"
echo "  $OUT_DIR/4D572607A26A48C1BBC94AB2CB6CFF86QR.so"
