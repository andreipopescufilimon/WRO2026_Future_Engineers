#!/usr/bin/env python3
"""Audit WRO V4.1 release file presence and traceability. Does not claim real-world success."""
from pathlib import Path
import csv
ROOT=Path(__file__).resolve().parents[1]
def records(name):
 p=ROOT/'docs/validation/physical'/name
 if not p.is_file():return []
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
checks=records('functional_checks.csv');build=records('build_verification.csv');runs=records('challenge_runs.csv')
camera=(ROOT/'src/camera.py').read_text(encoding='utf-8')
code=all(x in camera for x in ('SURPRISE_RULE','HEIGHT_CONFIRM_FRAMES','get_cube_height','get_passing_color','CLOSE_BOTTOM_Y'))
valid_checks=[r for r in checks if r.get('date_utc') and len(r.get('firmware_commit',''))==40 and r.get('observed') and r.get('status','').upper()=='PASS']
valid_build=[r for r in build if r.get('date_utc') and len(r.get('firmware_commit',''))==40 and r.get('compile_outcome','').upper() in ('PASS','SUCCESS','0') and r.get('log_path')]
valid_runs=[r for r in runs if r.get('date_utc') and len(r.get('firmware_commit',''))==40 and r.get('outcome','').upper() in ('COMPLETED','PARTIAL','FAILED') and r.get('video_url')]
print('Source camera implementation present:',code)
print('GitHub workflow present:',(ROOT/'.github/workflows/verify-v41.yml').is_file())
print('Dated completed subsystem checks:',len(valid_checks),'of',len(checks))
print('Documented real successful builds:',len(valid_build))
print('Documented physical firmware-linked runs:',len(valid_runs))
print('Recorded PASS fields are evidence labels only when observation, date and revision are present.')
