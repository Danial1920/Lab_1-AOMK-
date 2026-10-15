#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
OUT="$ROOT/BuildFiles/NoLibc"
mkdir -p "$OUT"
for source in CEcoLab1Generated.c CEcoLab1GeneratedFactory.c polyfit_bus.c; do
  clang -std=c89 -Wall -Wextra -Werror -fno-stack-protector -DECO_APPLE \
    -I"$ROOT/DependenciesFiles/Eco.Core1/SharedFiles" \
    -I"$ROOT/DependenciesFiles/Eco.InterfaceBus1/SharedFiles" \
    -I"$ROOT/DependenciesFiles/Eco.MemoryManager1/SharedFiles" \
    -I"$ROOT/HeaderFiles" -I"$ROOT/SharedFiles" -c "$ROOT/SourceFiles/$source" -o "$OUT/${source%.c}.o"
done
nm -u "$OUT"/*.o | tee "$OUT/undefined.txt"
if rg -n '#include[[:space:]]*[<"](stdio|stdlib|math|string|memory)\.h|\b(malloc|calloc|realloc|free|memcpy|memset|memcmp|printf|scanf|fabs|sqrt|pow)[[:space:]]*\(' "$ROOT/SourceFiles" "$ROOT/HeaderFiles" "$ROOT/SharedFiles"; then
  echo "forbidden libc dependency found" >&2
  exit 1
fi
echo "NO LIBC CHECK PASSED"
