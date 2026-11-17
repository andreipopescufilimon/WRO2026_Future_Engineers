#!/usr/bin/env python3
"""Export the actual World Final Arduino CI build outcome; never infer hardware success."""
from pathlib import Path
from datetime import datetime, timezone
import argparse, csv, re
parser = argparse.ArgumentParser()
parser.add_argument('--evidence', default='build-evidence')
args = parser.parse_args()
base = Path(args.evidence)
base.mkdir(parents=True, exist_ok=True)
def txt(name):
    f = base / name
    return f.read_text(encoding='utf-8', errors='replace').strip() if f.is_file() else ''
def libver(folder):
    p = Path('src/libraries') / folder / 'library.properties'
    if not p.is_file(): return ''
    m = re.search(r'^version=(.*)$', p.read_text(encoding='utf-8', errors='replace'), re.M)
    return m.group(1).strip() if m else ''
exit_code = txt('exit-code.txt')
status = 'PASS' if exit_code == '0' else 'FAIL' if exit_code else 'NOT_RUN'
values = {
    'test_id': 'CI-WORLD-FINAL-BUILD',
    'date_utc': txt('build-date-utc.txt') or datetime.now(timezone.utc).isoformat(),
    'hardware_revision': 'World Final (design target; CI runner is not physical hardware)',
    'firmware_commit': txt('source-commit.txt'),
    'nano_esp32_core_version': 'arduino:esp32@2.0.18-arduino.5' if txt('installed-cores.txt') else '',
    'arduino_cli_version': txt('arduino-cli-version.txt').replace('\n', ' '),
    'bmi088_version': libver('BMI088'),
    'esp32encoder_version': libver('ESP32Encoder'),
    'esp32servo_version': libver('ESP32Servo'),
    'sparkfun_vl53l5cx_version': libver('SparkFun_VL53L5CX_Arduino_Library'),
    'camera_firmware_version': 'OpenMV camera firmware not tested in CI',
    'compile_command': 'arduino-cli compile --fqbn arduino:esp32:nano_nora --libraries src/libraries --warnings all --verbose src/HLR_FE_World_Final',
    'compile_outcome': status,
    'log_path': 'build-evidence/compile.log',
    'notes': f'GitHub Actions compiler exit code: {exit_code or "not reached"}. No physical upload or operation claimed.'
}
with (base / 'build_verification.csv').open('w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=list(values), lineterminator='\n')
    writer.writeheader()
    writer.writerow(values)
print(f'World Final compile outcome: {status}; exit code: {exit_code or "n/a"}')
