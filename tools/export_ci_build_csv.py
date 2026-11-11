#!/usr/bin/env python3
"""Export the actual Arduino CI build outcome and software versions. No fabricated build results."""
from pathlib import Path
from datetime import datetime,timezone
import argparse,csv,os,re,sys
p=argparse.ArgumentParser();p.add_argument('--evidence',default='build-evidence');a=p.parse_args()
base=Path(a.evidence);base.mkdir(parents=True,exist_ok=True)
def txt(name):
    f=base/name
    return f.read_text(encoding='utf-8',errors='replace').strip() if f.exists() else ''
def libver(folder):
    p=Path('src/libraries')/folder/'library.properties'
    if not p.is_file():return ''
    t=p.read_text(encoding='utf-8',errors='replace')
    return re.search(r'^version=(.*)$',t,re.M).group(1).strip() if re.search(r'^version=(.*)$',t,re.M) else ''
ret=txt('exit-code.txt')
status='PASS' if ret=='0' else 'FAIL' if ret else 'NOT_RUN'
sha=txt('source-commit.txt')
values={
 'test_id':'CI-MODULAR-BUILD',
 'date_utc':txt('build-date-utc.txt') or datetime.now(timezone.utc).isoformat(),
 'hardware_revision':'V4.1 (design target; CI runner, not physical hardware)',
 'firmware_commit':sha,
 'nano_esp32_core_version':'arduino:esp32@2.0.18-arduino.5' if txt('installed-cores.txt') else '',
 'arduino_cli_version':txt('arduino-cli-version.txt').replace('\n',' '),
 'bmi088_version':libver('BMI088'),
 'esp32encoder_version':libver('ESP32Encoder'),
 'esp32servo_version':libver('ESP32Servo'),
 'sparkfun_vl53l5cx_version':libver('SparkFun_VL53L5CX_Arduino_Library'),
 'camera_firmware_version':'OpenMV RT1062 firmware not tested in CI',
 'compile_command':'arduino-cli compile --fqbn arduino:esp32:nano_nora --libraries src/libraries --warnings all --verbose src/HLR_FE_Separated',
 'compile_outcome':status,
 'log_path':'build-evidence/compile.log',
 'notes':f'Actual GitHub Actions runner result. Compiler exit code: {ret or "not reached"}. Source SHA in source-commit.txt. No physical upload or run claimed.'
}
with (base/'build_verification.csv').open('w',newline='',encoding='utf-8') as f:
 w=csv.DictWriter(f,fieldnames=list(values),lineterminator='\n');w.writeheader();w.writerow(values)
print(f'Compiled source={sha or "not captured"}; result={status}; code={ret or "n/a"}')
