"""Shared test datasets and strict validation for independent executable references."""
import json
import math
import subprocess
import tempfile
from decimal import Decimal, localcontext
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CASES = {
    'quadratic': ([-2, -1, 0, 1, 2], [11, 5, 3, 5, 11], 2, 1e-11, 1e-10),
    'cubic': ([-2, -1, 0, 1, 2, 3], [-14, -.5, 3, 2.5, 4, 13.5], 3, 1e-11, 1e-10),
    'constant': ([-10, -2, 4, 9], [7, 7, 7, 7], 0, 1e-12, 1e-12),
    'large_offset': ([1000000+i for i in range(5)], [2000003+2*i for i in range(5)], 1, 1e-3, 1e-9),
    'tiny_identity': ([-1e-310, 0, 1e-310], [-1e-310, 0, 1e-310], 1, 1e-320, 1e-10),
    'huge_x_constant': ([1e308]*3, [7]*3, 0, 1e-12, 1e-12),
    'huge_y_constant': ([-2, -1, 0, 1, 2], [1e308]*5, 0, 0.0, 1e-12),
    'noisy_linear': ([-2, -1, 0, 1, 2], [-.9, .9, 3, 5.1, 6.9], 1, 1e-11, 1e-10),
    # The slope and intercept have different units. A floating reference at
    # y~1e308 resolves the original-basis intercept only to ~1e292; the slope
    # retains its strict 1e-12 bound. The C regression also asserts intercept==0.
    'opposite_extremes': ([-1e308, 0, 1e308], [-1e308, 0, 1e308], 1, [1e-12, 1e294], 1e-12),
}

def parse_component(text):
    rows = {}
    for line in text.splitlines():
        if not line.startswith('REFERENCE '):
            continue
        row = json.loads(line[len('REFERENCE '):])
        if set(row) != {'case', 'method', 'coefficients'}:
            raise ValueError('Unexpected component row fields')
        name, method, values = row['case'], row['method'], row['coefficients']
        if name not in CASES or type(method) is not int or method not in (0, 1):
            raise ValueError('Unknown case or algorithm')
        key = (name, method)
        if key in rows:
            raise ValueError('Duplicate component result: %r' % (key,))
        validate_values(name, values)
        rows[key] = values
    if set(rows) != {(name, method) for name in CASES for method in (0, 1)}:
        raise ValueError('Component output is missing required cases or algorithms')
    return rows

def validate_values(name, values):
    if not isinstance(values, list) or len(values) != CASES[name][2] + 1:
        raise ValueError('Wrong coefficient count for ' + name)
    if any(type(v) not in (int, float) or not math.isfinite(v) for v in values):
        raise ValueError('Non-finite/non-numeric coefficient for ' + name)

def fresh_component():
    with tempfile.TemporaryDirectory(prefix='eco_reference_') as temporary:
        build = Path(temporary) / 'host'
        subprocess.run(['make', '-B', 'BUILD=' + str(build), 'all'], cwd=ROOT, check=True,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        result = subprocess.run([str(build / 'EcoLab1GeneratedUnitTest')], cwd=ROOT, check=True,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    rows = parse_component(result.stdout)
    (ROOT / 'DesignFiles/Reference/component_output.txt').write_text(result.stdout)
    return rows

def normalized_case(name):
    """Use Decimal only for conditioning and coefficient basis conversion.

    The overdetermined solve itself is performed by SciPy/NumPy/MATLAB, never
    by the component or a home-grown normal-equation solver.
    """
    x, y, degree, _, _ = CASES[name]
    with localcontext() as context:
        context.prec = 90
        xd = [Decimal(str(v)) for v in x]
        yd = [Decimal(str(v)) for v in y]
        center = (min(xd) + max(xd)) / 2
        scale = max(abs(v-center) for v in xd) or Decimal(1)
        ycenter = (min(yd) + max(yd)) / 2
        yscale = max(abs(v-ycenter) for v in yd) or Decimal(1)
        z = [float((v-center)/scale) for v in xd]
        rhs = [float((v-ycenter)/yscale) for v in yd]
    return z, rhs, degree, (center, scale, ycenter, yscale)

def restore_coefficients(coefficients, transform):
    with localcontext() as context:
        context.prec = 90
        center, scale, ycenter, yscale = transform
        result = [Decimal(0)]
        for coefficient in coefficients:
            next_values = [Decimal(0)] * (len(result)+1)
            for power, value in enumerate(result):
                next_values[power] -= value*center/scale
                next_values[power+1] += value/scale
            next_values[0] += Decimal(str(float(coefficient)))
            result = next_values
        # The first Horner iteration creates a redundant leading zero.
        result = result[:-1]
        result = [v*yscale for v in result]
        result[0] += ycenter
        return [float(v) for v in reversed(result)]

def compare(component, references, label):
    if set(references) != set(CASES):
        raise ValueError('Reference case set does not match the manifest')
    for name, values in references.items():
        validate_values(name, values)
    lines = []
    for (name, method), actual in sorted(component.items()):
        reference = references[name]
        atol, rtol = CASES[name][3:]
        for index, (a, b) in enumerate(zip(actual, reference)):
            # math.isclose handles scale, validates both finite sides above.
            absolute = atol[index] if isinstance(atol, list) else atol
            if not math.isclose(a, b, rel_tol=rtol, abs_tol=absolute):
                raise AssertionError('%s %s method%d coefficient%d: %.17g vs %.17g' %
                                     (label, name, method, index, a, b))
        lines.append('%s case=%s method=%d PASS atol=%s rtol=%g' % (label, name, method, atol, rtol))
    return lines
