#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
OUT="$ROOT/BuildFiles/Sanitizers/EcoLab1GeneratedUnitTest"
mkdir -p "$(dirname "$OUT")"
clang -std=c89 -Wall -Wextra -fno-stack-protector -DECO_APPLE \
  "-I$ROOT/DependenciesFiles/Eco.Core1/SharedFiles" \
  "-I$ROOT/DependenciesFiles/Eco.InterfaceBus1/SharedFiles" \
  "-I$ROOT/DependenciesFiles/Eco.MemoryManager1/SharedFiles" \
  "-I$ROOT/HeaderFiles" "-I$ROOT/SharedFiles" \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  "$ROOT/SourceFiles/CEcoLab1Generated.c" \
  "$ROOT/SourceFiles/CEcoLab1GeneratedFactory.c" \
  "$ROOT/SourceFiles/polyfit_bus.c" \
  "$ROOT/UnitTestFiles/SourceFiles/HostTests.c" -o "$OUT"
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 "$OUT"
printf '%s\n' "SANITIZER CHECK PASSED"
