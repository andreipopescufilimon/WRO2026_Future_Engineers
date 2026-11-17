#!/usr/bin/env python3
"""Source-grounded WRO documentation audit. Does not manufacture hardware evidence."""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
import re
import subprocess
import sys
from datetime import date
from pathlib import Path
from urllib.parse import unquote

ROOT=Path(__file__).resolve().parents[1]
DOCS=ROOT/'docs'
PHYSICAL=DOCS/'validation/physical'
REPORT=DOCS/'validation/EVIDENCE_AUDIT.md'
SOURCE=ROOT/'src/camera.py'
HEADERS=['id','category','outcome','details','reference']


def table(path):
    if not path.exists():return []
    with path.open(newline='',encoding='utf-8-sig') as f:return list(csv.DictReader(f))


def sha256(path):return hashlib.sha256(path.read_bytes()).hexdigest()


def source_has(string):return string in SOURCE.read_text(encoding='utf-8')


def git_full_sha(s):return bool(re.fullmatch(r'[0-9a-fA-F]{40}',s or ''))


def github_head():
    if not (ROOT/'.git').exists():return None
    p=subprocess.run(['git','rev-parse','HEAD'],cwd=ROOT,text=True,capture_output=True)
    return p.stdout.strip() if p.returncode==0 else None


def audit_links():
    bad=[];total=0
    # Check locally referenced targets in authored Markdown, excluding code and patterns with dynamic placeholders.
    for path in [ROOT/'README.md',*sorted(DOCS.glob('*.md')), *sorted((DOCS/'validation').glob('*.md')),
                 ROOT/'3D-models/README.md',ROOT/'electrical-schematics/README.md',ROOT/'video/video.md']:
        if not path.is_file():continue
        txt=path.read_text(encoding='utf-8')
        txt=re.sub(r'(?ms)^\s*```.*?^\s*```','',txt)
        urls=re.findall(r'(?<!!)\[[^]\n]+\]\(([^)]+)\)',txt)
        urls+=re.findall(r'!\[[^]\n]*\]\(([^)]+)\)',txt)
        urls+=re.findall(r'(?:src|href)="([^"]+)"',txt)
        for raw in urls:
            # Markdown links can have title attributes, but the project mainly uses plain URLs.
            target=raw.split(' "')[0].strip().strip('<>')
            if not target or target.startswith(('http://','https://','mailto:','data:','#','javascript:')):continue
            if '{' in target or '}' in target or '$' in target:continue
            target=unquote(target.split('#')[0].split('?')[0]);total+=1
            if target.startswith('/'):
                p=ROOT/target.lstrip('/')
            else:p=path.parent/target
            if not p.exists():bad.append((str(path.relative_to(ROOT)),raw))
    return total,bad


def audit():
    checks=[]
    def add(name,state,detail,ref):checks.append((name,state,detail,ref))
    docs=[f'0{i}-{s}.md' for i,s in ((1,'overview'),(2,'mobility'),(3,'power-sensors'),(4,'software'),(5,'systems'),(6,'reproducibility'))]
    add('Six engineering journal chapters','PASS' if all((DOCS/x).is_file() for x in docs) else 'FAIL','Six WRO chapters are present','docs/')
    r=(ROOT/'README.md').read_text(encoding='utf-8')
    add('Minimum README length','PASS' if len(r)>=5000 else 'FAIL',f'{len(r):,} Unicode characters, minimum 5,000','README.md')
    add('Mechanical CAD and PCB packages','PASS' if len(list((ROOT/'3D-models').glob('*.step')))>=8 and len(list((ROOT/'electrical-schematics').rglob('Gerber_*.zip')))>=3 else 'FAIL',f'{len(list((ROOT/"3D-models").glob("*.step")))} STEP files, {len(list((ROOT/"electrical-schematics").rglob("Gerber_*.zip")))} Gerber ZIPs','3D-models/; electrical-schematics/')
    add('World Final firmware tabs','PASS' if len(list((ROOT/'src/HLR_FE_World_Final').glob('*.ino')))>=8 else 'FAIL',f'{len(list((ROOT/"src/HLR_FE_World_Final").glob("*.ino")))} tabs present','src/HLR_FE_World_Final/')
    cam=SOURCE.read_text(encoding='utf-8')
    add('Area-based current camera source','PASS' if 'follow_threshold = 5000' in cam and 'area < follow_threshold' in cam else 'FAIL','Current area threshold and current branching are present','src/camera.py')
    add('Height-based Surprise Rule in uploaded camera','PASS' if all(x in cam for x in ['SURPRISE_RULE','def get_cube_height(','def get_passing_color(','CLOSE_BOTTOM_Y']) else 'MISSING','The 50/100/150 mm rule and bottom-position close condition are not in the uploaded camera program','src/camera.py')
    candidate=ROOT/'src/camera_candidates/rt1062_surprise_rule_v11.py'
    candidate_tests=table(DOCS/'validation/v11_camera_candidate_unit_tests.csv')
    candidate_passing=bool(candidate_tests) and all(row.get('result')=='PASS' for row in candidate_tests)
    add('Recovered v11 candidate camera pure logic', 'PASS' if candidate.is_file() and candidate_passing else 'PENDING', f'{sum(x.get("result")=="PASS" for x in candidate_tests)}/{len(candidate_tests)} candidate pure-function checks pass; no camera hardware or 2026 field run certified', 'src/camera_candidates/rt1062_surprise_rule_v11.py')
    add('WRO 2026 ToF active configuration','PASS' if all(x in (ROOT/'src/HLR_FE_World_Final/tof.ino').read_text() for x in ['#define TOF_FRAME_SIDE 4','#define TOF_RANGING_HZ 30']) else 'FAIL','Configured 4×4 zones and nominal 30 Hz, not 8×8/60 Hz','src/HLR_FE_World_Final/tof.ino')
    add('Source-hash availability','PASS',f'SHA-256 for camera: {sha256(SOURCE)}','src/camera.py')
    total,bad=audit_links()
    add('Local markdown links','PASS' if not bad else 'ISSUES',f'{total} local links checked; {len(bad)} unresolved','docs/')
    if bad:
        for path,target in bad[:45]:add('Broken link','FAIL',f'{path}: {target}',path)
    checks_rows=table(PHYSICAL/'functional_checks.csv')
    valid_checks=[x for x in checks_rows if x.get('date_utc') and git_full_sha(x.get('firmware_commit',''))
                  and x.get('observed') and x.get('status','').upper() in ('PASS','FAIL') and x.get('evidence_url')]
    add('Documented physical subsystem observations','PASS' if len(valid_checks)>=12 else 'PENDING',f'{len(valid_checks)}/{len(checks_rows)} checks have date, 40-character SHA, measured observation, pass/fail status and evidence','docs/validation/physical/functional_checks.csv')
    builds=table(PHYSICAL/'build_verification.csv')
    valid_build=[]
    for x in builds:
        ok=x.get('date_utc') and git_full_sha(x.get('firmware_commit','')) and x.get('compile_outcome','').upper() in ('PASS','SUCCESS','0') and x.get('log_path')
        if ok:
            loc=(ROOT/x['log_path']).resolve()
            if loc.is_file() and loc.is_relative_to(ROOT):valid_build.append(x)
    add('Successful build record + retained output','PASS' if valid_build else 'PENDING',f'{len(valid_build)} compiler success records with date, full source SHA and existing log file','docs/validation/physical/build_verification.csv')
    runs=table(PHYSICAL/'challenge_runs.csv')
    valid_runs=[x for x in runs if x.get('date_utc') and git_full_sha(x.get('firmware_commit',''))
               and x.get('outcome','').strip().upper() not in ('','NOT_RECORDED','TEST_PLAN') and x.get('video_url','').startswith(('https://','http://'))]
    add('Firmware-linked full-field runs','PASS' if len(valid_runs)>=2 else 'PENDING',f'{len(valid_runs)} runs with date, flashed SHA, actual result and video link','docs/validation/physical/challenge_runs.csv')
    def valid_measurements(name,fields):
        entries=table(PHYSICAL/name)
        return [x for x in entries if x.get('date_utc') and git_full_sha(x.get('firmware_commit','')) and all(x.get(k,'').strip() for k in fields) and not x.get('test_id','').startswith(('PLAN-','SIM_'))]
    measurement_sets={
        'power_runs.csv':['battery_v','battery_a','rail5_v'],
        'tof_samples.csv':['reference_mm','measured_mm'],
        'imu_samples.csv':['reference_yaw_deg','integrated_yaw_deg'],
        'camera_trials.csv':['reference_color','detected_color'],
    }
    for name,req in measurement_sets.items():
        entries=valid_measurements(name,req)
        add('Measured '+name,'PASS' if entries else 'PENDING',f'{len(entries)} dated physical measurements with source SHA and required numeric fields',f'docs/validation/physical/{name}')
    same=(ROOT/'docs/validation/physical/subsystem_test_record.csv').is_file()
    add('Companion subsystem ledger','PASS' if same else 'FAIL','Physical ledger present; check ID reconciliation requires real test outcomes','docs/validation/physical/subsystem_test_record.csv')
    add('Historical Git commit verification','PENDING' if not github_head() else 'SOURCE_ONLY',
        'No .git directory in the supplied ZIP' if not github_head() else f'Working-tree Git HEAD {github_head()} is not evidence of competition milestone dates','GitHub history')
    return checks


def create_report(checks):
    esc=lambda s:str(s).replace('|','\\|').replace('\n',' ')
    lines=['# V4.1 documentation evidence audit','','**Evidence scope:** this is an automated inspection of the supplied files and CSV records, not a competition judge score or physical-robot test. It does not turn calculations, pre-filled templates, simulated rows or source-code checks into field validation.','','| Check | Result | What was inspected | Evidence path |','|---|---|---|---|']
    for name,state,detail,ref in checks:lines.append(f'| {esc(name)} | **{state}** | {esc(detail)} | `{esc(ref)}` |')
    p=sum(state=='PASS' for _,state,*_ in checks)
    outstanding=sum(state in ('FAIL','PENDING','ISSUES','MISSING') for _,state,*_ in checks)
    lines.extend(['',f'**Summary:** {p} checks pass; {outstanding} findings need attention. This is **not** a score out of 30.','','## Interpreting this report','','- PASS means the stated **file or reproducible source-level condition** was observed. A file-presence PASS cannot substitute for a motor test.','- PENDING means a real compiler, hardware, camera or field observation is not available with traceable evidence.','- MISSING means an advertised source feature was absent. Do not claim that feature was tested.','- Source-only checks do not establish that code can compile for Nano ESP32 or run on OpenMV hardware.','- Retain actual instrument logs, failed trials, and the exact flashed SHA before making an engineering reliability claim.','','See [judging evidence map](../07-judging-evidence.md), [implementation gaps](IMPLEMENTATION_GAPS.md), and [physical verification directory](physical/README.md).',''])
    REPORT.write_text('\n'.join(lines),encoding='utf-8')


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--strict',action='store_true',help='Fail if ANY evidence or source mismatch remains')
    p.add_argument('--json',action='store_true')
    a=p.parse_args()
    c=audit();create_report(c)
    if a.json:print(json.dumps([dict(zip(HEADERS[:4],[name,'',state,detail])) for name,state,detail,ref in c],indent=2))
    else:
        for name,state,detail,ref in c:print(f'{state:10s} {name:45s} {detail[:110]}')
        print(f'\nReport: {REPORT.relative_to(ROOT)}')
    if a.strict and any(st!='PASS' for _,st,_,_ in c):sys.exit(1)
    if any(st=='FAIL' for _,st,_,_ in c) and not a.strict:sys.exit(2)

if __name__=='__main__':main()
