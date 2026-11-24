#!/usr/bin/env python3
"""Exercise final camera pure functions with controlled mock blobs, not camera hardware."""
from pathlib import Path
import ast, csv, argparse
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/camera.py'
parser=argparse.ArgumentParser()
parser.add_argument('--output',default='docs/validation/camera_source_contract.csv')
args=parser.parse_args()
src=SOURCE.read_text(encoding='utf-8')
tree=ast.parse(src, filename=str(SOURCE))
# Load literal constants and selected functions only; never initialize hardware or the frame loop.
names={'get_largest_blob','clamp','opposite_color','is_side_clipped','valid_cube_blob','choose_best_cube','valid_surprise_blob','classify_sign_ratio','reset_surprise_state','invalidate_side_partial','update_surprise_state','get_cube_distance_data','choose_surprise_cube'}
ns={'img_w':320,'img_h':240}
for n in tree.body:
    if isinstance(n,ast.Assign) and len(n.targets)==1 and isinstance(n.targets[0],ast.Name):
        try: ns[n.targets[0].id]=ast.literal_eval(n.value)
        except (ValueError,TypeError): pass
funcs=[n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name in names]
assert len(funcs)==len(names)
exec(compile(ast.fix_missing_locations(ast.Module(body=funcs,type_ignores=[])),str(SOURCE),'exec'),ns)
class Blob:
    def __init__(self,x,y,w,h):
        self.x,self.y,self.w,self.h=x,y,w,h
        self.area=w*h; self.density=0.9; self.cx=x+w//2; self.cy=y+h//2
rows=[]
def check(case,condition,description):
    rows.append([case,'PASS' if condition else 'FAIL',description,'SOURCE_FUNCTION_TEST_MOCK_INPUT'])
for colour in ('R','G'):
    opposite='G' if colour=='R' else 'R'
    for ratio,height,out in [(0.60,50,opposite),(1.19,50,opposite),(1.20,100,colour),(1.99,100,colour),(2.00,150,opposite),(4.00,150,opposite)]:
        check(f'RATIO-{colour}-{ratio}',ns['classify_sign_ratio'](ratio,colour)==(height,out),f'Ratio {ratio}: {height} mm, effective {out}')
check('CLAMP',ns['clamp'](1,-0.35,0.35)==0.35 and ns['clamp'](-1,-0.35,0.35)==-0.35,'Derivative clamp')
check('EDGE',ns['is_side_clipped'](Blob(6,30,20,40)) and ns['is_side_clipped'](Blob(294,30,20,40)) and not ns['is_side_clipped'](Blob(100,30,20,40)),'Both horizontal margins')
check('CLOSE',not ns['get_cube_distance_data'](Blob(100,129,20,40))[2] and ns['get_cube_distance_data'](Blob(100,130,20,40))[2],'Bottom Y 169 versus 170')
check('EMPTY',ns['get_largest_blob']([]) is None,'Empty blob list')
check('SIGN-FILTER',ns['valid_surprise_blob'](Blob(100,40,20,40)) and not ns['valid_surprise_blob'](Blob(100,40,20,100)),'Ratio plausibility filter')
ns['reset_surprise_state'](); ns['update_surprise_state'](Blob(100,40,20,30),'R')
check('FULL',ns['surprise_height_mm']==100 and ns['surprise_has_full_measurement'],'Full ratio classification')
ns['update_surprise_state'](Blob(100,130,20,40),'R',Blob(100,130,20,40))
check('LOCK',ns['surprise_locked'] and ns['surprise_height_mm']==100,'Close preserves existing height')
ns['update_surprise_state'](Blob(0,130,20,40),'R')
check('INVALIDATE',ns['surprise_height_mm'] is None and ns['surprise_effective_color'] is None and not ns['surprise_locked'],'Side clipping invalidates prior height and command')
ns['reset_surprise_state'](); ns['update_surprise_state'](Blob(100,0,20,40),'G')
check('TOP-FAR',ns['surprise_height_mm']==150 and ns['surprise_effective_color']=='R','Top-clipped far inference')
ns['reset_surprise_state'](); ns['update_surprise_state'](Blob(100,0,80,180),'G')
check('TOP-CLOSE',ns['surprise_height_mm'] is None,'First close observation with clipped top waits')
ns['reset_surprise_state'](); ns['update_surprise_state'](Blob(100,40,20,30),'G',Blob(0,130,20,40))
check('FOLLOW-EDGE',ns['surprise_height_mm'] is None,'Clipped follow view blocks classification')
check('UART','UART(1, 19200)' in src,'UART configuration present')
check('EXPOSURE','exposure_us=40000' in src and 'gain_db=24' in src,'Fixed exposure and gain present')
check('ENABLED',ns['SURPRISE_RULE_ENABLED'] and ns['SURPRISE_REQUIRE_HEIGHT_FOR_COLOR'],'Height classification required')
out=Path(args.output)
if not out.is_absolute():out=ROOT/out
with out.open('w',newline='',encoding='utf-8') as f:
    w=csv.writer(f,lineterminator='\n'); w.writerow(['case_id','result','test_description','evidence_class']);w.writerows(rows)
for row in rows: print(*row[:3])
assert all(row[1]=='PASS' for row in rows)
print(f'{len(rows)}/{len(rows)} source-only checks passed')
