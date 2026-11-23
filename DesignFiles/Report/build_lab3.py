"""Build and exercise the public event client in all four macOS configurations."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
from zipfile import ZipFile, ZIP_DEFLATED

root = Path(__file__).resolve().parents[2]
build = root / 'BuildFiles'
components = {
    'Eco.Lab1': 'D3B3E2F18D214A6B9A40440000000045',
    'Eco.Lab1QR': 'D3B3E2F18D214A6B9A40440000000046',
    'Eco.Lab2': 'D3B3E2F18D214A6B9A40440000000065',
    'Eco.CalculatorA': '4828F6552E4540E78121EBD220DC360E',
    'Eco.CalculatorB': 'AE202E543CE54550899603BD70C62565',
    'Eco.CalculatorD': '3A8E44677E82475CB4A3719ED8397E61',
    'Eco.CalculatorE': '872FEF1DE3314B87AD44D1E7C232C2F0',
}
env = dict(os.environ, CALCULATOR_VARIANT='A', BUILD_LAB3_CLIENTS='1')
env.setdefault('ECO_FRAMEWORK', str(Path.home() / 'Downloads/Eco.Framework'))
for config in ('Debug', 'Release'):
    env['CONFIGURATION'] = config
    out = build / 'Lab3' / config
    out.mkdir(parents=True, exist_ok=True)
    runtime = build / 'DynamicRuntime' / config / 'A'
    for command in ([sys.executable, 'DesignFiles/Report/build_component_clients.py'],
                    ['sh', 'DesignFiles/Report/build_dynamic_runtime_macos.sh']):
        subprocess.run(command, cwd=root, env=env, check=True)
    for mode in ('Static', 'Dynamic'):
        exe = out / ('EcoLab3' + mode)
        run_env = dict(env)
        if mode == 'Dynamic':
            run_env.update(ECO_FRAMEWORK_RT=str(runtime), DYLD_PRINT_LIBRARIES='1')
        result = subprocess.run([exe], env=run_env, capture_output=True, text=True)
        (out / (mode + '.log')).write_text(result.stdout)
        (out / (mode + '.stderr.log')).write_text(result.stderr)
        print(config, mode, flush=True)
        print(result.stdout, end='')
        if result.returncode:
            sys.exit(result.stderr or result.returncode)
        if mode == 'Dynamic':
            for cid in components.values():
                if str(runtime / (cid + '.dylib')) not in result.stderr:
                    sys.exit('Missing loader evidence for ' + cid)
            with tempfile.TemporaryDirectory() as empty:
                negative = subprocess.run([exe], env=dict(env, ECO_FRAMEWORK_RT=empty),
                                          capture_output=True, text=True)
                if not negative.returncode or 'QueryComponent' not in negative.stderr:
                    sys.exit('Unexpected outcome with an empty runtime: ' + negative.stderr)
            print('Empty runtime: QueryComponent failed as expected')

# macOS Archive Utility preserves executable bits from this zip.
package = build / 'Lab3' / 'EcoLab3-macOS-arm64.zip'
with ZipFile(package, 'w', ZIP_DEFLATED) as archive:
    for mode in ('Static', 'Dynamic'):
        archive.write(out / ('EcoLab3' + mode), 'EcoLab3' + mode)
        archive.write(out / (mode + '.log'), 'logs/' + mode + '.log')
    for name, cid in components.items():
        library = build / 'Components/Release' / name / ('lib' + cid + '.a')
        archive.write(library, 'Static/' + library.name)
        archive.write(runtime / (cid + '.dylib'), 'Runtime/' + cid + '.dylib')
    archive.writestr('Запуск.txt', 'Из папки распакованного архива на macOS arm64:\n'
                     './EcoLab3Static\nECO_FRAMEWORK_RT="$PWD/Runtime" ./EcoLab3Dynamic\n')
print(package)
