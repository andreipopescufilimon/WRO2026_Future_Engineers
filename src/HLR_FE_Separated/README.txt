HLR FE sketch, split into Open and Final rounds

Open the folder HLR_FE_Separated in Arduino IDE and open HLR_FE_Separated.ino.
Keep all .ino files in this folder. The folder name and main .ino name match.

Mode selection is sampled once when the board starts:
  D11 HIGH, D12 LOW  = Final round
  D12 HIGH, D11 LOW  = Open round
  Both HIGH or both LOW = safety stop with LED blinking
The inputs use INPUT_PULLDOWN and expect 3.3V HIGH, not 5V.
Restart the board after changing the selected mode.

Open.ino: open_configure, open_color_poll, open_execute, open_execute_command
Final.ino: final_configure, final_exit_parking, final_execute, final_color_poll,
           final_execute_command, final_park and final marker functions
functions.ino: shared TCS34725 low-level I2C, delays, pass_cube, debug helpers
motors.ino, gyro.ino, tof.ino: common hardware code

Open round corner-speed, turn geometry, ToF thresholds and stop behavior are retained.
Final round steering states, camera commands and parking sequences retain their original logic.
No hardware test or Arduino-board compilation was possible in this environment.
