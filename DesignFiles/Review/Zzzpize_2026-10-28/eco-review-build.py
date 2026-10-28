from pathlib import Path
import subprocess,shlex
root=Path('/tmp/eco-zzz-review-20261028')
fw=Path('/Users/danial2006/Downloads/Eco.Framework')
flags=['clang','-std=c89','-fno-stack-protector','-DECO_APPLE','-DECO_AARCH64','-DECO_LIB','-DUGUID_UTILITY']
fw_inc=['-I'+str(p) for p in fw.glob('*/SharedFiles')]
ref_inc=['-I'+str(root/n/d) for n in ['Eco.Lab2','Eco.Lab1','Eco.CalculatorA','Eco.CalculatorB','Eco.CalculatorC','Eco.CalculatorD','Eco.CalculatorE'] for d in ['HeaderFiles','SharedFiles']]
libs=[str(fw/n/'BuildFiles/Mac/arm64/StaticRelease'/('lib'+cid+'.a')) for n,cid in [('Eco.System1','00000000000000000000000053595333'),('Eco.InterfaceBus1','00000000000000000000000042757331'),('Eco.MemoryManager1','0000000000000000000000004D656D31'),('Eco.FileSystemManagement1','00000000000000000000000046534D31')]]
for variant in ['A','C','ours']:
 if variant == 'ours':
  sources=[str(p) for p in Path('SourceFiles').glob('*.c')]+['/tmp/eco-compatibility-probe.c']
  inc=['-IHeaderFiles','-ISharedFiles']+fw_inc
 else:
  sources=[str(p) for n in ['Eco.Lab2','Eco.Lab1','Eco.Calculator'+variant,'Eco.CalculatorB','Eco.CalculatorD','Eco.CalculatorE'] for p in (root/n/'SourceFiles').glob('*.c')]+[str(root/'Eco.Lab2/UnitTestFiles/SourceFiles/EcoLab2.c')]
  inc=ref_inc+fw_inc
 cmd=flags+inc+sources+libs+['-o','/tmp/eco-review-'+variant]
 build=subprocess.run(cmd,capture_output=True,text=True)
 Path('/tmp/eco-review-build-'+variant+'.log').write_text(shlex.join(cmd)+'\n'+build.stdout+build.stderr)
 print(variant,'build exit',build.returncode)
 if build.returncode==0:
  run=subprocess.run(['/tmp/eco-review-'+variant],capture_output=True,text=True,timeout=30)
  Path('/tmp/eco-review-run-'+variant+'.log').write_text(run.stdout+run.stderr+'\nEXIT: '+str(run.returncode)+'\n')
  print('run exit',run.returncode, '\n',run.stdout[-1700:])
 else: print(build.stderr[-2000:])
