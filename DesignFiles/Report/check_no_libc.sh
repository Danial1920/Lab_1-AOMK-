#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
OUT=${ECO_NO_LIBC_BUILD:-"$ROOT/BuildFiles/NoLibc"}
mkdir -p "$OUT"
command -v nm >/dev/null 2>&1 || { echo "NO LIBC CHECK FAILED: nm unavailable" >&2; exit 2; }
command -v rg >/dev/null 2>&1 || { echo "NO LIBC CHECK FAILED: rg unavailable" >&2; exit 2; }
# Compile both optimizer branches: compiler-emitted calls matter as much as
# direct source calls. Link the component objects together before checking
# undefined symbols, so only genuine external dependencies remain.
for level in 0 2; do
  for source in CEcoLab1Generated.c CEcoLab1GeneratedFactory.c CalculatorComponents.c EcoLab2.c polyfit_bus.c; do
    clang -std=c89 -Wall -Wextra -Werror -fno-stack-protector -fno-builtin -O"$level" -DECO_APPLE -DECO_AARCH64 -DECO_LIB \
      -I"$ROOT/DependenciesFiles/Eco.Core1/SharedFiles" \
      -I"$ROOT/DependenciesFiles/Eco.InterfaceBus1/SharedFiles" \
      -I"$ROOT/DependenciesFiles/Eco.MemoryManager1/SharedFiles" \
      -I"$ROOT/HeaderFiles" -I"$ROOT/SharedFiles" -c "$ROOT/SourceFiles/$source" -o "$OUT/${source%.c}_O$level.o"
  done
  # Directory paths can contain spaces; use positional arguments instead of
  # splitting paths containing whitespace.
  set -- "$OUT/CEcoLab1Generated_O$level.o" "$OUT/CEcoLab1GeneratedFactory_O$level.o" "$OUT/CalculatorComponents_O$level.o" "$OUT/EcoLab2_O$level.o" "$OUT/polyfit_bus_O$level.o"
  nm -g "$@" > "$OUT/symbols_O$level.txt"
  python3 - "$OUT/symbols_O$level.txt" <<'PY'
import sys
from pathlib import Path
undefined, defined = set(), set()
for line in Path(sys.argv[1]).read_text().splitlines():
    fields = line.split()
    if len(fields) >= 2 and fields[-2] == 'U':
        undefined.add(fields[-1])
    elif len(fields) >= 3 and fields[-2].upper() in {'T', 'D', 'B', 'S', 'R', 'A', 'C'}:
        defined.add(fields[-1])
external = sorted(undefined - defined)
if external:
    raise SystemExit('NO LIBC CHECK FAILED: unexpected external symbols: ' + ', '.join(external))
print('External component symbols: none')
PY
done
if rg -n '#[[:space:]]*include[[:space:]]*[<"](stdio|stdlib|math|string|memory)\.h|\b(malloc|calloc|realloc|free|memcpy|memmove|memset|memcmp|printf|puts|scanf|abort|exit|fabs|sqrt|pow)[[:space:]]*\(' "$ROOT/SourceFiles" "$ROOT/HeaderFiles" "$ROOT/SharedFiles"; then
  echo "NO LIBC CHECK FAILED: forbidden source dependency" >&2
  exit 1
fi
echo "NO LIBC CHECK PASSED: source + external symbols at O0/O2"
