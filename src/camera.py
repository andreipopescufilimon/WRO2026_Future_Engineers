import sensor
import time
from machine import UART, LED

# -------- DEBUG FLAG --------
DEBUG = False

# -------- Camera & Sensor Setup --------
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)       # QVGA: 320x240

sensor.set_vflip(True)
sensor.set_hmirror(True)

# Disable auto settings for stable color tracking.
# RT1062 uses the OV5640 sensor, so you may need to retune thresholds/exposure.
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=16000)

sensor.skip_frames(time=2000)
clock = time.clock()

# -------- UART & LED Setup --------
# RT1062 UART is on P4/P5:
# P4 = UART1 TX  -> connect to robot RX
# P5 = UART1 RX  -> connect to robot TX, optional if you only send data
# GND must be common.
# IMPORTANT: RT1062 I/O is 3.3 V only, not 5 V tolerant.
uart = UART(1, 19200, timeout_char=200)

red_led = LED("LED_RED")
green_led = LED("LED_GREEN")
blue_led = LED("LED_BLUE")


def leds_off():
    red_led.off()
    green_led.off()
    blue_led.off()


# Blink yellow (green + red) to indicate ready/qualification mode
green_led.on()
red_led.on()
time.sleep_ms(500)
leds_off()
time.sleep_ms(500)

# -------- Color Thresholds (LAB Space) --------
# These are copied from your H7 Plus code.
# Because RT1062 uses OV5640 instead of the H7 Plus sensor, retune in OpenMV IDE if needed.
red_threshold = [(35, 45, 28, 66, 11, 40), (29, 54, 21, 64, 6, 44)]
green_threshold = [(9, 24, -23, -12, -9, 21), (15, 42, -37, -11, -2, 30), (20, 41, -37, -11, -2, 30)]
blue_threshold = [(9, 76, -45, 27, -57, -8)]
orange_threshold = [(62, 91, -3, 43, 5, 69)]
pink_threshold = [(30, 70, 10, 60, -15, 15)]
black_threshold = [(0, 37, -26, 7, -17, 11)]

# -------- Define Regions of Interest (ROI) --------
img_h = sensor.height()
img_w = sensor.width()

cubes_roi = (0, int(img_h * 0.4), img_w, int(img_h * 0.6))
lines_roi = (5, int(img_h * 0.5), img_w - 10, int(img_h * 0.4))
wall_roi = (50, int(img_h * 0.5 - 18), img_w - 100, int(img_h * 0.2 - 10))
final_wall_roi = (30, 80, img_w - 60, img_h - 60)

# -------- Blob Filtering Parameters --------
min_cube_size = 500
min_line_size = 800
min_area = 10
min_valid_cube_area = 450
pink_wall_min_area = 5000
black_wall_min_area = 70000    # 7000
final_black_wall_min_area = 9000
min_black_height = 39

# -------- PD Parameters for Cube Following --------
kp_cube = 0.4
kd_cube = 2.4
pid_last_error = 0.0
follow_threshold = 5000

direction = 0  # 0 = not set, 1 = left, 2 = right


# -------- Helper Functions --------
def get_largest_blob(blobs):
    if not blobs:
        return None
    return max(blobs, key=lambda b: b.area())


def is_invalid_orange(orange_blob, red_blobs):
    # Ignore tall thin orange, or overlap logic with red blobs.
    if orange_blob.h() > orange_blob.w():
        return "ignore_orange"

    for r in red_blobs:
        overlap = (
            r.x() < orange_blob.x() + orange_blob.w() and
            r.x() + r.w() > orange_blob.x() and
            r.y() < orange_blob.y() + orange_blob.h() and
            r.y() + r.h() > orange_blob.y()
        )

        if overlap:
            red_area = r.area()
            orange_area = orange_blob.area()

            if red_area >= orange_area * 1.5:
                return "ignore_orange"
            elif orange_area >= red_area * 0.1:
                return "ignore_red"

    return None


def send(msg):
    uart.write(msg)


# -------- Main Loop --------
while True:
    clock.tick()
    img = sensor.snapshot()

    target_x = img_w // 2

    # ---- Detect Blobs ----
    red_blobs = img.find_blobs(
        red_threshold,
        roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size,
        merge=True
    )

    green_blobs = img.find_blobs(
        green_threshold,
        roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size,
        merge=True
    )

    blue_blobs = img.find_blobs(
        blue_threshold,
        roi=lines_roi,
        pixels_threshold=min_line_size,
        area_threshold=min_line_size,
        merge=True
    )

    orange_blobs = img.find_blobs(
        orange_threshold,
        roi=lines_roi,
        pixels_threshold=min_line_size,
        area_threshold=min_line_size,
        merge=True
    )

    # Main black wall detection.
    # Your old code overwrote black_blobs with final_wall_roi, so this keeps that final behavior.
    black_blobs = img.find_blobs(
        black_threshold,
        roi=final_wall_roi,
        pixels_threshold=final_black_wall_min_area,
        area_threshold=final_black_wall_min_area,
        merge=True
    )

    # ---- Get Largest Blobs ----
    red_cube = get_largest_blob([b for b in red_blobs if b.area() >= min_valid_cube_area])
    green_cube = get_largest_blob([b for b in green_blobs if b.area() >= min_valid_cube_area])
    blue_line = get_largest_blob([b for b in blue_blobs if b.area() >= min_area])
    orange_line = get_largest_blob([b for b in orange_blobs if b.area() >= min_area])
    black_blob = get_largest_blob(black_blobs)

    # ---- Detect Black Wall for Turns ----
    if black_blob:
        blob_h = black_blob.h()
        blob_area = black_blob.area()

        if blob_h >= min_black_height and blob_area >= black_wall_min_area:
            img.draw_rectangle(black_blob.rect(), color=(200, 200, 10))
            img.draw_string(
                black_blob.x(),
                black_blob.y() + blob_h - 10,
                "TURN",
                color=(255, 255, 255)
            )

            if DEBUG:
                print("Black wall: h={} area={}".format(blob_h, blob_area))

            #  send("BLACK\n")

    # ---- Choose Closest Cube ----
    candidates = []
    if red_cube:
        candidates.append(("R", red_cube))
    if green_cube:
        candidates.append(("G", green_cube))

    if candidates:
        color_char, cube = max(candidates, key=lambda x: x[1].area())
        area = cube.area()
        draw_color = (255, 0, 0) if color_char == "R" else (0, 255, 0)

        img.draw_rectangle(cube.rect(), color=draw_color)
        img.draw_cross(cube.cx(), cube.cy(), color=draw_color)
        img.draw_string(cube.x(), cube.y() + cube.h() - 10, str(area), color=draw_color)

        # PD on normalized error.
        error = (cube.cx() - target_x) / float(target_x)
        pid_output = kp_cube * error + kd_cube * (error - pid_last_error)
        pid_last_error = error

        if area < follow_threshold:
            send("S{:+.3f}\n".format(pid_output))

            if DEBUG:
                print("{} FOLLOW err:{:+.3f}, pid:{:+.3f}, area:{}".format(
                    "RED" if color_char == "R" else "GREEN",
                    error,
                    pid_output,
                    area
                ))
        else:
            send("RED\n" if color_char == "R" else "GREEN\n")

            if DEBUG:
                print("{} CLOSE area {} >= {}".format(
                    "RED" if color_char == "R" else "GREEN",
                    area,
                    follow_threshold
                ))

    # ---- Process Line Following ----
    valid_orange = None
    if orange_line and not is_invalid_orange(orange_line, red_blobs):
        valid_orange = orange_line

    chosen_line = None
    chosen_color = None

    if blue_line and valid_orange:
        if blue_line.area() > valid_orange.area():
            chosen_line, chosen_color = blue_line, "BLUE"
        else:
            chosen_line, chosen_color = valid_orange, "ORANGE"
    elif blue_line:
        chosen_line, chosen_color = blue_line, "BLUE"
    elif valid_orange:
        chosen_line, chosen_color = valid_orange, "ORANGE"

    if chosen_line:
        draw_color = (0, 0, 255) if chosen_color == "BLUE" else (255, 165, 0)
        img.draw_rectangle(chosen_line.rect(), color=draw_color)
        img.draw_string(
            chosen_line.x(),
            chosen_line.y() + chosen_line.h() - 10,
            str(chosen_line.area()),
            color=draw_color
        )

        #  send(chosen_color + "\n")

        if direction == 0:
            direction = 1 if chosen_color == "BLUE" else 2

    # Always send current turn direction.
    send(str(direction) + "\n")
