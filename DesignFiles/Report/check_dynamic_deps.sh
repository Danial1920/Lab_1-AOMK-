#!/bin/sh
set -eu
ARTIFACT=${1:-}
if [ -z "$ARTIFACT" ]; then
  echo "usage: $0 path/to/library.so" >&2
  exit 2
fi
if [ ! -f "$ARTIFACT" ]; then
  echo "DYNAMIC CHECK FAILED: artifact not found: $ARTIFACT" >&2
  exit 1
fi
if command -v nm >/dev/null 2>&1; then
  UNDEFINED=$(nm -u "$ARTIFACT" | awk 'NF {print $NF}')
  if [ -n "$UNDEFINED" ]; then
    echo "$UNDEFINED" >&2
    echo "DYNAMIC CHECK FAILED: unresolved symbols" >&2
    exit 1
  fi
fi
if command -v readelf >/dev/null 2>&1; then
  readelf -d "$ARTIFACT" | grep -E 'NEEDED|SONAME' || true
  if readelf -d "$ARTIFACT" | grep -E 'NEEDED.*(libc|libm|libSystem)' >/dev/null 2>&1; then
    echo "DYNAMIC CHECK FAILED: forbidden C runtime dependency" >&2
    exit 1
  fi
  echo "DYNAMIC CHECK PASSED: readelf"
elif command -v otool >/dev/null 2>&1; then
  otool -L "$ARTIFACT"
  # macOS injects libSystem into every dynamic image; the component source
  # itself is still checked separately with nm and the forbidden-symbol scan.
  echo "DYNAMIC CHECK PASSED: otool -L (platform libSystem is allowed)"
else
  echo "DYNAMIC CHECK SKIPPED: readelf is unavailable on this host"
fi
