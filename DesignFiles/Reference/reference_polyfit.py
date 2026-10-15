#!/usr/bin/env python3
"""Independent standard-library reference for task 44 (MATLAB order)."""

def solve(a, b):
    n = len(b)
    for col in range(n):
        pivot = max(range(col, n), key=lambda row: abs(a[row][col]))
        if abs(a[pivot][col]) < 1e-14:
            raise ValueError("singular")
        a[col], a[pivot] = a[pivot], a[col]
        b[col], b[pivot] = b[pivot], b[col]
        for row in range(col + 1, n):
            factor = a[row][col] / a[col][col]
            for k in range(col, n):
                a[row][k] -= factor * a[col][k]
            b[row] -= factor * b[col]
    x = [0.0] * n
    for row in range(n - 1, -1, -1):
        x[row] = (b[row] - sum(a[row][k] * x[k] for k in range(row + 1, n))) / a[row][row]
    return x

def polyfit(x, y, degree):
    order = degree + 1
    gram = [[0.0 for _ in range(order)] for _ in range(order)]
    rhs = [0.0 for _ in range(order)]
    for xv, yv in zip(x, y):
        powers = [xv ** p for p in range(2 * degree + 1)]
        for row in range(order):
            rhs[row] += yv * powers[row]
            for col in range(order):
                gram[row][col] += powers[row + col]
    # Normal equations are ascending powers; reverse for MATLAB convention.
    return list(reversed(solve(gram, rhs)))

CASES = [
    ([-2, -1, 0, 1, 2], [11, 5, 3, 5, 11], 2),
    ([-2, -1, 0, 1, 2, 3], [-14, -0.5, 3, 2.5, 4, 13.5], 3),
    ([-10, -2, 4, 9], [7, 7, 7, 7], 0),
]

if __name__ == '__main__':
    for x, y, degree in CASES:
        print('degree=%d coefficients=%s' % (degree, polyfit(x, y, degree)))
