#!/bin/sh
# Build one loadable factory per real CID, then exercise the official Eco loader.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
PROJECT="$ROOT/AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj"
FRAMEWORK=${ECO_FRAMEWORK:?set ECO_FRAMEWORK to the unpacked Eco.Framework}
CONFIGURATION=${CONFIGURATION:-Release}
case "$CONFIGURATION" in Debug|Release) ;; *) echo "CONFIGURATION must be Debug or Release" >&2; exit 2;; esac
RUNTIME="$ROOT/BuildFiles/DynamicRuntime/$CONFIGURATION"
LOGS="$ROOT/BuildFiles/DynamicRuntimeLogs/$CONFIGURATION"
mkdir -p "$RUNTIME" "$LOGS"

build_factory() {
    cid=$1
    selector=$2
    echo "Building $CONFIGURATION $cid"
    if ! xcodebuild -quiet -project "$PROJECT" -scheme Dynamic \
        -configuration "$CONFIGURATION" -jobs 1 \
        -derivedDataPath "$ROOT/BuildFiles/XcodeDynamicRuntime/$CONFIGURATION/$cid" \
        ECO_FRAMEWORK="$FRAMEWORK" CODE_SIGNING_ALLOWED=NO \
        PRODUCT_NAME="$cid" CONFIGURATION_BUILD_DIR="$RUNTIME" \
        "OTHER_CFLAGS=-fno-stack-protector -fno-builtin $selector" \
        build >"$LOGS/$cid.build.log" 2>&1; then
        cat "$LOGS/$cid.build.log" >&2
        exit 1
    fi
    test -s "$RUNTIME/$cid.dylib"
    "$ROOT/DesignFiles/Report/check_dynamic_deps.sh" "$RUNTIME/$cid.dylib" \
        >"$LOGS/$cid.dependencies.log" 2>&1
}

build_factory D3B3E2F18D214A6B9A40440000000045 ""
build_factory D3B3E2F18D214A6B9A40440000000046 -DPOLYFIT_QR_ONLY
build_factory D3B3E2F18D214A6B9A40440000000060 -DECO_CALCULATOR_KIND=0
build_factory D3B3E2F18D214A6B9A40440000000061 -DECO_CALCULATOR_KIND=1
build_factory D3B3E2F18D214A6B9A40440000000062 -DECO_CALCULATOR_KIND=2
build_factory D3B3E2F18D214A6B9A40440000000063 -DECO_CALCULATOR_KIND=3
build_factory D3B3E2F18D214A6B9A40440000000064 -DECO_CALCULATOR_KIND=4
build_factory D3B3E2F18D214A6B9A40440000000065 -DECO_LAB2_ONLY

# This executable links no laboratory sources or static laboratory libraries.
CC=${CC:-clang}
"$CC" -std=c89 -Wall -Wextra -fno-stack-protector -DECO_APPLE \
    -DECO_AARCH64 -DECO_LIB \
    -I"$ROOT/SharedFiles" -I"$ROOT/HeaderFiles" \
    -I"$FRAMEWORK/Eco.Core1/SharedFiles" \
    -I"$FRAMEWORK/Eco.System1/SharedFiles" \
    -I"$FRAMEWORK/Eco.InterfaceBus1/SharedFiles" \
    -I"$FRAMEWORK/Eco.MemoryManager1/SharedFiles" \
    -I"$FRAMEWORK/Eco.FileSystemManagement1/SharedFiles" \
    "$ROOT/UnitTestFiles/SourceFiles/DynamicRuntimeTest.c" \
    "$FRAMEWORK/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a" \
    "$FRAMEWORK/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a" \
    "$FRAMEWORK/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a" \
    "$FRAMEWORK/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a" \
    -o "$RUNTIME/EcoLab2DynamicTest"

if ! ECO_FRAMEWORK_RT="$RUNTIME" ECO_LAB_RUNTIME="$RUNTIME" DYLD_PRINT_LIBRARIES=1 \
    "$RUNTIME/EcoLab2DynamicTest" >"$LOGS/runtime.log" 2>"$LOGS/loader.log"; then
    cat "$LOGS/runtime.log" >&2
    cat "$LOGS/loader.log" >&2
    exit 1
fi
for suffix in 45 46 60 61 62 63 64 65; do
    grep -F "$RUNTIME/D3B3E2F18D214A6B9A404400000000$suffix.dylib" \
        "$LOGS/loader.log" >/dev/null
done
cat "$LOGS/runtime.log"

# A fresh process with an empty runtime must fail, proving there is no static
# laboratory fallback or previously registered factory in the executable.
EMPTY="$ROOT/BuildFiles/DynamicRuntime/Empty"
mkdir -p "$EMPTY"
if ECO_FRAMEWORK_RT="$EMPTY" ECO_LAB_RUNTIME="$EMPTY" \
    "$RUNTIME/EcoLab2DynamicTest" >"$LOGS/missing-runtime.log" 2>&1; then
    echo "FAIL: dynamic test succeeded with no laboratory dylibs" >&2
    exit 1
fi
echo "Missing runtime: expected nonzero exit confirmed"
echo "Runtime directory: $RUNTIME"
echo "Build and loader logs: $LOGS"
