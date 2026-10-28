#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
FRAMEWORK=${ECO_FRAMEWORK:?set ECO_FRAMEWORK for real Eco sanitizer integration}
OUT=${ECO_SANITIZER_BUILD:-BuildFiles/Sanitizers}
case "$OUT" in /*) RUN="$OUT";; *) RUN="$ROOT/$OUT";; esac
FLAGS='-std=c89 -Wall -Wextra -Werror -fno-stack-protector -DECO_APPLE -fsanitize=address,undefined -fno-omit-frame-pointer -g'
# LeakSanitizer is unavailable on Darwin. Actual allocator balance is asserted
# separately by both test binaries, including every injected failure path.
case $(uname -s) in Darwin) LEAKS=0;; *) LEAKS=1;; esac
export ASAN_OPTIONS="detect_leaks=$LEAKS:halt_on_error=1:abort_on_error=1"
export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
for variant in A C; do
    make -C "$ROOT" BUILD="$OUT/$variant" CALCULATOR_VARIANT="$variant" CFLAGS="$FLAGS" all eco-build ECO_FRAMEWORK="$FRAMEWORK"
    "$RUN/$variant/EcoLab1GeneratedUnitTest"
    "$RUN/$variant/EcoLab1EcoTest"
done
printf '%s\n' "SANITIZER CHECK PASSED: Host + real Eco integration, A/C; allocator balance asserted"
