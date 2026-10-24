#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
PROJECT="$ROOT/AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj"
FRAMEWORK=${ECO_FRAMEWORK:?set ECO_FRAMEWORK to the unpacked Eco.Framework}
OUT="$ROOT/BuildFiles/Mac/arm64 x86_64"

xcodebuild -project "$PROJECT" -scheme Dynamic -configuration Release \
  ECO_FRAMEWORK="$FRAMEWORK" CODE_SIGNING_ALLOWED=NO \
  PRODUCT_NAME=4D572607A26A48C1BBC94AB2CB6CFF86 \
  CONFIGURATION_BUILD_DIR="$OUT/DynamicRelease" \
  -derivedDataPath "$ROOT/BuildFiles/XcodeDynamicRuntimeNormal" build

xcodebuild -project "$PROJECT" -scheme Dynamic -configuration Release \
  ECO_FRAMEWORK="$FRAMEWORK" CODE_SIGNING_ALLOWED=NO \
  PRODUCT_NAME=4D572607A26A48C1BBC94AB2CB6CFF86QR \
  OTHER_CFLAGS=-DPOLYFIT_QR_ONLY \
  CONFIGURATION_BUILD_DIR="$OUT/DynamicReleaseQR" \
  -derivedDataPath "$ROOT/BuildFiles/XcodeDynamicRuntimeQR" build

xcodebuild -project "$PROJECT" -scheme Dynamic -configuration Release \
  ECO_FRAMEWORK="$FRAMEWORK" CODE_SIGNING_ALLOWED=NO \
  PRODUCT_NAME=4D572607A26A48C1BBC94AB2CB6CFF86LAB2 \
  OTHER_CFLAGS=-DECO_LAB2_ONLY \
  CONFIGURATION_BUILD_DIR="$OUT/DynamicReleaseLab2" \
  -derivedDataPath "$ROOT/BuildFiles/XcodeDynamicRuntimeLab2" build

echo "Normal CID: $OUT/DynamicRelease/4D572607A26A48C1BBC94AB2CB6CFF86.dylib"
echo "QR CID:     $OUT/DynamicReleaseQR/4D572607A26A48C1BBC94AB2CB6CFF86QR.dylib"
echo "Lab2 CID:   $OUT/DynamicReleaseLab2/4D572607A26A48C1BBC94AB2CB6CFF86LAB2.dylib"
