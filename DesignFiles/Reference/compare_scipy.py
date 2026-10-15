#!/usr/bin/env python3
"""Compare captured component coefficients with the SciPy reference cases."""
import re
from pathlib import Path

text = Path('DesignFiles/Reference/component_output.txt').read_text()
rows = []
for line in text.splitlines():
    match = re.search(r'degree=(\d+) coefficients=\[(.*)\]', line)
    if match:
        vals = [float(v.strip()) for v in match.group(2).split(',')]
        rows.append((int(match.group(1)), vals))
expected = {
    2: [2.0, 0.0, 3.0],
    3: [1.0, -2.0, 0.5, 3.0],
}
if len(rows) != 4:
    raise SystemExit('expected four component result rows, got %d' % len(rows))
max_error = 0.0
for degree, actual in rows:
    ref = expected[degree]
    error = max(abs(a - b) for a, b in zip(actual, ref))
    max_error = max(max_error, error)
    print('degree=%d max_abs_error=%.17g' % (degree, error))
print('component_vs_scipy max_abs_error=%.17g' % max_error)
if max_error > 1.0e-12:
    raise SystemExit(1)
