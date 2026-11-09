"""Regenerate V4.1 algorithm/configuration plots from docs/validation CSVs.

Run from any directory: python docs/validation/generate_charts.py
Requires matplotlib. These charts visualize code-derived records, NOT field tests.
"""
from pathlib import Path
import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

HERE=Path(__file__).resolve().parent
OUT=HERE.parent/'docs-media'
OUT.mkdir(parents=True,exist_ok=True)

def read(name):
    with (HERE/name).open(newline='',encoding='utf-8') as f:
        return list(csv.DictReader(f))

def number(x):return float(x)

def apply_style(ax):
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.grid(axis='y',alpha=0.18)
    ax.set_axisbelow(True)

def output(fig,name):
    fig.savefig(OUT/name,dpi=230,bbox_inches='tight',facecolor='white')
    fig.savefig(OUT/name.replace('.png','.svg'),bbox_inches='tight',facecolor='white')
    plt.close(fig)

# Steering: commands, not wheel angles
steer=read('steering_calibration.csv')
fig,ax=plt.subplots(figsize=(8.4,4.5),layout='constrained')
for group,label,style in [('NORMAL','Normal mode','o-'),('PARKING','Parking mode','s--')]:
    data=sorted((r for r in steer if r['steering_map']==group),key=lambda r:float(r['normalized_steering_input']))
    xs=[number(r['normalized_steering_input']) for r in data]
    ys=[number(r['effective_integer_write_deg']) for r in data]
    ax.plot(xs,ys,style,linewidth=2,markersize=6,label=label)
ax.set(xlabel='Normalized control request [-1, +1]',ylabel='Servo API write argument (degrees)',title='V4.1 steering command map')
ax.set_xticks([-1,-.5,0,.5,1]); ax.legend(frameon=False,loc='upper left'); apply_style(ax)
fig.text(.5,-.025,'Programmed servo values only. Physical front-wheel angles and turning radii are unmeasured.',ha='center',fontsize=9)
output(fig,'data-steering-command-map.png')

# Odometry: monotonic threshold, no physical distance
odo=read('odometry.csv')
odo=sorted(odo,key=lambda r:float(r['target_mm']))
xt=[number(r['target_mm']) for r in odo]
ct=[int(r['minimum_abs_ticks_for_stop']) for r in odo]
fig,ax=plt.subplots(figsize=(8.4,4.5),layout='constrained')
ax.scatter(xt,ct, s=38,label='Calculated first stopping tick',zorder=4)
xx=np.linspace(0,650,200)
ax.plot(xx,xx/1.831,ls='--',lw=1.5,label='Unrounded target / 1.831 mm per tick')
ax.set(xlabel='Commanded translation (mm)',ylabel='Minimum absolute encoder count (ticks)',title='V4.1 odometry command thresholds')
ax.legend(frameon=False,loc='upper left'); apply_style(ax)
fig.text(.5,-.025,'The 1.831 mm/tick setting comes from firmware; points are algorithm-derived, not measured travel.',ha='center',fontsize=9)
output(fig,'data-odometry-tick-thresholds.png')

# ToF scenarios: only discrete logic expectations
tof=read('tof_corners.csv')
labels=[r['scenario_name'].replace('_',' ') for r in tof]
base=[int(r['code_base_pwm']) for r in tof]
expected=[int(r['expected_speed_pwm']) for r in tof]
y=np.arange(len(tof))
fig,ax=plt.subplots(figsize=(10.2,6.2),layout='constrained')
ax.barh(y-.18,base,height=.34,label='Scenario base PWM')
ax.barh(y+.18,expected,height=.34,label='Expected output PWM')
for i,r in enumerate(tof):
    if r['expected_consumed_turn']=='1':
        ax.text(max(base[i],expected[i])+6,i,'turn',va='center',fontsize=9)
ax.set_yticks(y,labels,fontsize=9); ax.invert_yaxis(); ax.set_xlim(0,310)
ax.set(xlabel='8-bit PWM command (0–255)',title='Open Challenge ToF threshold logic: deterministic cases')
ax.legend(frameon=False,ncol=2,loc='lower right'); ax.grid(axis='x',alpha=.2); ax.set_axisbelow(True)
for s in ['top','right']:ax.spines[s].set_visible(False)
fig.text(.5,-.023,'Input distances and frame ages are simulated test vectors, not physical sensor measurements.',ha='center',fontsize=9)
output(fig,'data-tof-threshold-cases.png')

# PWM duty, not energy or electrical current
pwm=read('power_budget.csv')
labels=[r['trial_id'].replace('CFG-PWM-','').replace('_',' ').capitalize() for r in pwm]
y=np.arange(len(pwm))
vals=[number(r['nominal_duty_percent']) for r in pwm]
fig,ax=plt.subplots(figsize=(9.7,5.8),layout='constrained')
ax.barh(y,vals,height=.66)
ax.set_yticks(y,labels,fontsize=9);ax.invert_yaxis()
ax.set(xlabel='Nominal duty (% of 255)',title='Configured motor PWM levels, not measured electrical power')
ax.set_xlim(0,110);ax.grid(axis='x',alpha=.2);ax.set_axisbelow(True)
for i,v in enumerate(vals):ax.text(v+1,i,f'{v:.1f}%',va='center',fontsize=9)
for s in ['top','right']:ax.spines[s].set_visible(False)
fig.text(.5,-.025,'PWM duty alone does not determine motor current, battery input or torque.',ha='center',fontsize=9)
output(fig,'data-pwm-duty-commands.png')

# Gyro scripted target increments and absolute commands, never measured yaws
gyro=[r for r in read('gyro_test.csv') if r['commanded_yaw_deg'].strip()]
y=np.arange(len(gyro));vals=[number(r['commanded_yaw_deg']) for r in gyro]
labels=[r['command_context'] for r in gyro]
fig,ax=plt.subplots(figsize=(9.8,5.2),layout='constrained')
ax.barh(y,vals,height=.62)
ax.set_yticks(y,labels,fontsize=9);ax.invert_yaxis()
ax.axvline(0,color='#222',lw=.8)
ax.set(xlabel='Programmed yaw angle (degrees)',title='Gyro-guided scripted yaw targets and increments')
for i,v in enumerate(vals):ax.text(v+(4 if v>=0 else -4),i,f'{v:g}°',va='center',ha='left' if v>=0 else 'right',fontsize=9)
ax.grid(axis='x',alpha=.2); ax.set_axisbelow(True)
for s in ['top','right']:ax.spines[s].set_visible(False)
fig.text(.5,-.025,'Mix of absolute targets and relative increments. Not measured integrated yaw or angular error.',ha='center',fontsize=9)
output(fig,'data-gyro-commanded-yaw.png')

# Parking sequences: only encoder commanded segments
parking=read('parking.csv')
labels=[r['direction'].title()+' '+('exit' if 'EXIT' in r['trial_id'] else 'park') for r in parking]
y=np.arange(len(parking))
fw=[number(r['commanded_forward_mm']) for r in parking]
rev=[number(r['commanded_reverse_mm']) for r in parking]
fig,ax=plt.subplots(figsize=(8.6,4.5),layout='constrained')
ax.barh(y-.2,fw,height=.35,label='Commanded forward')
ax.barh(y+.2,rev,height=.35,label='Commanded reverse')
ax.set_yticks(y,labels);ax.invert_yaxis()
ax.set(xlabel='Sum of encoder-distance segments (mm)',title='Parking and exit: distance commands by sequence')
ax.legend(frameon=False,loc='lower right');ax.grid(axis='x',alpha=.2);ax.set_axisbelow(True)
for s in ['top','right']:ax.spines[s].set_visible(False)
fig.text(.5,-.025,'Excludes timed travel, ToF-driven approach and turns. Not measured displacement or parking clearance.',ha='center',fontsize=9)
output(fig,'data-parking-distance-commands.png')
print('Created 6 source-derived charts as PNG and SVG under', OUT)
