#!/usr/bin/env python3
"""Build isolated component archives; link one public client object with A or C."""
import hashlib
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
FRAMEWORK = Path(os.environ.get('ECO_FRAMEWORK', str(Path.home() / 'Downloads/Eco.Framework')))
OUT = ROOT / 'BuildFiles' / 'Components'
CONFIG = os.environ.get('CONFIGURATION', 'Debug')
if CONFIG not in ('Debug', 'Release'):
    raise SystemExit('CONFIGURATION must be Debug or Release')
OUT = OUT / CONFIG
OUT.mkdir(parents=True, exist_ok=True)
FLAGS = ['-std=c89', '-Wall', '-Wextra', '-Werror', '-fno-stack-protector', '-fno-builtin',
         '-DECO_APPLE', '-DECO_AARCH64', '-DECO_LIB'] + (['-g', '-O0'] if CONFIG == 'Debug' else ['-O2'])
SDK = ['-I' + str(FRAMEWORK / n / 'SharedFiles') for n in (
    'Eco.Core1', 'Eco.System1', 'Eco.InterfaceBus1', 'Eco.MemoryManager1', 'Eco.FileSystemManagement1')]
SERVER_INC = ['-I' + str(ROOT / 'HeaderFiles'), '-I' + str(ROOT / 'SharedFiles')] + SDK
LIBS = [str(FRAMEWORK / n / 'BuildFiles/Mac/arm64/StaticRelease' / ('lib' + cid + '.a')) for n, cid in (
    ('Eco.System1', '00000000000000000000000053595333'),
    ('Eco.InterfaceBus1', '00000000000000000000000042757331'),
    ('Eco.MemoryManager1', '0000000000000000000000004D656D31'),
    ('Eco.FileSystemManagement1', '00000000000000000000000046534D31'))]
for path in LIBS:
    if not Path(path).is_file():
        raise SystemExit('Missing official Framework library: ' + path)

def run(args, **kwargs):
    subprocess.run(args, check=True, cwd=ROOT, **kwargs)

def archive(name, cid, sources, defines=()):
    directory = OUT / name
    directory.mkdir(exist_ok=True)
    objects = []
    for source in sources:
        obj = directory / (Path(source).stem + '.o')
        run(['clang'] + FLAGS + list(defines) + SERVER_INC + ['-c', str(ROOT / source), '-o', str(obj)])
        objects.append(str(obj))
    library = directory / ('lib' + cid + '.a')
    # Fresh archive avoids stale members when a source list is changed.
    if library.exists():
        library.unlink()
    run(['ar', 'rcs', str(library)] + objects)
    return str(library)

calculator_ids = ['4828F6552E4540E78121EBD220DC360E', 'AE202E543CE54550899603BD70C62565',
                  '4828F6552E4540E78121EBD220DC360E', '3A8E44677E82475CB4A3719ED8397E61',
                  '872FEF1DE3314B87AD44D1E7C232C2F0']
calculators = {name: archive('Eco.Calculator' + name, cid, ['SourceFiles/CalculatorComponents.c'],
                           ['-DECO_CALCULATOR_KIND=' + str(index)])
              for index, (name, cid) in enumerate(zip('ABCDE', calculator_ids))}
math_sources = ['SourceFiles/CEcoLab1Generated.c', 'SourceFiles/CEcoLab1GeneratedFactory.c']
normal = archive('Eco.Lab1', 'D3B3E2F18D214A6B9A40440000000045', math_sources, ['-DPOLYFIT_SEPARATE_FACTORY'])
qr = archive('Eco.Lab1QR', 'D3B3E2F18D214A6B9A40440000000046', math_sources,
             ['-DPOLYFIT_SEPARATE_FACTORY', '-DPOLYFIT_QR_ONLY'])
lab2 = archive('Eco.Lab2', 'D3B3E2F18D214A6B9A40440000000065', ['SourceFiles/EcoLab2.c'])
client = OUT / 'Lab2Client.o'
# No HeaderFiles or server C sources in this compilation command.
run(['clang'] + FLAGS + ['-I' + str(ROOT / 'UnitTestFiles/ExternalContracts'),
    '-I' + str(ROOT / 'SharedFiles')] + SDK + ['-c', str(ROOT / 'UnitTestFiles/SourceFiles/Lab2Client.c'), '-o', str(client)])
digest = hashlib.sha256(client.read_bytes()).hexdigest()
for variant in 'AC':
    executable = OUT / ('EcoLab2Client' + variant)
    run(['clang', str(client), lab2, normal, qr, calculators[variant], calculators['B'], calculators['D'], calculators['E']]
        + LIBS + ['-o', str(executable)])
    env = dict(os.environ, ECO_EXPECT_CALCULATOR=variant)
    result = subprocess.run([str(executable)], env=env, text=True, capture_output=True)
    (OUT / ('client-' + variant + '.log')).write_text(result.stdout + result.stderr)
    print(result.stdout, end='')
    if result.returncode:
        raise SystemExit(result.returncode)
    if hashlib.sha256(client.read_bytes()).hexdigest() != digest:
        raise SystemExit('Client object changed between A/C runs')
print('Same client object SHA256: ' + digest)
print('Independent archives and A/C substitution PASSED: ' + str(OUT))
