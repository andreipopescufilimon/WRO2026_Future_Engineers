#!/usr/bin/env python3
"""Aggregate ONLY physically logged V4.1 observations. No synthetic data is generated."""
import argparse
import csv
import math
import statistics
from pathlib import Path
from collections import defaultdict

FILES = {
  'power': 'power_runs.csv',
  'tof': 'tof_samples.csv',
  'imu': 'imu_samples.csv',
  'camera': 'camera_trials.csv',
  'field': 'challenge_runs.csv',
  'iterations': 'revision_comparisons.csv',
  'functional': 'functional_checks.csv',
}

def readrows(p):
    if not p.exists(): return []
    with p.open(newline='', encoding='utf-8-sig') as f:
        rows = [r for r in csv.DictReader(f) if any(str(v or '').strip() for v in r.values())]
        # Only rows containing actual observations enter the physical summary.
        rows = [r for r in rows if not str(r.get('notes', '')).startswith('TEST_PLAN')]
        if p.name == 'functional_checks.csv':
            rows = [r for r in rows if r.get('date_utc') and r.get('observed') and
                    str(r.get('status', '')).strip().upper() in ('PASS', 'FAIL')]
        return rows

def number(v):
    try: return float(v)
    except (ValueError, TypeError): return None

def fmt(x,unit='',digits=2):
    return f'{x:.{digits}f}{unit}' if x is not None else 'not available'

def summarize(base):
    d={name:readrows(base/filename) for name,filename in FILES.items()}
    lines=['# V4.1 physical validation summary','','Generated from recorded physical observations only. Blank input files produce no results. This summary is not an automated claim that any test passed.','']
    lines+=['## Electrical load and 5 V stability','','| Test / state | Samples | Mean pack current | Peak pack current | Minimum 5 V rail | Recorded pack energy |','|---|---:|---:|---:|---:|---:|']
    by=defaultdict(list)
    for row in d['power']:
        by[(row.get('test_id',''),row.get('state',''))].append(row)
    if not by: lines+=['| No physical power log provided | 0 | not available | not available | not available | not available |']
    for (tid,state),rs in sorted(by.items()):
        amps=[number(r.get('battery_a')) for r in rs]; amps=[v for v in amps if v is not None]
        rails=[number(r.get('rail5_v')) for r in rs]; rails=[v for v in rails if v is not None]
        pairs=sorted([(number(r.get('t_s')),number(r.get('battery_v')),number(r.get('battery_a'))) for r in rs if all(number(r.get(k)) is not None for k in ('t_s','battery_v','battery_a'))])
        wh=sum(max(0,b[0]-a[0])*(a[1]*a[2]+b[1]*b[2])/2/3600 for a,b in zip(pairs,pairs[1:])) if len(pairs)>=2 else None
        lines+=[f'| {tid} / {state} | {len(rs)} | {fmt(statistics.mean(amps) if amps else None," A",3)} | {fmt(max(amps) if amps else None," A",3)} | {fmt(min(rails) if rails else None," V",3)} | {fmt(wh," Wh",4)} |']
    lines+=['','## Distance sensor accuracy','','Valid ToF samples are evaluated individually against ruler distance. Invalid samples are counted separately; zone frame-rate estimates require timestamps.','','| Sensor | Samples | Valid | Invalid | Mean bias | RMSE | Frame rate |','|---|---:|---:|---:|---:|---:|---:|']
    by=defaultdict(list)
    for row in d['tof']: by[row.get('sensor','unspecified')].append(row)
    if not by: lines+=['| No physical ToF log provided | 0 | 0 | 0 | not available | not available | not available |']
    for sensor,rs in sorted(by.items()):
        pairs=[(number(r.get('reference_mm')),number(r.get('measured_mm'))) for r in rs if str(r.get('valid','')).lower() in ('1','true','yes')]
        errors=[m-ref for ref,m in pairs if m is not None and ref is not None]
        valid=len(errors); invalid=len(rs)-valid
        # Compute frame rate inside each uninterrupted test/reference run,
        # not across gaps while moving targets or resetting the logger.
        by_trial=defaultdict(list)
        for r in rs:
            t=number(r.get('frame_timestamp_ms'))
            if t is not None:
                by_trial[(r.get('test_id',''),r.get('reference_mm',''))].append(t)
        time_ms=0.0; frames=0
        for group_times in by_trial.values():
            tt=sorted(set(group_times))
            if len(tt)>1 and tt[-1]>tt[0]:
                frames+=len(tt)-1;time_ms+=tt[-1]-tt[0]
        rate=1000*frames/time_ms if time_ms>0 else None
        bias=statistics.mean(errors) if errors else None
        rmse=math.sqrt(statistics.mean([e*e for e in errors])) if errors else None
        lines+=[f'| {sensor} | {len(rs)} | {valid} | {invalid} | {fmt(bias," mm")} | {fmt(rmse," mm")} | {fmt(rate," Hz",1)} |']
    lines+=['','## BMI088 drift and heading accuracy','','| Test / condition | Samples | Mean gyro Z rate | Heading drift | Mean absolute turn error |','|---|---:|---:|---:|---:|']
    by=defaultdict(list)
    for r in d['imu']: by[(r.get('test_id',''),r.get('condition',''))].append(r)
    if not by: lines+=['| No physical IMU log provided | 0 | not available | not available | not available |']
    for (tid,cond),rs in sorted(by.items()):
        zs=[number(r.get('gyro_z_dps')) for r in rs]; zs=[x for x in zs if x is not None]
        pt=sorted((number(r.get('t_s')),number(r.get('integrated_heading_deg'))) for r in rs if number(r.get('t_s')) is not None and number(r.get('integrated_heading_deg')) is not None)
        drift=60*(pt[-1][1]-pt[0][1])/(pt[-1][0]-pt[0][0]) if cond.lower()=='stationary' and len(pt)>1 and pt[-1][0]>pt[0][0] else None
        es=[abs(number(r.get('integrated_heading_deg'))-number(r.get('reference_heading_deg'))) for r in rs if number(r.get('integrated_heading_deg')) is not None and number(r.get('reference_heading_deg')) is not None and cond.lower()!='stationary']
        lines+=[f'| {tid} / {cond} | {len(rs)} | {fmt(statistics.mean(zs) if zs else None," °/s",3)} | {fmt(drift," °/min",2)} | {fmt(statistics.mean(es) if es else None,"°")} |']
    lines+=['','## OpenMV RT1062 classification','','A trial is counted as detected when a red/green classification is reported, and correct only when it matches the reference color.','','| Lighting / distance | Trials | Correct | Missed | Wrong colour | Accuracy | Mean processing latency |','|---|---:|---:|---:|---:|---:|---:|']
    by=defaultdict(list)
    for r in d['camera']: by[(r.get('lighting',''),r.get('distance_mm',''))].append(r)
    if not by: lines+=['| No recorded camera trials | 0 | 0 | 0 | 0 | not available | not available |']
    for (lighting,dist),rs in sorted(by.items()):
        norm=lambda x: (x or '').strip().lower()
        no_detect={'','none','missing','no_detection'}
        true_empty=lambda r: norm(r.get('reference_color')) in no_detect
        detect_empty=lambda r: norm(r.get('detected_color')) in no_detect
        correct=sum((true_empty(r) and detect_empty(r)) or (not true_empty(r) and norm(r.get('detected_color'))==norm(r.get('reference_color'))) for r in rs)
        missed=sum(not true_empty(r) and detect_empty(r) for r in rs)
        wrong=len(rs)-correct-missed # includes false positives for no-pillar trials
        ls=[number(r.get('processing_ms')) for r in rs];ls=[x for x in ls if x is not None]
        lines+=[f'| {lighting} / {dist} mm | {len(rs)} | {correct} | {missed} | {wrong} | {100*correct/len(rs):.1f}% | {fmt(statistics.mean(ls) if ls else None," ms")} |']
    lines+=['','## Subsystem functional test outcomes','','| Test | Subsystem | Result | Observation | Evidence |','|---|---|---|---|---|']
    if not d['functional']: lines+=['| No dated physical subsystem test records provided | not available | not available | not available | not available |']
    for r in d['functional']:
        clean=lambda x:(x or '').replace('|','/').replace('\n',' ')
        lines+=[f"| {clean(r.get('test_id'))} | {clean(r.get('subsystem'))} | {clean(r.get('status'))} | {clean(r.get('observed'))} | {clean(r.get('evidence_url'))} |"]
    lines+=['','## Challenge runs and revision comparisons','','| Mode | Runs logged | Documented successful runs | Evidence links recorded |','|---|---:|---:|---:|']
    by=defaultdict(list)
    for r in d['field']: by[r.get('challenge','unspecified')].append(r)
    if not by: lines+=['| No dated V4.1 run records provided | 0 | 0 | 0 |']
    for mode,rs in sorted(by.items()):
        succ=sum(str(r.get('outcome','')).strip().lower() in ('pass','completed','success') for r in rs)
        vids=sum(bool(str(r.get('video_url','')).strip()) for r in rs)
        lines+=[f'| {mode} | {len(rs)} | {succ} | {vids} |']
    lines+=['','## V4 → V4.1 engineering comparison','','| Subsystem | Test method / metric | V4 result | V4.1 result | Assessment |','|---|---|---|---|---|']
    by=defaultdict(list)
    for r in d['iterations']: by[(r.get('subsystem',''),r.get('metric',''),r.get('test_condition',''))].append(r)
    if not by: lines+=['| No paired revision measurements provided | not available | not available | not available | Cannot quantify improvement |']
    for (ss,met,cond),rs in sorted(by.items()):
        old=[number(r.get('value')) for r in rs if r.get('revision','').upper()=='V4' and number(r.get('value')) is not None]
        new=[number(r.get('value')) for r in rs if r.get('revision','').upper()=='V4.1' and number(r.get('value')) is not None]
        a=statistics.mean(old) if old else None;b=statistics.mean(new) if new else None
        assessment=fmt(b-a,'') if a is not None and b is not None else 'Not paired'
        lines+=[f'| {ss} | {met}, {cond} | {fmt(a)} | {fmt(b)} | {assessment} (V4.1−V4) |']
    lines+=['','Every reported result must retain its source CSV, firmware revision, hardware revision, date, conditions and evidence link. This script does not infer test success from the presence of a photograph, sketch or video.','']
    return '\n'.join(lines),d

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--input',default='docs/validation/physical')
    p.add_argument('--output',default='docs/validation/physical/SUMMARY.md')
    args=p.parse_args()
    md, datasets=summarize(Path(args.input))
    Path(args.output).parent.mkdir(parents=True, exist_ok=True)
    Path(args.output).write_text(md, encoding='utf-8')
    print(f'Wrote {args.output}; physically recorded rows: '+', '.join(f'{k}={len(v)}' for k,v in datasets.items()))
