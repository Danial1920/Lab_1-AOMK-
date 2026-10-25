#!/usr/bin/env python3
"""Rebuild the actual component and compare with real SciPy and NumPy solves."""
import json
import numpy as np
import scipy
from scipy import linalg
from reference_cases import ROOT, CASES, fresh_component, normalized_case, restore_coefficients, compare

def references():
    scipy_rows, numpy_rows = {}, {}
    for name in CASES:
        z, rhs, degree, transform = normalized_case(name)
        design = np.vander(np.asarray(z, dtype=float), degree+1)
        coefficients, _, rank, _ = linalg.lstsq(design, np.asarray(rhs), lapack_driver='gelsd')
        if rank != degree+1:
            raise ValueError('SciPy rank deficient reference: ' + name)
        scipy_rows[name] = restore_coefficients(coefficients, transform)
        coefficients, _, rank, _ = np.linalg.lstsq(design, np.asarray(rhs), rcond=None)
        if rank != degree+1:
            raise ValueError('NumPy rank deficient reference: ' + name)
        numpy_rows[name] = restore_coefficients(coefficients, transform)
    return scipy_rows, numpy_rows

def main():
    component = fresh_component()
    scipy_rows, numpy_rows = references()
    lines = ['SciPy %s scipy.linalg.lstsq; NumPy %s numpy.linalg.lstsq' % (scipy.__version__, np.__version__)]
    lines += compare(component, scipy_rows, 'SciPy')
    lines += compare(component, numpy_rows, 'NumPy')
    lines.append('REFERENCE COMPARISON PASSED: freshly built component; %d cases x 2 algorithms x 2 references' % len(CASES))
    output = '\n'.join(lines)+'\n'
    (ROOT/'DesignFiles/Reference/scipy_comparison.txt').write_text(output)
    (ROOT/'DesignFiles/Reference/scipy_output.txt').write_text(json.dumps({'scipy_version':scipy.__version__, 'numpy_version':np.__version__, 'scipy':scipy_rows, 'numpy':numpy_rows}, indent=2)+'\n')
    print(output, end='')
if __name__ == '__main__':
    main()
