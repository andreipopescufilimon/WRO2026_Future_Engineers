# Mechanical CAD and fabrication files | HyperLine V4.1

This directory contains the **current published STEP reference parts** and a separate archive of **earlier V3/V4.0 STL models**. The competition vehicle uses a PCB-based structural chassis, front parallel steering and a rear mechanical differential. For complete vehicle assembly consult [Mobility](../docs/02-mobility.md) and [Reproducibility](../docs/06-reproducibility.md).

## Current published STEP inventory

| File | Part / assembly scope | Reproduction note |
|---|---|---|
| [`battery-mount.step`](battery-mount.step) | Battery retention bracket | Confirm installed battery dimensions, connector access and clearances |
| [`drive-system-mount.step`](drive-system-mount.step) | Rear drive-system support | Verify bearing fits, gear mesh and fastener spacing in the assembled robot |
| [`front-TOF-mount.step`](front-TOF-mount.step) | Front ToF sensor mount | Confirm zone orientation and unobstructed optical path |
| [`steering and camera mounting.step`](steering%20and%20camera%20mounting.step) | Front steering and camera support | Verify servo travel and camera position after assembly |
| [`steering bar.step`](steering%20bar.step) | Parallel steering linkage | Check joint clearance and side-to-side symmetry |
| [`steering-hub-left.step`](steering-hub-left.step) | Left steering hub | Mate with the right hub and front axle in CAD |
| [`steering-right.step`](steering-right.step) | Right steering hub | Verify mirrored handedness and clearances |
| [`wheel with silicone.step`](wheel%20with%20silicone.step) | Wheel geometry with tyre representation | Remeasure effective rolling diameter on the installed material |

STEP files are neutral solid-geometry exchanges. They are not printer profiles or machine-ready toolpaths. Export STL/3MF from a CAD program only after checking units, orientation, fit and manufacturing allowances.

## Historical parts, not the current bill of materials

- [`old-3D-models/V3`](old-3D-models/V3/) preserves the V3 version including impeller components.
- [`old-3D-models/V4.0`](old-3D-models/V4.0/) preserves older rims, steering hubs and drivetrain parts.
- The **impeller/fan parts are historical**. The V4.1 documentation says that the downforce fan is not installed. Do not fabricate and install those old parts merely because they are present in the archive.
- Some supporting manufacturing drawings are available in [`docs/technical-drawings`](../docs/technical-drawings/). Their filenames and designs may correspond to earlier revisions; compare the shape to the current STEP part before fabrication.

## Rebuilding the mechanical subsystem

1. Open the eight current STEP files using a millimeter-based CAD project and inspect mating faces and clearances.
2. Inspect the PCB chassis and electrical board outline under [`electrical-schematics/`](../electrical-schematics/). The PCB chassis is not replaced by a generic printed plate.
3. Fit and test the front steering linkage with the drive motor switched off. Center the servo at 85 and check both endpoints (30 and 140).
4. Check rear differential freedom, wheel runout, actual tyre contact diameter, wiring strain relief and sensor occlusion.
5. Record the actual printed/machined process, material, layer height (if printed), fasteners, tolerances and observed test measurements during assembly.
6. Repeat dimensional checks after any wheel, steering or gear revision.