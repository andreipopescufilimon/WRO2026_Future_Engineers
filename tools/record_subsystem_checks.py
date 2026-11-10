#!/usr/bin/env python3
"""Interactive real hardware verification recorder. No values or PASS results are invented."""
import argparse,csv,datetime,pathlib,subprocess,sys
ROOT=pathlib.Path(__file__).resolve().parent.parent
CSV=ROOT/'docs/validation/physical/functional_checks.csv'

def git_state():
    try:
        rev=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True,stderr=subprocess.DEVNULL).strip()
        dirty=subprocess.check_output(['git','status','--porcelain','--untracked-files=no'],cwd=ROOT,text=True,stderr=subprocess.DEVNULL)
        return rev,bool(dirty)
    except Exception:
        return '',False

def main():
    p=argparse.ArgumentParser(description='Record observed results from actual V4.1 hardware checks')
    p.add_argument('--test',help='ID of physical test to enter')
    p.add_argument('--list',action='store_true',help='Show available test IDs and current results')
    a=p.parse_args()
    with CSV.open(newline='',encoding='utf-8') as f:
        reader=csv.DictReader(f);header=reader.fieldnames;rows=list(reader)
    if a.list:
        for row in rows:print(f"{row['test_id']:20s} {row['status']:12s} {row['subsystem']}")
        if not a.test:return
    if not a.test:
        p.error('use --test TEST_ID or --list')
    row=next((r for r in rows if r['test_id']==a.test),None)
    if row is None:
        p.error('unknown test ID; use --list')
    print('TEST:',row['test_id'],row['subsystem'])
    print('EXPECTED:',row['expected'])
    print('PROCEDURE:',row['notes'])
    print('Perform the physical test first; do not enter expected outcomes as observations.')
    observed=input('Actual observed readings / direction / behavior (required): ').strip()
    if not observed:
        sys.exit('Nothing recorded; observation is required.')
    status=input('Result [PASS/FAIL] (required): ').strip().upper()
    if status not in {'PASS','FAIL'}:
        sys.exit('Only PASS or FAIL is accepted for a performed test.')
    revision=input('Installed hardware revision (required): ').strip()
    if not revision:
        sys.exit('Hardware revision is required.')
    evidence=input('Evidence URL or repository path (optional): ').strip()
    notes=input('Conditions / measured values / tolerances / other notes (optional): ').strip()
    rev,dirty=git_state()
    if not rev:
        rev=input('Exact flashed firmware Git commit (required): ').strip()
        if not rev:sys.exit('Exact firmware revision is required.')
    else:
        print('Git HEAD:',rev,'(modified files present)' if dirty else '')
        flashed=input('Is this exact revision on the robot? [yes/no]: ').strip().lower()
        if flashed!='yes':
            rev=input('Exact flashed firmware Git commit (required): ').strip()
            if not rev:sys.exit('Exact flashed firmware revision is required.')
    timestamp=datetime.datetime.now(datetime.timezone.utc).isoformat(timespec='seconds')
    row.update(date_utc=timestamp,firmware_commit=rev,observed=observed,status=status,evidence_url=evidence)
    row['notes']+=f' Actual hardware revision: {revision}. Conditions: {notes or "not recorded"}.'
    with CSV.open('w',newline='',encoding='utf-8') as f:
        writer=csv.DictWriter(f,fieldnames=header);writer.writeheader();writer.writerows(rows)
    print('Saved',row['test_id'],status,timestamp)

if __name__=='__main__':main()
