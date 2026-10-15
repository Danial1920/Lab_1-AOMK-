#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
OUT="$ROOT/BuildFiles/StrictC89"
CC=${CC:-gcc}
mkdir -p "$OUT"
COMMON="-std=c89 -pedantic -Wall -Wextra -Wcast-qual -Werror -fno-stack-protector"
for source in CEcoLab1Generated.c CEcoLab1GeneratedFactory.c polyfit_bus.c; do
  # Eco.Core1 contains legacy anonymous unions; suppress only that upstream warning.
  $CC $COMMON -Wno-c11-extensions -DECO_APPLE \
    "-I$ROOT/DependenciesFiles/Eco.Core1/SharedFiles" \
    "-I$ROOT/DependenciesFiles/Eco.InterfaceBus1/SharedFiles" \
    "-I$ROOT/DependenciesFiles/Eco.MemoryManager1/SharedFiles" \
    "-I$ROOT/HeaderFiles" "-I$ROOT/SharedFiles" \
    -c "$ROOT/SourceFiles/$source" -o "$OUT/${source%.c}.o"
done
printf '%s\n' "STRICT C89 CHECK PASSED (component sources; upstream Eco headers excluded from warning policy)"
