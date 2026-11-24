#!/usr/bin/env python3
"""Show only evidence-backed release readiness, without inventing test outcomes."""
from pathlib import Path
import csv,re
ROOT=Path(__file__).resolve().parents[1]
DIR=ROOT/'docs/validation/physical'
def rows(name):
 p=DIR/name
 with p.open(newline='',encoding='utf-8-sig') as f:return list(csv.DictReader(f))
sha=lambda s:bool(re.fullmatch('[0-9a-fA-F]{40}',s or ''))
cam=(ROOT/'src/camera.py').read_text(encoding='utf-8')
needed=['SURPRISE_RULE_ENABLED = True','def classify_sign_ratio(','def update_surprise_state(','CLOSE_BOTTOM_Y = 170']
print('Height-sensitive Surprise Rule exists in bundled camera:',all(x in cam for x in needed))
print('Nano ESP32 compilation workflow exists:',(ROOT/'.github/workflows/verify-v41.yml').is_file())
functional=rows('functional_checks.csv')
passed=[r for r in functional if r.get('date_utc') and sha(r.get('firmware_commit')) and r.get('observed') and r.get('evidence_url') and r.get('status','').upper()=='PASS']
print(f'Dated and evidenced physical subsystem passes: {len(passed)}/{len(functional)}')
builds=[r for r in rows('build_verification.csv') if r.get('date_utc') and sha(r.get('firmware_commit')) and r.get('compile_outcome','').upper() in ('0','PASS','SUCCESS') and r.get('log_path') and (ROOT/r['log_path']).is_file()]
print('Dated successful compiler outputs retained in archive:',len(builds))
runs=[r for r in rows('challenge_runs.csv') if r.get('date_utc') and sha(r.get('firmware_commit')) and r.get('video_url','').startswith(('https://','http://')) and r.get('outcome','') not in ('','NOT_RECORDED')]
print('V4.1 field runs tied to flashed source and video:',len(runs))
print('Historical video and simulated data are separate from physical release validation.')
