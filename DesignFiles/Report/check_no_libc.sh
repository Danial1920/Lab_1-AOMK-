#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
OUT=${ECO_NO_LIBC_BUILD:-"$ROOT/BuildFiles/NoLibc"}
mkdir -p "$OUT"
command -v nm >/dev/null 2>&1 || { echo "NO LIBC CHECK FAILED: nm unavailable" >&2; exit 2; }
command -v python3 >/dev/null 2>&1 || { echo "NO LIBC CHECK FAILED: python3 unavailable" >&2; exit 2; }
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
# Python is already required for the symbol audit above. Use its standard
# library for source scanning too, so a normal terminal need not install rg.
python3 - "$ROOT" <<'PY'
import re
import sys
from pathlib import Path

root = Path(sys.argv[1])
pattern = re.compile(
    r'#[\s]*include[\s]*[<"](stdio|stdlib|math|string|memory)\.h'
    r'|\b(malloc|calloc|realloc|free|memcpy|memmove|memset|memcmp|printf|puts|scanf|abort|exit|fabs|sqrt|pow)\s*\('
)
found = False
for directory in ('SourceFiles', 'HeaderFiles', 'SharedFiles'):
    for path in sorted((root / directory).rglob('*')):
        if not path.is_file() or path.suffix not in ('.c', '.h', '.idl'):
            continue
        for number, line in enumerate(path.read_text(encoding='utf-8-sig').splitlines(), 1):
            if pattern.search(line):
                print('{}:{}:{}'.format(path, number, line))
                found = True
if found:
    raise SystemExit('NO LIBC CHECK FAILED: forbidden source dependency')
PY
echo "NO LIBC CHECK PASSED: source + external symbols at O0/O2"
