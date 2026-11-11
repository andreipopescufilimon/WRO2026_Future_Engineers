#!/usr/bin/env python3
"""Unit-test exactly the pure functions extracted from the published OpenMV camera.py.

Runs original function bodies using Python AST, without camera hardware or OpenMV imports.
Tests software rules, not classification accuracy on real images.
"""
from pathlib import Path
import argparse
import ast
import csv
import sys

ROOT = Path(__file__).resolve().parents[1]
CAMERA = ROOT / 'src/camera.py'

parser = argparse.ArgumentParser()
parser.add_argument('--output', default='docs/validation/v41_camera_function_unit_tests.csv')
args = parser.parse_args()

tree = ast.parse(CAMERA.read_text(encoding='utf-8'), filename=str(CAMERA))
names = {'classify_cube_height', 'get_cube_height', 'get_passing_color'}
funcs = [n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name in names]
if {f.name for f in funcs} != names:
    raise SystemExit('Camera source missing height classification functions')
config_names = {'SURPRISE_RULE', 'UNKNOWN_CUBE_ACTION', 'HEIGHT_50_MAX_RATIO',
                'HEIGHT_100_MIN_RATIO', 'HEIGHT_100_MAX_RATIO', 'HEIGHT_150_MIN_RATIO'}
config = {}
for n in tree.body:
    if isinstance(n, ast.Assign) and len(n.targets) == 1 and isinstance(n.targets[0], ast.Name):
        if n.targets[0].id in config_names:
            config[n.targets[0].id] = ast.literal_eval(n.value)
if set(config) != config_names:
    raise SystemExit('Camera source missing critical height thresholds')
namespace = dict(config)
module = ast.fix_missing_locations(ast.Module(body=funcs, type_ignores=[]))
exec(compile(module, str(CAMERA), 'exec'), namespace)

cases = []
def add(case_id, kind, data, expected, actual):
    cases.append([case_id, kind, data, expected, actual,
                  'PASS' if str(expected) == str(actual) else 'FAIL',
                  'EXECUTED_ORIGINAL_PURE_FUNCTIONS'])

for i, (ratio, clipped, expected) in enumerate([
    (0.39, False, None), (0.40, False, 50), (1.0, False, 50),
    (1.30, False, 50), (1.31, False, None), (1.43, False, 100),
    (1.80, False, 100), (2.02, False, 100), (2.03, False, None),
    (2.12, False, 150), (2.50, False, 150), (4.50, False, 150),
    (4.51, False, None), (1.0, True, None), (2.2, True, 150)
], start=1):
    actual = namespace['classify_cube_height'](ratio, clipped)
    add(f'HEIGHT-FUNC-{i:02d}', 'ratio class',
        f'ratio={ratio}, top_clipped={clipped}', expected if expected is not None else 'UNKNOWN',
        actual if actual is not None else 'UNKNOWN')

for height in (50, 100, 150, None):
    for color, expected in [('R', 'G' if height in (50,150) else 'R'),
                            ('G', 'R' if height in (50,150) else 'G')]:
        track = {'height': height, 'candidate_reliable': False, 'candidate': None}
        actual = namespace['get_passing_color'](color, track)
        add(f'PASS-FUNC-{height or "UNKNOWN"}-{color}', 'passing side',
            f'height={height}, visible={color}', expected, actual)

output = Path(args.output)
if not output.is_absolute():
    output = ROOT / output
output.parent.mkdir(parents=True, exist_ok=True)
with output.open('w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f, lineterminator='\n')
    writer.writerow(['case_id', 'function', 'input', 'expected', 'actual', 'result', 'evidence_type'])
    writer.writerows(cases)

passed = sum(r[5] == 'PASS' for r in cases)
print(f'{passed}/{len(cases)} actual pure-function tests passed; output={output}')
if passed != len(cases):
    sys.exit(1)
