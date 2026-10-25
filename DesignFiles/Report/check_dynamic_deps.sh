#!/bin/sh
set -eu
ARTIFACT=${1:-}
if [ -z "$ARTIFACT" ]; then
  echo "usage: $0 path/to/library.dylib-or-so" >&2
  exit 2
fi
if [ ! -f "$ARTIFACT" ]; then
  echo "DYNAMIC CHECK FAILED: artifact not found: $ARTIFACT" >&2
  exit 1
fi
command -v nm >/dev/null 2>&1 || { echo 'DYNAMIC CHECK FAILED: nm unavailable' >&2; exit 2; }
command -v file >/dev/null 2>&1 || { echo 'DYNAMIC CHECK FAILED: file unavailable' >&2; exit 2; }
KIND=$(file -b "$ARTIFACT")
TEMP=$(mktemp -d "${TMPDIR:-/tmp}/eco_dynamic_check.XXXXXX")
trap 'rm -rf "$TEMP"' EXIT HUP INT TERM
case "$KIND" in
  *Mach-O*)
    command -v otool >/dev/null 2>&1 || { echo 'DYNAMIC CHECK FAILED: otool unavailable' >&2; exit 2; }
    nm -arch all -g "$ARTIFACT" > "$TEMP/symbols"
    awk 'NF >= 2 && $(NF-1) == "U" {print $NF}' "$TEMP/symbols" > "$TEMP/undefined"
    if [ -s "$TEMP/undefined" ]; then
      cat "$TEMP/undefined" >&2
      echo 'DYNAMIC CHECK FAILED: unresolved symbols in at least one architecture' >&2
      exit 1
    fi
    otool -L "$ARTIFACT" > "$TEMP/dependencies"
    cat "$TEMP/dependencies"
    # LC_ID_DYLIB is the first dependency line in each architecture. The
    # system loader injects libSystem; no other runtime dependency is allowed.
    python3 - "$ARTIFACT" "$TEMP/dependencies" <<'PY'
import sys
from pathlib import Path
artifact = Path(sys.argv[1])
headers = 0
first = True
for line in Path(sys.argv[2]).read_text().splitlines():
    if not line[:1].isspace():
        headers += 1
        first = True
        continue
    dependency = line.strip().split(' (compatibility version', 1)[0]
    if first:
        if Path(dependency).name != artifact.name:
            raise SystemExit('DYNAMIC CHECK FAILED: unexpected image ID '+dependency)
        first = False
    elif dependency != '/usr/lib/libSystem.B.dylib':
        raise SystemExit('DYNAMIC CHECK FAILED: unexpected dependency '+dependency)
if headers == 0:
    raise SystemExit('DYNAMIC CHECK FAILED: no Mach-O architecture records')
PY
    echo 'DYNAMIC CHECK PASSED: every Mach-O architecture, symbols and dependencies'
    ;;
  *ELF*)
    command -v readelf >/dev/null 2>&1 || { echo 'DYNAMIC CHECK FAILED: readelf unavailable' >&2; exit 2; }
    nm -u "$ARTIFACT" > "$TEMP/undefined"
    if [ -s "$TEMP/undefined" ]; then
      cat "$TEMP/undefined" >&2
      echo 'DYNAMIC CHECK FAILED: unresolved ELF symbols' >&2
      exit 1
    fi
    readelf -d "$ARTIFACT" > "$TEMP/dependencies"
    cat "$TEMP/dependencies"
    if grep 'NEEDED' "$TEMP/dependencies" >/dev/null 2>&1; then
      echo 'DYNAMIC CHECK FAILED: unexpected external runtime dependency' >&2
      exit 1
    fi
    echo 'DYNAMIC CHECK PASSED: ELF symbols and dependencies'
    ;;
  *) echo "DYNAMIC CHECK FAILED: unsupported binary format: $KIND" >&2; exit 2;;
esac
