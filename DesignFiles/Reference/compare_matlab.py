#!/usr/bin/env python3
"""Compare captured component coefficients with MATLAB polyfit output."""
import re
from pathlib import Path

component_text = Path('DesignFiles/Reference/component_output.txt').read_text()
matlab_text = Path('DesignFiles/Reference/matlab_output.txt').read_text()

component_rows = []
for line in component_text.splitlines():
    match = re.search(r'degree=(\d+) coefficients=\[(.*)\]', line)
    if match:
        values = [float(value.strip()) for value in match.group(2).split(',')]
        component_rows.append((int(match.group(1)), values))

matlab_rows = {}
for line in matlab_text.splitlines():
    match = re.search(r'degree=(\d+) coefficients=\s*(.*)', line)
    if match:
        matlab_rows[int(match.group(1))] = [float(value) for value in match.group(2).split()]

if len(component_rows) != 4:
    raise SystemExit('expected four component result rows, got %d' % len(component_rows))
if set(matlab_rows) != {0, 2, 3}:
    raise SystemExit('expected MATLAB degree rows 0, 2 and 3')

max_error = 0.0
for degree, actual in component_rows:
    reference = matlab_rows[degree]
    error = max(abs(a - b) for a, b in zip(actual, reference))
    max_error = max(max_error, error)
    print('degree=%d max_abs_error=%.17g' % (degree, error))

print('component_vs_matlab max_abs_error=%.17g' % max_error)
if max_error > 1.0e-12:
    raise SystemExit(1)
