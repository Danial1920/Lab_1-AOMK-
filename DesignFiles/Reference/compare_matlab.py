#!/usr/bin/env python3
"""Execute installed MATLAB and compare with freshly built component results."""
import argparse
import json
import os
import shutil
import subprocess
import tempfile
from pathlib import Path
from reference_cases import ROOT, CASES, fresh_component, normalized_case, restore_coefficients, compare

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--matlab', default=os.environ.get('MATLAB', '/Applications/MATLAB_R2026b.app/bin/matlab'))
    parser.add_argument('--timeout', type=int, default=600)
    args = parser.parse_args()
    executable = shutil.which(args.matlab)
    if executable is None:
        raise SystemExit('MATLAB unavailable: set MATLAB to the installed executable')
    component = fresh_component()
    transforms = {}
    with tempfile.TemporaryDirectory(prefix='eco_matlab_') as temporary:
        temporary = Path(temporary)
        script = temporary/'compare_actual.m'
        output = temporary/'results.json'
        def vector(values):
            return '['+' '.join('%.17g' % value for value in values)+']'
        statements = ['rows = struct();', "rows.version = version;", 'rows.cases = struct();']
        for name in CASES:
            z, rhs, degree, transform = normalized_case(name)
            transforms[name] = transform
            statements.append('rows.cases.%s = polyfit(%s, %s, %d);' % (name, vector(z), vector(rhs), degree))
        statements += ["fid = fopen('%s', 'w');" % str(output).replace("'", "''"),
                       "assert(fid ~= -1);", "fprintf(fid, '%s', jsonencode(rows));", 'fclose(fid);']
        script.write_text('\n'.join(statements)+'\n')
        result = subprocess.run([executable, '-batch', "run('%s')" % str(script).replace("'", "''")],
                                cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                timeout=args.timeout)
        (ROOT/'DesignFiles/Reference/matlab_output.txt').write_text(result.stdout.rstrip()+"\n")
        if result.returncode != 0:
            raise SystemExit('MATLAB exited %d; see matlab_output.txt' % result.returncode)
        data = json.loads(output.read_text())
    if set(data['cases']) != set(CASES):
        raise ValueError('Wrong MATLAB reference case set')
    rows = {}
    for name, coefficients in data['cases'].items():
        if not isinstance(coefficients, list): coefficients = [coefficients]
        rows[name] = restore_coefficients(coefficients, transforms[name])
    lines = ['MATLAB '+data['version'], 'Reference input conditioning is recorded in reference_cases.py.']
    lines += compare(component, rows, 'MATLAB')
    lines.append('MATLAB COMPARISON PASSED: %d cases x 2 component algorithms' % len(CASES))
    text = '\n'.join(lines)+'\n'
    (ROOT/'DesignFiles/Reference/matlab_comparison.txt').write_text(text)
    (ROOT/'DesignFiles/Reference/matlab_results.json').write_text(json.dumps({'version':data['version'], 'cases':rows}, indent=2)+'\n')
    (ROOT/'DesignFiles/Reference/matlab_status.txt').write_text('MATLAB executed with exit code 0; fresh component compared.\n'+text)
    print(text, end='')
if __name__ == '__main__':
    main()
