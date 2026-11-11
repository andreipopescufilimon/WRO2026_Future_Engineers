# V4.1 source and documentation preflight

This report records **static source and file checks only**. It does not prove Arduino compilation, camera execution, physical sensor precision or autonomous driving.

| Check | Result | Scope |
|---|---|---|
| Seven V4.1 firmware tabs present | PASS | HLR_FE_Separated.ino, Open.ino, Final.ino, functions.ino, motors.ino, gyro.ino, tof.ino |
| D11/D12 mode-selection definitions present | PASS | Static declarations, not physical switch verification |
| Four ToF I2C address definitions present | PASS | Seven-bit addresses, static source check |
| Four-by-four, 30 Hz ToF configuration present | PASS | Device operation and achieved frame rate not tested |
| 150 ms ToF freshness threshold present | PASS | Firmware configuration only |
| Camera UART 19,200 baud present | PASS | UART electrical link not tested |
| Firmware dependency headers declared | PASS | ESP32Encoder.h, ESP32Servo.h, SparkFun_VL53L5CX_Library.h, BMI088.h |
| Chapters 03 and 06 present | PASS | docs/03-power-sensors.md, docs/06-reproducibility.md |
| RT1062 terminology consistent | PASS | Both authored chapter bodies |
| All packaged media and validation links resolve | PASS | Local docs-media and docs/validation references resolved |

**Summary:** 10/10 checks passed.

[Source file checksums](source-sha256.json) allow another team to confirm that they are using the same exact sketch and camera script. A successful compilation log and physical tests remain separate evidence.
