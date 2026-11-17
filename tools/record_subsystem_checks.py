#!/usr/bin/env python3
"""Capture an *actual* manual robot check and write BOTH physical ledgers consistently.

This tool does not connect to the robot, and cannot turn a planned test into evidence.
"""
from __future__ import annotations
import argparse
import csv
import re
import subprocess
from datetime import datetime, timezone
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
DIR=ROOT/'docs/validation/physical'
FUNCTIONAL=DIR/'functional_checks.csv'
SUBSYSTEM=DIR/'subsystem_test_record.csv'

def read_csv(path):
    with path.open(newline='',encoding='utf-8-sig') as f:
        reader=csv.DictReader(f)
        return list(reader.fieldnames),list(reader)

def save_csv(path,columns,rows):
    tmp=path.with_suffix(path.suffix+'.tmp')
    with tmp.open('w',newline='',encoding='utf-8') as f:
        w=csv.DictWriter(f,fieldnames=columns,lineterminator='\n')
        w.writeheader();w.writerows(rows)
    tmp.replace(path)

def git_head():
    try:
        return subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,encoding='utf-8',stderr=subprocess.DEVNULL).strip()
    except (subprocess.CalledProcessError,OSError):return ''

p=argparse.ArgumentParser(description='Record real observed robot test outcomes; never fabricate evidence')
p.add_argument('--list',action='store_true',help='Display all planned checks and current status')
p.add_argument('--test',help='Test ID, e.g. V41-TOF-FRONT')
a=p.parse_args()
fc,fr=read_csv(FUNCTIONAL)
sc,sr=read_csv(SUBSYSTEM)
if a.list:
    for row in fr:print(f"{row['test_id']:22} {row['status']:23} {row['subsystem']}")
    raise SystemExit(0)
if not a.test:p.error('Use --list or --test <test ID>')
match=[x for x in fr if x['test_id']==a.test]
other=[x for x in sr if x['test_id']==a.test]
if len(match)!=1 or len(other)!=1:raise SystemExit('Test ID must exist exactly once in both physical ledgers')
r=match[0];r2=other[0]
print('Test:',r['test_id'])
print('Expected:',r['expected'])
print('Perform the check on the physical robot and record original measurements.')
observed=input('ACTUAL observations, with numbers where available: ').strip()
if len(observed)<8:raise SystemExit('Observed result missing or too short; no data saved')
result=input('Actual outcome [PASS/FAIL/INCONCLUSIVE]: ').strip().upper()
if result not in ('PASS','FAIL','INCONCLUSIVE'):raise SystemExit('Invalid outcome; no data saved')
revision=input('Full 40-character SHA of firmware ACTUALLY FLASHED: ').strip()
if not revision:
    candidate=git_head()
    if candidate and input(f'Use current Git HEAD {candidate}? Type YES only if this is flashed: ').strip()=='YES':revision=candidate
if not re.fullmatch(r'[0-9a-fA-F]{40}',revision):raise SystemExit('Full SHA required for traceability; no data saved')
hw=input('Hardware revision shown on assembled robot [V4.1]: ').strip() or 'V4.1'
evidence=input('Path or URL to ORIGINAL photo, video, instrument/serial log: ').strip()
if not evidence:raise SystemExit('An evidence path or URL is needed; no data saved')
if not evidence.startswith(('https://','http://')) and not (ROOT/evidence).is_file():
    raise SystemExit('Local primary evidence path does not exist; no data saved')
notes=input('Instrument, test conditions, surface, lighting or failures: ').strip()
if input('Confirm you personally observed this test and all entries are accurate. Type CONFIRM: ').strip()!='CONFIRM':
    raise SystemExit('Canceled without changing files')
when=datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')
r.update(date_utc=when,firmware_commit=revision,observed=observed,status=result,evidence_url=evidence,
         notes=f'Physical hardware: {hw}. Conditions: {notes}. Evidence recorded from actual test, not generated.')
r2.update(hardware_revision=hw,test_date_utc=when,actual_firmware_commit=revision,observed_behavior=observed,
          result=result,evidence_to_capture=evidence)
save_csv(FUNCTIONAL,fc,fr)
save_csv(SUBSYSTEM,sc,sr)
print('Recorded actual check in both ledgers. Run scripts/summarize_validation.py and tools/documentation_audit.py to refresh reports.')
