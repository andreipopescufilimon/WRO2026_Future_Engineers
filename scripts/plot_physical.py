#!/usr/bin/env python3
"""Produce graphs ONLY from recorded physical observations. No demo values are inserted."""
import csv
from pathlib import Path
from collections import defaultdict
import argparse

def records(path):
    if not path.is_file():return []
    with path.open(newline='',encoding='utf-8-sig') as f:return [r for r in csv.DictReader(f) if any(str(v or '').strip() for v in r.values())]

def num(s):
    try:return float(s)
    except (TypeError,ValueError):return None

def main():
    p=argparse.ArgumentParser()
    p.add_argument('--input',default='docs/validation/physical')
    p.add_argument('--output',default='docs/validation/physical/figures')
    args=p.parse_args()
    base=Path(args.input); out=Path(args.output)
    try:
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
    except ImportError:
        print('matplotlib is required for optional figures: python -m pip install matplotlib')
        return
    created=0
    power=records(base/'power_runs.csv')
    by=defaultdict(list)
    for r in power:
        t=num(r.get('t_s'));i=num(r.get('battery_a'));v=num(r.get('rail5_v'))
        if t is not None and (i is not None or v is not None):
            by[r.get('test_id','unlabeled')].append((t,i,v))
    for name,rows in by.items():
        rows.sort()
        for idx,label,units,filename in ((1,'Battery input current','A','power-current'),(2,'Regulated 5 V rail','V','power-rail5')):
            xy=[(r[0],r[idx]) for r in rows if r[idx] is not None]
            if len(xy)<2:continue
            fig,ax=plt.subplots(figsize=(9,5))
            ax.plot([x for x,y in xy],[y for x,y in xy],marker='.',markersize=3)
            ax.set(xlabel='Time (s)',ylabel=f'{label} ({units})',title=f'V4.1: {label} | {name}')
            ax.grid(alpha=.2);fig.tight_layout()
            out.mkdir(parents=True,exist_ok=True);fig.savefig(out/f'{filename}-{name}.png',dpi=180)
            plt.close(fig);created+=1
    tof=records(base/'tof_samples.csv')
    by=defaultdict(list)
    for r in tof:
        target=num(r.get('reference_mm'));m=num(r.get('measured_mm'))
        if target is None or m is None or str(r.get('valid','')).lower() not in ('1','true','yes'):continue
        by[r.get('sensor','unknown')].append((target,m-target))
    if by:
        fig,ax=plt.subplots(figsize=(9,5))
        for name,xy in sorted(by.items()):ax.scatter([x for x,y in xy],[y for x,y in xy],label=name,alpha=.4,s=12)
        ax.axhline(0,ls='--',lw=1)
        ax.set(xlabel='Reference distance (mm)',ylabel='Measured minus reference (mm)',title='V4.1 ToF individual-frame range errors')
        ax.legend();ax.grid(alpha=.2);fig.tight_layout();out.mkdir(parents=True,exist_ok=True)
        fig.savefig(out/'tof-errors.png',dpi=180);plt.close(fig);created+=1
    gyro=records(base/'imu_samples.csv')
    by=defaultdict(list)
    for r in gyro:
        t=num(r.get('t_s'));h=num(r.get('integrated_heading_deg'))
        if t is not None and h is not None and str(r.get('condition','')).lower()=='stationary':by[r.get('test_id','unlabeled')].append((t,h))
    if by:
        fig,ax=plt.subplots(figsize=(9,5))
        for name,xy in sorted(by.items()):
            xy.sort();ax.plot([x for x,y in xy],[y for x,y in xy],label=name)
        ax.set(xlabel='Time (s)',ylabel='Integrated heading (degrees)',title='V4.1 BMI088 stationary drift')
        ax.legend();ax.grid(alpha=.2);fig.tight_layout();out.mkdir(parents=True,exist_ok=True)
        fig.savefig(out/'bmi088-stationary-drift.png',dpi=180);plt.close(fig);created+=1
    cam=records(base/'camera_trials.csv')
    by=defaultdict(list)
    for r in cam:by[(r.get('lighting',''),r.get('reference_color',''))].append(r)
    if by:
        names=[];rates=[]
        for key,rows in sorted(by.items()):
            right=sum((r.get('reference_color','').strip().lower()==r.get('detected_color','').strip().lower()) for r in rows)
            names.append(f'{key[1]} | {key[0]}'); rates.append(100*right/len(rows))
        fig,ax=plt.subplots(figsize=(max(8,len(names)*1.6),5))
        ax.bar(names,rates)
        ax.set(ylabel='Correct classifications (%)',ylim=(0,100),title='V4.1 RT1062 recorded classification trials')
        ax.tick_params(axis='x',labelrotation=35);fig.tight_layout();out.mkdir(parents=True,exist_ok=True)
        fig.savefig(out/'rt1062-classification.png',dpi=180);plt.close(fig);created+=1
    print(f'Generated {created} plots from recorded physical data. No physical samples -> no plots.')

if __name__=='__main__':main()
