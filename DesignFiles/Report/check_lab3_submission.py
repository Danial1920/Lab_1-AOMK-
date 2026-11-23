"""Check the 2x2 rubric against runs, the submitted report and the unpacked build."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
from xml.etree import ElementTree as ET
from zipfile import ZipFile, ZIP_DEFLATED

root = Path(__file__).resolve().parents[2]
build = root / 'BuildFiles/Lab3'
expected = [(1,'Gauss',5,5), (1,'QR',14,14), (2,'Gauss',5,5), (2,'QR',0,14)]

def verify_output(output):
    rounds = [(int(r), source, int(c), int(j)) for r, source, c, j in
              re.findall(r'^round=(\d+) source=(\w+) console=(\d+) journal=(\d+)$', output, re.M)]
    passed = re.search(r'^LAB3 PASS: (\d+) checks$', output, re.M)
    if rounds != expected or not passed:
        raise SystemExit('Incomplete many-to-many demonstration:\n' + output)
    return passed.group(1)

rows = []
for config in ('Debug', 'Release'):
    rows.append([config] + [verify_output((build/config/(mode+'.log')).read_text())
                            for mode in ('Static', 'Dynamic')])
with ZipFile(root/'EcoLab3_Report.docx') as doc:
    xml = ET.fromstring(doc.read('word/document.xml'))
    ns = {'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
    tables = []
    for table in xml.findall('.//w:tbl',ns):
        tables.append([[''.join(t.text or '' for t in cell.findall('.//w:t',ns))
                        for cell in row.findall('w:tc',ns)] for row in table.findall('w:tr',ns)])
    if not any(table[1:] == rows for table in tables):
        raise SystemExit('Word report build table differs from actual runs')
    if not xml.findall('.//w:drawing',ns):
        raise SystemExit('Word report has no connection diagram')
for path in ['SourceFiles/Lab1Connections.c','SourceFiles/CEcoLab1Generated.c',
             'SharedFiles/IEcoLab1Events.h','UnitTestFiles/SourceFiles/Lab3Client.c']:
    if not (root/path).is_file():
        raise SystemExit('Missing source: '+path)

with tempfile.TemporaryDirectory(prefix='eco3-delivery-') as folder, ZipFile(build/'EcoLab3-macOS-arm64.zip') as z:
    unpacked = Path(folder)
    for info in z.infolist():
        dest = unpacked / info.filename
        if not dest.resolve().is_relative_to(unpacked.resolve()):
            raise SystemExit('Invalid archive path')
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(z.read(info))
        dest.chmod((info.external_attr >> 16) & 0o777)
    for mode in ('Static','Dynamic'):
        proc = subprocess.run([unpacked/('EcoLab3'+mode)], cwd=unpacked, capture_output=True,
                              env=dict(os.environ, ECO_FRAMEWORK_RT=str(unpacked/'Runtime')), text=True)
        if proc.returncode:
            raise SystemExit(proc.stderr or proc.returncode)
        if proc.stdout != (unpacked/'logs'/(mode+'.log')).read_text():
            raise SystemExit('Packaged '+mode+' client differs from its log')
        verify_output(proc.stdout)
print('LAB3 SUBMISSION PASS: two sources, two sinks, four callbacks, selective disconnect; '
      'four runs, Word table and diagram, sources, relocated Release clients')

# Keep the submission self-contained for both the immediate demo and a rebuild
# with the official SDK installed. Ship only sources and shared Xcode settings.
project_files = {root/name for name in ('Makefile','README.md','COMPLIANCE.md','EcoLab3_Report.docx')}
patterns = (
    'SourceFiles/*.c', 'HeaderFiles/*.h', 'SharedFiles/*', 'UnitTestFiles/*/*.c',
    'UnitTestFiles/*/*.h', 'DependenciesFiles/*/SharedFiles/*.h',
    'AssemblyFiles/Mac/*/*.xcodeproj/project.pbxproj',
    'AssemblyFiles/Mac/*/*.xcodeproj/xcshareddata/xcschemes/*.xcscheme',
    'DesignFiles/Report/*.sh', 'DesignFiles/Report/build_component_clients.py',
    'DesignFiles/Report/*lab3*.py', 'DesignFiles/Reference/*.py',
    'DesignFiles/Reference/*.json', 'DesignFiles/Reference/*.m',
)
for pattern in patterns:
    project_files.update(p for p in root.glob(pattern) if p.is_file() and not p.name.startswith('.'))
package = build/'EcoLab3-submission.zip'
with ZipFile(package, 'w', ZIP_DEFLATED) as delivery:
    for path in sorted(project_files):
        delivery.write(path, str(path.relative_to(root)))
    delivery.write(build/'EcoLab3-macOS-arm64.zip', 'EcoLab3-macOS-arm64.zip')
    delivery.write(build/'audit.log', 'Results/audit.log')
    for config in ('Debug','Release'):
        for mode in ('Static','Dynamic'):
            delivery.write(build/config/(mode+'.log'), 'Results/'+config+'/'+mode+'.log')
print(package)
