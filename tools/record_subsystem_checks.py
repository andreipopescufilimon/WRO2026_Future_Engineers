#!/usr/bin/env python3
"""Record actual manual subsystem observations; this script does not perform tests."""
import argparse, csv, subprocess
from datetime import datetime, timezone
from pathlib import Path

FILE=Path(__file__).resolve().parents[1]/'docs/validation/physical/functional_checks.csv'

def git_head():
    try: return subprocess.check_output(['git','rev-parse','HEAD'],cwd=FILE.parents[3],text=True,stderr=subprocess.DEVNULL).strip()
    except (OSError,subprocess.CalledProcessError): return ''

p=argparse.ArgumentParser(description='Record real V4.1 subsystem checks after observing your robot.')
p.add_argument('--list',action='store_true')
p.add_argument('--test',type=str)
a=p.parse_args()
if not FILE.exists(): raise SystemExit(f'Missing {FILE}')
with FILE.open(newline='',encoding='utf-8-sig') as f:
    r=csv.DictReader(f); cols=r.fieldnames; rows=list(r)
if a.list:
    for row in rows: print(f"{row['test_id']:20s} {row['subsystem']:29s} {row['status']}")
    raise SystemExit(0)
if not a.test: p.error('specify --list or --test <test ID>')
match=[x for x in rows if x['test_id']==a.test]
if not match: raise SystemExit('Unknown test ID. Use --list')
row=match[0]
print('Test:',row['test_id'],row['subsystem'])
print('Expected:',row['expected'])
print('Run this on the actual physical robot before answering.')
observed=input('Observed result, including real readings or behavior: ').strip()
if not observed: raise SystemExit('No observation supplied. Nothing saved.')
status=input('Outcome [PASS/FAIL/INCONCLUSIVE]: ').strip().upper()
if status not in ('PASS','FAIL','INCONCLUSIVE'): raise SystemExit('Invalid outcome. Nothing saved.')
rev=input('Actual flashed Git commit (enter if same as current git HEAD): ').strip() or git_head()
if not rev: raise SystemExit('Missing firmware commit. Nothing saved.')
hw=input('Hardware revision: ').strip() or 'V4.1'
evidence=input('Photo, video, serial log file or GitHub URL (optional): ').strip()
additional=input('Test conditions, tool, surface or notes: ').strip()
if input('Confirm these are YOUR REAL robot observations? Type YES: ').strip()!='YES':
    raise SystemExit('Cancelled. Nothing saved.')
row.update(date_utc=datetime.now(timezone.utc).isoformat(timespec='seconds').replace('+00:00','Z'),firmware_commit=rev,observed=observed,status=status,evidence_url=evidence,notes=f'Hardware {hw}. {additional}'.strip())
with FILE.open('w',newline='',encoding='utf-8') as f:
    w=csv.DictWriter(f,fieldnames=cols,lineterminator='\n');w.writeheader();w.writerows(rows)
print('Saved actual observation:',row['test_id'])
