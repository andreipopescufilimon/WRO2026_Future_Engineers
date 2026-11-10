# V4.1 reproduction evidence

`static-preflight-results.md` and `source-sha256.json` establish that the published V4.1 files are internally consistent at the source level. They do not prove successful firmware compilation or robot performance. The Power, ToF, BMI088, camera and challenge-run observations are recorded under [`../physical/`](../physical/).

The Windows script [`../../../scripts/capture_build_environment.ps1`](../../../scripts/capture_build_environment.ps1) writes the actual installed Arduino board/library versions and compiler log into `local-build/` on the development computer. The OpenMV RT1062 firmware build and IDE version must be recorded on the camera machine. A complete verified-release commit can be identified only after these files and the final validated sketch have been committed together.

The [assembly video](https://youtu.be/HSzWATB6nWQ) and the [public driving video](https://www.youtube.com/watch?v=bg92ulHXomw) are available project media. Neither is a substitute for a dated run log tying the recorded robot to a specific final V4.1 firmware revision.
