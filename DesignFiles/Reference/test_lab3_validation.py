"""Ensure the real event client rejects individually introduced server defects."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
framework = os.environ.get('ECO_FRAMEWORK', str(Path.home() / 'Downloads/Eco.Framework'))
faults = [
    ('missing Gaussian step', 'CEcoLab1Generated.c',
     'lab1_pivot(connections, i, p, a[p][i]);', '(void)0;', 'sink->used =='),
    ('wrong rotation parameter', 'CEcoLab1Generated.c',
     'lab1_rotation(connections, n, i, c, s);', 'lab1_rotation(connections, n, i, c, -s);', 'r->s'),
    ('silent subscription', 'Lab1Connections.c',
     'sink->pVTbl->call;', 'if (node->cookie != 2u) sink->pVTbl->call;', 'sink->used =='),
    ('acknowledged but still connected', 'Lab1Connections.c',
     'sink = node->sink;\n        node->sink = 0;', 'sink = node->sink;\n        if (sink) return 0;\n        node->sink = 0;', 'Unadvise'),
]
evidence = []
with tempfile.TemporaryDirectory(prefix='eco3-negative-') as temp:
    copy = Path(temp)
    shutil.copytree(root/'SourceFiles', copy/'SourceFiles')
    for directory in ('HeaderFiles','SharedFiles','UnitTestFiles','DependenciesFiles'):
        (copy/directory).symlink_to(root/directory, target_is_directory=True)
    shutil.copy2(root/'Makefile', copy/'Makefile')
    command = ['make', 'ECO_FRAMEWORK='+framework, 'lab3-unit']
    clean = subprocess.run(command, cwd=copy, text=True, capture_output=True)
    if clean.returncode or 'LAB3 PASS:' not in clean.stdout:
        raise SystemExit(clean.stdout + clean.stderr)
    for name, filename, original, replacement, expected in faults:
        source = copy/'SourceFiles'/filename
        baseline = source.read_bytes()
        code = source.read_text()
        if code.count(original) != 1:
            raise SystemExit('Mutation site changed: ' + name)
        source.write_text(code.replace(original, replacement))
        result = subprocess.run(command, cwd=copy, capture_output=True, text=True)
        message = next((s for s in result.stderr.splitlines() if 'LAB3 FAIL' in s), '')
        if not result.returncode or expected not in message:
            raise SystemExit('Undetected or unrelated failure: ' + name + '\n' + result.stdout + result.stderr)
        evidence.append(name + ' -> ' + message)
        print(evidence[-1])
        source.write_bytes(baseline)
evidence.append('LAB3 AUDIT PASS: 4/4 server defects detected in isolated copies')
output = root/'BuildFiles/Lab3/audit.log'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text('\n'.join(evidence) + '\n')
print(evidence[-1])
