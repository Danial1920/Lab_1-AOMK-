#!/usr/bin/env python3
"""Mutation checks performed only on an isolated temporary copy of the project."""
import os
import shutil
import subprocess
import tempfile
from pathlib import Path
from reference_cases import ROOT


def main():
    framework = os.environ.get('ECO_FRAMEWORK', str(Path.home()/'Downloads/Eco.Framework'))
    with tempfile.TemporaryDirectory(prefix='eco_build_validation_') as temporary:
        temporary = Path(temporary)
        for directory in ('SourceFiles','HeaderFiles','SharedFiles','UnitTestFiles','DesignFiles/Report'):
            shutil.copytree(ROOT/directory, temporary/directory)
        (temporary/'DependenciesFiles').symlink_to(ROOT/'DependenciesFiles', target_is_directory=True)
        shutil.copy2(ROOT/'Makefile', temporary/'Makefile')
        def run(*args, success=True):
            result = subprocess.run(args,cwd=temporary,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
            if (result.returncode == 0) != success:
                raise AssertionError('Unexpected exit %d: %s\n%s' % (result.returncode,args,result.stdout))
            return result.stdout
        run('make','ECO_FRAMEWORK='+framework,'all','eco-build')
        host = temporary/'BuildFiles/Host/EcoLab1GeneratedUnitTest'
        eco = temporary/'BuildFiles/Host/EcoLab1EcoTest'
        before = host.stat().st_mtime_ns
        header = temporary/'HeaderFiles/polyfit_core.h'
        header.write_bytes(header.read_bytes()+b'\n/* temporary mutation */\n')
        run('make','all')
        assert host.stat().st_mtime_ns > before, 'Host did not rebuild after HeaderFiles change'
        before = eco.stat().st_mtime_ns
        header = temporary/'SharedFiles/IEcoPolynomialMath.h'
        header.write_bytes(header.read_bytes()+b'\n/* temporary mutation */\n')
        run('make','ECO_FRAMEWORK='+framework,'eco-build')
        assert eco.stat().st_mtime_ns > before, 'Eco did not rebuild after SharedFiles change'
        before = host.stat().st_mtime_ns
        run('make','CFLAGS=-std=c89 -DECO_APPLE -DVALIDATION_CONFIG_CHANGED','all')
        assert host.stat().st_mtime_ns > before, 'Host did not rebuild after CFLAGS change'
        run('make','ECO_FRAMEWORK=/nonexistent-eco-framework','eco-build', success=False)
        print('BUILD DEPENDENCY MUTATIONS PASSED: headers, SharedFiles, CFLAGS, invalid Framework')
        source = temporary/'SourceFiles/polyfit_bus.c'
        original_source = source.read_bytes()
        run('sh', 'DesignFiles/Report/check_no_libc.sh')
        # An unused standard header adds no undefined symbol: only the source
        # scanner can catch this mutation. Keep that check independent of nm.
        source.write_bytes(original_source + b'\n#include <stdlib.h>\n')
        output = run('sh', 'DesignFiles/Report/check_no_libc.sh', success=False)
        assert 'forbidden source dependency' in output and 'stdlib.h' in output, output
        print('NO-LIBC SOURCE MUTATION PASSED: unused forbidden header detected')
        source.write_bytes(original_source+b'\nextern void abort(void);\nvoid validation_libc_probe(void) { abort(); }\n')
        output = run('sh','DesignFiles/Report/check_no_libc.sh', success=False)
        assert 'unexpected external symbols' in output and '_abort' in output, output
        print('NO-LIBC NEGATIVE MUTATION PASSED: abort() detected through nm')
        source = temporary/'SourceFiles/CalculatorComponents.c'
        text = source.read_text(encoding='utf-8-sig')
        assert 'return (int32_t)a + (int32_t)b;' in text
        source.write_text(text.replace('return (int32_t)a + (int32_t)b;','return (int32_t)a + (int32_t)b + 1;',1), encoding='utf-8-sig')
        output = run('make','ECO_FRAMEWORK='+framework,'eco-test', success=False)
        assert 'FAIL ' in output, output
        print('UNITTEST NEGATIVE MUTATION PASSED: incorrect real operation produces nonzero exit')
        # Mutate only A, then only D. The public Horner client must observe
        # both changes through real included interfaces, not local arithmetic.
        mutations = (
            ('A.Addition', 'return (int32_t)a + (int32_t)b;',
             'return (int32_t)a + (int32_t)b + (component->kind == CALCULATOR_A ? 1L : 0L);'),
            ('D.Multiplication', 'return (int32_t)a * (int32_t)b;',
             'return (int32_t)a * (int32_t)b + (((CCalculatorYView*)me)->owner->kind == CALCULATOR_D ? 1L : 0L);'),
        )
        for label, original, replacement in mutations:
            assert original in text
            source.write_text(text.replace(original, replacement, 1), encoding='utf-8-sig')
            output = run('make', 'ECO_FRAMEWORK='+framework, 'component-clients', success=False)
            assert 'FAIL: Horner via included A/C and D' in output, output
            print('HORNER DEPENDENCY MUTATION PASSED: ' + label + ' changes the public result')

if __name__ == '__main__': main()
