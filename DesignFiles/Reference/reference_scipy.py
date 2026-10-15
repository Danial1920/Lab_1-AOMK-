#!/usr/bin/env python3
"""SciPy cross-check. It is optional when SciPy is unavailable on macOS."""
try:
    import numpy as np
except ImportError:
    print('NUMPY CHECK SKIPPED: numpy is not installed')
else:
    cases = [
        ([-2, -1, 0, 1, 2], [11, 5, 3, 5, 11], 2),
        ([-2, -1, 0, 1, 2, 3], [-14, -0.5, 3, 2.5, 4, 13.5], 3),
    ]
    try:
        from scipy import __version__ as scipy_version
    except ImportError:
        print('SCIPY CHECK SKIPPED: scipy is not installed; NumPy reference follows')
    else:
        print('SciPy %s' % scipy_version)
    for x, y, degree in cases:
        print('degree=%d coefficients=%s' % (degree, np.polyfit(x, y, degree)))
