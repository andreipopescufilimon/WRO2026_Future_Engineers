#!/usr/bin/env python3
"""Run source-integrity/static documentation checks. Not an Arduino compilation or hardware test."""
import hashlib
import json
import re
from pathlib import Path
from datetime import date

root=Path(__file__).resolve().parents[1]
checks=[]
def check(name,ok,details): checks.append((name,bool(ok),details))

source=root/'src/HLR_FE_Separated'
tabs=['HLR_FE_Separated.ino','Open.ino','Final.ino','functions.ino','motors.ino','gyro.ino','tof.ino']
check('Seven V4.1 firmware tabs present',all((source/f).is_file() for f in tabs),', '.join(tabs))
fw='\n'.join((source/f).read_text(errors='replace') for f in tabs if (source/f).exists())
checks.append(('D11/D12 mode-selection definitions present', '#define MODE_FINAL_PIN D11' in fw and '#define MODE_OPEN_PIN D12' in fw, 'Static declarations, not physical switch verification'))
checks.append(('Four ToF I2C address definitions present',all('0x'+x in fw for x in ['30','31','32','33']),'Seven-bit addresses, static source check'))
checks.append(('Four-by-four, 30 Hz ToF configuration present', '#define TOF_FRAME_SIDE 4' in fw and '#define TOF_RANGING_HZ 30' in fw,'Device operation and achieved frame rate not tested'))
checks.append(('150 ms ToF freshness threshold present','#define TOF_FRESH_MS 150UL' in fw,'Firmware configuration only'))
checks.append(('Camera UART 19,200 baud present','Serial0.begin(19200)' in fw and 'UART(1, 19200' in (root/'src/camera.py').read_text(errors='replace'),'UART electrical link not tested'))
expected=['ESP32Encoder.h','ESP32Servo.h','SparkFun_VL53L5CX_Library.h','BMI088.h']
checks.append(('Firmware dependency headers declared',all(x in fw for x in expected),', '.join(expected)))
mds=['docs/03-power-sensors.md','docs/06-reproducibility.md']
checks.append(('Chapters 03 and 06 present',all((root/x).is_file() for x in mds),', '.join(mds)))
checks.append(('RT1062 terminology consistent',all('OpenMV N6' not in (root/x).read_text(errors='replace') for x in mds),'Both authored chapter bodies'))
# Check only project-local images and validation datasets; other chapters and external repo folders exist outside this patch.
missing=[]
for rel in mds:
    p=root/rel
    t=p.read_text(encoding='utf-8')
    links=re.findall(r'(?:src|href)="([^"]+)"|\]\(([^)]+)\)',t)
    for a,b in links:
        target=a or b
        if target.startswith(('http:', 'https:', '#', 'mailto:')):continue
        target=target.split('#')[0].split('?')[0]
        if target.startswith(('docs-media/', 'validation/')) and not (p.parent/target).exists():
            missing.append(f'{rel}: {target}')
checks.append(('All packaged media and validation links resolve',not missing,'; '.join(missing[:18]) if missing else 'Local docs-media and docs/validation references resolved'))
files={}
for base in ('src/HLR_FE_Separated','src/camera.py','docs/03-power-sensors.md','docs/06-reproducibility.md'):
    pp=root/base
    for p in ([pp] if pp.is_file() else pp.rglob('*')):
        if p.is_file(): files[str(p.relative_to(root)).replace('\\','/')]=hashlib.sha256(p.read_bytes()).hexdigest()
manifest={'manifest_type':'source-file integrity record (not a Git commit or hardware test)','date_recorded':str(date.today()),'repository':'https://github.com/andreipopescufilimon/WRO2026_Future_Engineers','tested_git_commit':None,'firmware_build_status':'not independently verified','physical_run_status':'not independently verified','files_sha256':files}
out=root/'docs/validation/evidence/source-sha256.json'
out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
rows=['# V4.1 source and documentation preflight','','This report records **static source and file checks only**. It does not prove Arduino compilation, camera execution, physical sensor precision or autonomous driving.','','| Check | Result | Scope |','|---|---|---|']
for n,ok,detail in checks:
    safe_detail=detail.replace("|", "\\|")
    rows.append(f'| {n} | {"PASS" if ok else "FAIL"} | {safe_detail} |')
rows+=['',f'**Summary:** {sum(ok for _,ok,_ in checks)}/{len(checks)} checks passed.','','[Source file checksums](source-sha256.json) allow another team to confirm that they are using the same exact sketch and camera script. A successful compilation log and physical tests remain separate evidence.','']
(root/'docs/validation/evidence/static-preflight-results.md').write_text('\n'.join(rows),encoding='utf-8')
for n,ok,desc in checks: print(('PASS' if ok else 'FAIL'),n,desc)
raise SystemExit(0 if all(ok for _,ok,_ in checks) else 1)
