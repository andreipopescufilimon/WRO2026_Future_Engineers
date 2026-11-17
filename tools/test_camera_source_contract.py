#!/usr/bin/env python3
"""Test only the actual pure functions and source constants in the supplied OpenMV program.

These tests do not run a camera, assess image accuracy or validate a WRO field round.
"""
from pathlib import Path
import ast
import csv
import argparse

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/camera.py'
parser=argparse.ArgumentParser()
parser.add_argument('--output',default='docs/validation/camera_source_contract.csv')
args=parser.parse_args()
src=SOURCE.read_text(encoding='utf-8')
tree=ast.parse(src,filename=str(SOURCE))
funcs=[n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name in {'get_largest_blob','is_invalid_orange'}]
assert len(funcs)==2,'Expected original pure camera helpers are absent'
module=ast.fix_missing_locations(ast.Module(body=funcs,type_ignores=[]))
ns={}
exec(compile(module,str(SOURCE),'exec'),ns)
class Blob:
    def __init__(self,x,y,w,h,area):self._x=x;self._y=y;self._w=w;self._h=h;self._area=area
    def x(self):return self._x
    def y(self):return self._y
    def w(self):return self._w
    def h(self):return self._h
    def area(self):return self._area
rows=[]
def test(case,condition,description):rows.append([case,'PASS' if condition else 'FAIL',description,'EXECUTED_PURE_FUNCTION_FROM_BUNDLED_CAMERA'])
b1=Blob(0,0,20,20,400);b2=Blob(25,10,30,30,900)
test('BLOB-EMPTY',ns['get_largest_blob']([]) is None,'Empty candidates return None')
test('BLOB-LARGEST',ns['get_largest_blob']([b1,b2]) is b2,'Larger area wins')
test('ORANGE-TALL',ns['is_invalid_orange'](Blob(1,1,10,30,250),[])=='ignore_orange','Tall thin orange rejected')
test('ORANGE-NON-OVERLAP',ns['is_invalid_orange'](Blob(100,100,20,10,200),[b2]) is None,'Non-overlapping red does not suppress orange')
test('CAM-EXPOSURE','exposure_us=16000' in src,'Fixed exposure in current camera source is 16000 us')
test('CAM-AREA','follow_threshold = 5000' in src and 'area < follow_threshold' in src,'Area-based near rule is present')
test('CAM-UART','UART(1, 19200' in src,'Source sets UART(1) to 19200 baud')
test('CAM-DIR','send(str(direction) + "\\n")' in src,'Source emits numeric direction state')
test('CAM-HEIGHT-ABSENT','SURPRISE_RULE' not in src,'No unverified sign-height implementation attributed to actual source')
out=Path(args.output)
if not out.is_absolute():out=ROOT/out
out.parent.mkdir(parents=True,exist_ok=True)
with out.open('w',newline='',encoding='utf-8') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['case_id','result','test_description','evidence_class']);w.writerows(rows)
for r in rows:print(f'{r[1]} {r[0]}: {r[2]}')
assert all(r[1]=='PASS' for r in rows),'Source-contract regression failed'
print(f'Passed {len(rows)} of {len(rows)} source-only tests. Evidence: {out}')
