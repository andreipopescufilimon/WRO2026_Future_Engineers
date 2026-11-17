import sensor
import time
from machine import UART, LED


uart = UART(1, 19200)

# ================================================================
#      RT1062 CUBE DETECTION
#      CUBE CLOSE DETECTION ONLY BY DISTANCE TO BOTTOM
#      Updated for OpenMV 5.x drawing API and Blob attributes
# ================================================================

# ---------------- DEBUG OPTIONS ----------------
DEBUG = True

DEBUG_ROI = True
DEBUG_RAW = False
DEBUG_ACCEPTED = True
DEBUG_PRINT = True
DEBUG_CUBE_DISTANCE = True
DEBUG_SURPRISE = True


# ================================================================
# Camera Setup
# ================================================================

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

sensor.set_vflip(True)
sensor.set_hmirror(True)

sensor.skip_frames(time=3000)

rgb_gain_db = sensor.get_rgb_gain_db()
print(rgb_gain_db)

sensor.set_auto_whitebal(False, rgb_gain_db=(60.2, 61.8, 68.2))
sensor.set_auto_exposure(False, exposure_us=40000)
sensor.set_auto_gain(False, gain_db=24)

clock = time.clock()


# ================================================================
# LEDs
# ================================================================

red_led = LED("LED_RED")
green_led = LED("LED_GREEN")
blue_led = LED("LED_BLUE")


def leds_off():
    red_led.off()
    green_led.off()
    blue_led.off()


green_led.on()
red_led.on()
time.sleep_ms(300)
leds_off()
time.sleep_ms(300)


# ================================================================
# Thresholds
# ================================================================

red_threshold = [
    (17, 69, 23, 52, -4, 26)
]

green_threshold = [
    (11, 62, -75, -11, -10, 25)
]


# ================================================================
# ROIs
# ================================================================

img_h = sensor.height()
img_w = sensor.width()

# The ROI MUST start at camera Y=0. Otherwise a tall sign is cut off
# at the ROI edge before it can reach the actual top of the camera image.
# Keep the original bottom limit to avoid floor reflections.
cubes_roi = (0, 0, img_w, int(img_h * 0.79))


# ================================================================
# Filters
# ================================================================

min_cube_size = 500
min_valid_cube_area = 500
min_cube_density = 0.1
min_cube_width = 6
min_cube_height = 6


# ================================================================
# Surprise Rule: 50 x 50 mm base, height = 50 / 100 / 150 mm
# ================================================================

SURPRISE_RULE_ENABLED = True

# Height / width approximately 1, 2, 3 for the three sign heights.
# 100 mm = regular visible color; 50 and 150 mm = opposite color.
# Adjusted slightly for the 100 mm test, which looked closer to 1.5:1.
# Test all three heights on the real field to refine these limits.
SURPRISE_RATIO_50MM = 1.40
SURPRISE_RATIO_150MM = 1.9

# Only use a full bounding box of reasonable size.
SURPRISE_MIN_WIDTH_PX = 14
SURPRISE_MIN_HEIGHT_PX = 12
SURPRISE_EDGE_MARGIN_PX = 3
SURPRISE_MIN_PLAUSIBLE_RATIO = 0.60
SURPRISE_MAX_PLAUSIBLE_RATIO = 4.00

# Lock only when the top of the blob reaches the real camera top (Y=0).
# Keep the last FULL, valid measurement, not the already clipped ratio.
SURPRISE_TOP_LOCK_MARGIN_PX = 2

# Each tracked sign gets a new classification.
SURPRISE_RESET_AFTER_MISSING_FRAMES = 3
SURPRISE_NEW_TARGET_JUMP_PX = 65

surprise_locked = False
surprise_height_mm = None
surprise_effective_color = None
surprise_ratio = None
surprise_tracked_visible_color = None
surprise_last_center_x = None
surprise_missing_frames = 0


# ================================================================
# Close Detection Using Only Cube Bottom Y
# ================================================================

CLOSE_BOTTOM_Y = 148

CLOSE_CONFIRM_FRAMES = 1
close_counter = 0


# ================================================================
# Cube PD Follow
# ================================================================

kp_cube = 0.60
kd_cube = 1.20
pid_last_error = 0.0
max_cube_derivative = 0.35


# ================================================================
# Helpers
# ================================================================

def send(msg):
    uart.write(msg)


def clamp(value, low, high):
    if value < low:
        return low
    if value > high:
        return high
    return value


def get_largest_blob(blobs):
    if not blobs:
        return None
    # OpenMV 5.x: Blob fields are attributes, not methods.
    return max(blobs, key=lambda b: b.area)


def draw_label(img, blob, color, text):
    img.draw_rectangle(blob.rect, color=color)
    img.draw_cross((blob.cx, blob.cy), color=color)
    img.draw_string(
        (blob.x, blob.y + blob.h - 10),
        text,
        color=color
    )


def valid_cube_blob(blob):
    if not blob:
        return False

    if blob.area < min_valid_cube_area:
        return False

    if blob.w < min_cube_width or blob.h < min_cube_height:
        return False

    if blob.density < min_cube_density:
        return False

    # Reject thin reflections or vertical artifacts.
    if blob.w > 0:
        ratio = blob.h / blob.w
        if ratio > 4.0:
            return False

    return True


def choose_best_cube(red_blobs, green_blobs):
    red_cube = get_largest_blob([b for b in red_blobs if valid_cube_blob(b)])
    green_cube = get_largest_blob([b for b in green_blobs if valid_cube_blob(b)])

    if red_cube and green_cube:
        # Area selects between visible cubes, not closeness.
        if red_cube.area > green_cube.area * 1.15:
            return "R", red_cube

        if green_cube.area > red_cube.area * 1.15:
            return "G", green_cube

        # If similar, the lower cube is usually closer.
        if red_cube.cy >= green_cube.cy:
            return "R", red_cube
        else:
            return "G", green_cube

    if red_cube:
        return "R", red_cube

    if green_cube:
        return "G", green_cube

    return None, None


def opposite_color(color_char):
    return "G" if color_char == "R" else "R"


def read_complete_sign_ratio(cube):
    # Ignore too-small signs: their pixel dimensions are unstable.
    if cube.w < SURPRISE_MIN_WIDTH_PX or cube.h < SURPRISE_MIN_HEIGHT_PX:
        return None

    # After the sign touches the real camera top, its height is clipped.
    # Never include that partial height in the aspect-ratio estimate.
    if cube.y <= SURPRISE_TOP_LOCK_MARGIN_PX:
        return None

    roi_bottom = cubes_roi[1] + cubes_roi[3]
    if (cube.y + cube.h >= roi_bottom - SURPRISE_EDGE_MARGIN_PX or
            cube.x <= SURPRISE_EDGE_MARGIN_PX or
            cube.x + cube.w >= img_w - SURPRISE_EDGE_MARGIN_PX):
        return None

    ratio = float(cube.h) / float(cube.w)
    if ratio < SURPRISE_MIN_PLAUSIBLE_RATIO or ratio > SURPRISE_MAX_PLAUSIBLE_RATIO:
        return None
    return ratio

def classify_sign_ratio(ratio, visible_color):
    if ratio < SURPRISE_RATIO_50MM:
        return 50, opposite_color(visible_color)

    elif ratio < SURPRISE_RATIO_150MM:
        return 100, visible_color

    else:
        return 150, opposite_color(visible_color)

def reset_surprise_state():
    global surprise_locked, surprise_height_mm, surprise_effective_color
    global surprise_ratio
    surprise_locked = False
    surprise_height_mm = None
    surprise_effective_color = None
    surprise_ratio = None


def update_surprise_state(cube, visible_color):
    global surprise_locked, surprise_height_mm, surprise_effective_color
    global surprise_ratio

    if not SURPRISE_RULE_ENABLED or surprise_locked:
        return

    # When its top touches camera Y=0, freeze the LAST valid estimate.
    # At this point the current blob is truncated and unusable for ratio.
    if cube.y <= SURPRISE_TOP_LOCK_MARGIN_PX:
        if surprise_ratio is not None:
            surprise_locked = True
            if DEBUG and DEBUG_PRINT:
                print('SURPRISE LOCK top:{} ratio:{:.2f} height:{}mm used:{}'.format(
                    cube.y, surprise_ratio, surprise_height_mm, surprise_effective_color))
        return

    # Until then update from every good, complete bounding box.
    measurement = read_complete_sign_ratio(cube)
    if measurement is None:
        return

    # Use the latest FULL sign measurement, not its original distant ratio.
    # Deliberately do not average in the early incorrect measurements.
    surprise_ratio = measurement
    surprise_height_mm, surprise_effective_color = classify_sign_ratio(
        surprise_ratio, visible_color)


def draw_surprise_debug(img, cube, visible_color, effective_color):
    if not (DEBUG and DEBUG_SURPRISE and SURPRISE_RULE_ENABLED):
        return

    status = 'LOCKED' if surprise_locked else ('LIVE' if surprise_ratio is not None else 'WAIT')
    height_text = '{}mm'.format(surprise_height_mm) if surprise_height_mm is not None else '?'
    ratio_text = '{:.2f}'.format(surprise_ratio) if surprise_ratio is not None else '?'
    img.draw_string((2, 62), 'SIGN {} R:{} {}'.format(height_text, ratio_text, status),
                    color=(255, 255, 255))
    img.draw_string((2, 74), 'VISIBLE {} -> USED {}'.format(visible_color, effective_color),
                    color=(255, 255, 0) if visible_color != effective_color else (255, 255, 255))


def get_cube_distance_data(cube):
    cube_bottom = cube.y + cube.h
    distance_to_bottom = img_h - cube_bottom

    # Only bottom Y is used for CLOSE. No area/width score.
    raw_close = cube_bottom >= CLOSE_BOTTOM_Y

    return cube_bottom, distance_to_bottom, raw_close


def draw_cube_distance_debug(img, cube, color, cube_bottom, distance_to_bottom, raw_close, confirmed_close):
    cx = cube.cx

    DIST_COLOR = (0, 255, 255)        # cyan
    TEXT_COLOR = (255, 0, 255)        # magenta
    CLOSE_LINE_COLOR = (255, 255, 0)  # yellow
    BOTTOM_COLOR = (0, 255, 0)        # green
    FAR_COLOR = (0, 120, 255)         # blue-ish

    # Close trigger line
    img.draw_line(
        (0, CLOSE_BOTTOM_Y, img_w, CLOSE_BOTTOM_Y),
        color=CLOSE_LINE_COLOR
    )
    img.draw_string(
        (2, CLOSE_BOTTOM_Y - 10),
        "CLOSE Y {}".format(CLOSE_BOTTOM_Y),
        color=CLOSE_LINE_COLOR
    )

    # Image bottom reference line
    img.draw_line(
        (0, img_h - 2, img_w, img_h - 2),
        color=BOTTOM_COLOR
    )
    img.draw_string(
        (2, img_h - 14),
        "BOTTOM",
        color=BOTTOM_COLOR
    )

    # Distance from cube bottom to image bottom
    img.draw_line(
        (cx, cube_bottom, cx, img_h - 2),
        color=DIST_COLOR
    )

    # Cube bottom marker
    img.draw_circle((cx, cube_bottom, 4), color=DIST_COLOR)
    img.draw_cross((cx, cube_bottom), color=DIST_COLOR)

    # Horizontal marker on cube bottom
    img.draw_line(
        (cx - 10, cube_bottom, cx + 10, cube_bottom),
        color=DIST_COLOR
    )

    # Distance label
    mid_y = cube_bottom + (distance_to_bottom // 2)

    label_x = cx + 6
    if label_x > img_w - 95:
        label_x = cx - 90

    img.draw_string(
        (label_x, max(0, mid_y - 6)),
        "dist {}".format(distance_to_bottom),
        color=DIST_COLOR
    )

    status = "CLOSE" if confirmed_close else "FOLLOW"
    raw_status = "RAW CLOSE" if raw_close else "RAW FAR"

    img.draw_string((2, 2), "BOTTOM DIST DEBUG", color=TEXT_COLOR)
    img.draw_string((2, 14), "cube_bottom: {}".format(cube_bottom), color=TEXT_COLOR)
    img.draw_string((2, 26), "dist_bottom: {}".format(distance_to_bottom), color=TEXT_COLOR)
    img.draw_string(
        (2, 38),
        raw_status,
        color=CLOSE_LINE_COLOR if raw_close else FAR_COLOR
    )
    img.draw_string((2, 50), status, color=color)

    img.draw_string(
        (cube.x, max(0, cube.y - 12)),
        "bottom {} dist {}".format(cube_bottom, distance_to_bottom),
        color=TEXT_COLOR
    )


# ================================================================
# Main Loop
# ================================================================

while True:
    clock.tick()
    img = sensor.snapshot()

    target_x = img_w // 2

    # ============================================================
    # Cube Detection
    # ============================================================

    red_raw = img.find_blobs(
        red_threshold,
        roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size,
        merge=True,
        margin=10
    )

    green_raw = img.find_blobs(
        green_threshold,
        roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size,
        merge=True,
        margin=10
    )

    red_blobs = [b for b in red_raw if valid_cube_blob(b)]
    green_blobs = [b for b in green_raw if valid_cube_blob(b)]

    # Draw debug overlays AFTER detecting blobs, so the green ROI line
    # cannot accidentally be detected as a green obstacle.
    if DEBUG and DEBUG_ROI:
        img.draw_rectangle(cubes_roi, color=(0, 255, 0))
        img.draw_string(
            (cubes_roi[0] + 3, cubes_roi[1] + 3),
            "CUBES ROI",
            color=(0, 255, 0)
        )

    # ============================================================
    # Debug raw blobs
    # ============================================================

    if DEBUG and DEBUG_RAW:
        for b in red_raw:
            img.draw_rectangle(b.rect, color=(120, 0, 0))

        for b in green_raw:
            img.draw_rectangle(b.rect, color=(0, 120, 0))

    # ============================================================
    # Cube PD Following
    # UART commands:
    # S+0.123 while following
    # RED / GREEN when close
    # ============================================================

    color_char, cube = choose_best_cube(red_blobs, green_blobs)

    if cube:
        # Identify a new tracked target. The classification belongs to ONE sign,
        # not to every future sign in the run.
        new_target = (
            surprise_tracked_visible_color is None or
            surprise_tracked_visible_color != color_char or
            (surprise_last_center_x is not None and
             abs(cube.cx - surprise_last_center_x) > SURPRISE_NEW_TARGET_JUMP_PX)
        )

        if new_target:
            reset_surprise_state()
            close_counter = 0
            pid_last_error = 0.0

        surprise_tracked_visible_color = color_char
        surprise_last_center_x = cube.cx
        surprise_missing_frames = 0

        # Refresh on every complete measurement until the sign touches camera top.
        # The ratio is not fixed when the sign first appears.
        update_surprise_state(cube, color_char)

        # Use the most recent valid classification, whether LIVE or LOCKED.
        # Before any usable measurement, use the visible color as fallback.
        used_color = (surprise_effective_color
                      if SURPRISE_RULE_ENABLED and surprise_effective_color is not None
                      else color_char)

        if color_char == "R":
            draw_color = (255, 0, 0)
            label = "RED"
        else:
            draw_color = (0, 255, 0)
            label = "GREEN"

        if DEBUG_ACCEPTED:
            draw_label(img, cube, draw_color, label)

        draw_surprise_debug(img, cube, color_char, used_color)

        # Steering uses center X only.
        error = (cube.cx - target_x) / float(target_x)

        derivative = error - pid_last_error
        derivative = clamp(derivative, -max_cube_derivative, max_cube_derivative)

        pid_output = kp_cube * error + kd_cube * derivative
        pid_last_error = error

        # Distance logic using ONLY cube bottom Y.
        cube_bottom, distance_to_bottom, raw_close = get_cube_distance_data(cube)

        if raw_close:
            close_counter += 1
        else:
            close_counter = 0

        confirmed_close = close_counter >= CLOSE_CONFIRM_FRAMES

        if DEBUG and DEBUG_CUBE_DISTANCE:
            draw_cube_distance_debug(
                img,
                cube,
                draw_color,
                cube_bottom,
                distance_to_bottom,
                raw_close,
                confirmed_close
            )

        if not confirmed_close:
            send("S{:+.3f}\n".format(pid_output))

            if DEBUG and DEBUG_PRINT:
                print(
                    "{} FOLLOW err:{:+.3f} pid:{:+.3f} bottom:{} dist_to_bottom:{} close_count:{}".format(
                        label,
                        error,
                        pid_output,
                        cube_bottom,
                        distance_to_bottom,
                        close_counter
                    )
                )

        else:
            # UART sends the ORIGINAL color for a modified sign.
            if used_color == "R":
                send("RED\n")
            else:
                send("GREEN\n")

            if DEBUG and DEBUG_PRINT:
                print(
                    "{} CLOSE used:{} height:{} bottom:{} dist_to_bottom:{}".format(
                        label,
                        "RED" if used_color == "R" else "GREEN",
                        surprise_height_mm if surprise_height_mm is not None else "WAIT",
                        cube_bottom,
                        distance_to_bottom
                    )
                )

    else:
        # Prevent steering jump when cube appears again.
        pid_last_error *= 0.75
        close_counter = 0

        # A brief detection dropout should not discard the locked decision.
        surprise_missing_frames += 1
        if surprise_missing_frames >= SURPRISE_RESET_AFTER_MISSING_FRAMES:
            reset_surprise_state()
            surprise_tracked_visible_color = None
            surprise_last_center_x = None

    # ============================================================
    # General Debug Print
    # ============================================================

    if DEBUG and DEBUG_PRINT:
        print(
            "FPS:",
            clock.fps(),
            "red:",
            len(red_blobs),
            "green:",
            len(green_blobs),
            "cube:",
            color_char if cube else "NONE"
        )
