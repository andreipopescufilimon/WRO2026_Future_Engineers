import sensor
import time
from machine import UART, LED

uart = UART(1, 19200)

# ================================================================
#  OpenMV cube detection + robust 50/100/150 mm sign classification
#  CLOSE is decided ONLY from the cube's bottom Y in the follow ROI.
#  A TOP-clipped sign while RAW FAR is 150 mm (camera geometry rule).
#
#  FIX: Never classify, lock, or send RED/GREEN while either the
#  full-image sign or the follow cube is clipped at the left/right
#  edge. Invalidate any old classification when this happens.
# ================================================================

DEBUG = True
DEBUG_ROI = True
DEBUG_RAW = False
DEBUG_ACCEPTED = True
DEBUG_PRINT = True
DEBUG_CUBE_DISTANCE = True
DEBUG_SURPRISE = True
DEBUG_UART = True

# ================================================================
# Camera
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
# Color thresholds / regions
# ================================================================

red_threshold = [(31, 71, 19, 47, 3, 71)]
green_threshold = [(18, 70, -70, -10, -5, 60)]

img_h = sensor.height()
img_w = sensor.width()
cubes_roi = (0, int(img_h * 0.542), img_w, int(img_h * 0.30))
surprise_roi = (0, 0, img_w, img_h)

min_cube_size = 300
min_valid_cube_area = 300
min_cube_density = 0.1
min_cube_width = 6
min_cube_height = 6

# ================================================================
# Height-based sign rules
# ================================================================

SURPRISE_RULE_ENABLED = True

# Ratios observed on the original camera:
#   100 mm: about 2.05
#   150 mm: about 2.92
SURPRISE_RATIO_50MM = 1.20
SURPRISE_RATIO_150MM = 2.00

SURPRISE_MIN_BLOB_PIXELS = 45
SURPRISE_MIN_BLOB_AREA = 45
SURPRISE_MIN_WIDTH_PX = 7
SURPRISE_MIN_HEIGHT_PX = 8
SURPRISE_MIN_DENSITY = 0.10
SURPRISE_EDGE_MARGIN_PX = 6  # FIX: left/right edge safety margin
SURPRISE_MIN_PLAUSIBLE_RATIO = 0.60
SURPRISE_MAX_PLAUSIBLE_RATIO = 4.00
SURPRISE_TOP_MARGIN_PX = 2

# Camera-specific geometry assumption:
# A 100 mm sign's TOP is ALWAYS visible before RAW CLOSE.
# Thus, a top-cropped sign while RAW FAR is considered 150 mm.
# This rule is ONLY used if the LEFT and RIGHT sides are visible.

SURPRISE_RESET_AFTER_MISSING_FRAMES = 3
SURPRISE_NEW_TARGET_JUMP_PX = 65
SURPRISE_NEW_TARGET_BOTTOM_DROP_PX = 45
SURPRISE_FOLLOW_MATCH_X_PX = 55
SURPRISE_REQUIRE_HEIGHT_FOR_COLOR = True

surprise_locked = False
surprise_has_full_measurement = False
surprise_height_mm = None
surprise_effective_color = None
surprise_ratio = None
surprise_observed_ratio = None
surprise_ratio_source = 'WAIT'
surprise_tracked_visible_color = None
surprise_last_center_x = None
surprise_last_bottom = None
surprise_missing_frames = 0

# ================================================================
# CLOSE + follow PD
# ================================================================

CLOSE_BOTTOM_Y = 170
CLOSE_CONFIRM_FRAMES = 1
close_counter = 0

kp_cube = 0.60
kd_cube = 1.20
pid_last_error = 0.0
max_cube_derivative = 0.35


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
    return max(blobs, key=lambda b: b.area)


def draw_label(img, blob, color, text):
    img.draw_rectangle(blob.rect, color=color)
    img.draw_cross((blob.cx, blob.cy), color=color)
    img.draw_string((blob.x, blob.y + blob.h - 10), text, color=color)


def valid_cube_blob(blob):
    if not blob:
        return False
    if blob.area < min_valid_cube_area:
        return False
    if blob.w < min_cube_width or blob.h < min_cube_height:
        return False
    if blob.density < min_cube_density:
        return False
    if blob.w > 0 and blob.h / blob.w > 4.0:
        return False
    return True


def choose_best_cube(red_blobs, green_blobs):
    red_cube = get_largest_blob([b for b in red_blobs if valid_cube_blob(b)])
    green_cube = get_largest_blob([b for b in green_blobs if valid_cube_blob(b)])

    if red_cube and green_cube:
        if red_cube.area > green_cube.area * 1.15:
            return 'R', red_cube
        if green_cube.area > red_cube.area * 1.15:
            return 'G', green_cube
        if red_cube.cy >= green_cube.cy:
            return 'R', red_cube
        return 'G', green_cube
    if red_cube:
        return 'R', red_cube
    if green_cube:
        return 'G', green_cube
    return None, None


def opposite_color(color_char):
    return 'G' if color_char == 'R' else 'R'


def is_side_clipped(blob):
    """A bounding box that touches either horizontal image edge is unsafe."""
    if blob is None:
        return False
    return (blob.x <= SURPRISE_EDGE_MARGIN_PX or
            blob.x + blob.w >= img_w - SURPRISE_EDGE_MARGIN_PX)


def valid_surprise_blob(blob):
    if blob.area < SURPRISE_MIN_BLOB_AREA:
        return False
    if blob.w < SURPRISE_MIN_WIDTH_PX or blob.h < SURPRISE_MIN_HEIGHT_PX:
        return False
    if blob.density < SURPRISE_MIN_DENSITY:
        return False
    ratio = float(blob.h) / float(blob.w)
    return SURPRISE_MIN_PLAUSIBLE_RATIO <= ratio <= SURPRISE_MAX_PLAUSIBLE_RATIO


def choose_surprise_cube(red_blobs, green_blobs, follow_color, follow_cube):
    candidates = []
    for b in red_blobs:
        if valid_surprise_blob(b):
            candidates.append(('R', b))
    for b in green_blobs:
        if valid_surprise_blob(b):
            candidates.append(('G', b))
    if not candidates:
        return None, None

    if follow_cube is not None:
        matching = [(c, b) for c, b in candidates
                    if c == follow_color and
                    abs(b.cx - follow_cube.cx) <= SURPRISE_FOLLOW_MATCH_X_PX]
        if not matching:
            return None, None
        return min(matching, key=lambda item: abs(item[1].cx - follow_cube.cx))

    if surprise_tracked_visible_color is not None and surprise_last_center_x is not None:
        same = [(c, b) for c, b in candidates
                if c == surprise_tracked_visible_color and
                abs(b.cx - surprise_last_center_x) <= SURPRISE_NEW_TARGET_JUMP_PX and
                (surprise_last_bottom is None or
                 b.y + b.h >= surprise_last_bottom - SURPRISE_NEW_TARGET_BOTTOM_DROP_PX)]
        if same:
            return min(same, key=lambda item: abs(item[1].cx - surprise_last_center_x))

    return max(candidates, key=lambda item: item[1].area)


def classify_sign_ratio(ratio, visible_color):
    # Only use this when the full bounding box is inside the image.
    if ratio < SURPRISE_RATIO_50MM:
        return 50, opposite_color(visible_color)
    if ratio < SURPRISE_RATIO_150MM:
        return 100, visible_color
    return 150, opposite_color(visible_color)


def reset_surprise_state():
    global surprise_locked, surprise_has_full_measurement
    global surprise_height_mm, surprise_effective_color, surprise_ratio
    global surprise_observed_ratio, surprise_ratio_source
    surprise_locked = False
    surprise_has_full_measurement = False
    surprise_height_mm = None
    surprise_effective_color = None
    surprise_ratio = None
    surprise_observed_ratio = None
    surprise_ratio_source = 'WAIT'


def invalidate_side_partial():
    """FIX: throw away stale height/color while left/right is cropped."""
    global surprise_locked, surprise_has_full_measurement
    global surprise_height_mm, surprise_effective_color, surprise_ratio
    global surprise_ratio_source
    surprise_locked = False
    surprise_has_full_measurement = False
    surprise_height_mm = None
    surprise_effective_color = None
    surprise_ratio = None
    surprise_ratio_source = 'SIDE PARTIAL'


def update_surprise_state(cube, visible_color, follow_cube=None):
    global surprise_locked, surprise_has_full_measurement
    global surprise_height_mm, surprise_effective_color, surprise_ratio
    global surprise_observed_ratio, surprise_ratio_source

    if not SURPRISE_RULE_ENABLED:
        return

    # FIX: Check BEFORE ratio validation or RAW CLOSE locking.
    if is_side_clipped(cube) or is_side_clipped(follow_cube):
        invalidate_side_partial()
        return

    if cube.w < SURPRISE_MIN_WIDTH_PX or cube.h < SURPRISE_MIN_HEIGHT_PX:
        return

    measured_ratio = float(cube.h) / float(cube.w)
    if not (SURPRISE_MIN_PLAUSIBLE_RATIO <= measured_ratio <= SURPRISE_MAX_PLAUSIBLE_RATIO):
        return
    surprise_observed_ratio = measured_ratio

    top_clipped = cube.y <= SURPRISE_TOP_MARGIN_PX
    bottom_clipped = cube.y + cube.h >= img_h - SURPRISE_EDGE_MARGIN_PX

    # FOLLOW determines RAW CLOSE whenever a follow blob exists.
    close_reference = follow_cube if follow_cube is not None else cube
    raw_close = (close_reference.y + close_reference.h) >= CLOSE_BOTTOM_Y

    if raw_close:
        # Freeze already known classes, but only while sides are visible.
        if surprise_height_mm is not None:
            surprise_locked = True
            surprise_ratio_source = 'LOCKED CLOSE'
            return

        # First sighting at RAW CLOSE must be fully visible.
        if top_clipped or bottom_clipped:
            surprise_ratio_source = 'CLOSE ?'
            return
    else:
        # Top-clipped + RAW FAR implies 150 mm only with sides visible.
        if top_clipped:
            if surprise_has_full_measurement:
                surprise_ratio_source = 'FULL SAVED'
                return
            if bottom_clipped:
                surprise_ratio_source = 'TOP ?'
                return
            surprise_height_mm = 150
            surprise_effective_color = opposite_color(visible_color)
            surprise_ratio = measured_ratio
            surprise_ratio_source = 'TOP FAR 150'
            return

    # Ordinary ratio-based classification on complete bounding boxes.
    if bottom_clipped:
        surprise_ratio_source = 'PARTIAL'
        return
    if surprise_locked:
        surprise_ratio_source = 'LOCKED'
        return

    surprise_ratio = measured_ratio
    surprise_height_mm, surprise_effective_color = classify_sign_ratio(
        measured_ratio, visible_color)
    surprise_has_full_measurement = True
    surprise_ratio_source = 'FULL'


def draw_surprise_debug(img, cube, visible_color, effective_color):
    if not (DEBUG and DEBUG_SURPRISE and SURPRISE_RULE_ENABLED):
        return
    height_text = '{}mm'.format(surprise_height_mm) if surprise_height_mm is not None else '?'
    ratio_text = '{:.2f}'.format(surprise_observed_ratio) if surprise_observed_ratio is not None else '?'
    img.draw_string((2, 62), 'SIGN {} R:{} {}'.format(height_text, ratio_text,
                                                       surprise_ratio_source),
                    color=(255, 255, 255))
    img.draw_string((2, 74), 'VISIBLE {} -> USED {}'.format(visible_color, effective_color),
                    color=(255, 255, 0) if visible_color != effective_color
                    else (255, 255, 255))


def get_cube_distance_data(cube):
    cube_bottom = cube.y + cube.h
    distance_to_bottom = img_h - cube_bottom
    raw_close = cube_bottom >= CLOSE_BOTTOM_Y
    return cube_bottom, distance_to_bottom, raw_close


def draw_cube_distance_debug(img, cube, color, cube_bottom, distance_to_bottom,
                             raw_close, confirmed_close):
    cx = cube.cx
    DIST_COLOR = (0, 255, 255)
    TEXT_COLOR = (255, 0, 255)
    CLOSE_LINE_COLOR = (255, 255, 0)
    BOTTOM_COLOR = (0, 255, 0)
    FAR_COLOR = (0, 120, 255)

    img.draw_line((0, CLOSE_BOTTOM_Y, img_w, CLOSE_BOTTOM_Y), color=CLOSE_LINE_COLOR)
    img.draw_string((2, CLOSE_BOTTOM_Y - 10), 'CLOSE Y {}'.format(CLOSE_BOTTOM_Y),
                    color=CLOSE_LINE_COLOR)
    img.draw_line((0, img_h - 2, img_w, img_h - 2), color=BOTTOM_COLOR)
    img.draw_string((2, img_h - 14), 'BOTTOM', color=BOTTOM_COLOR)
    img.draw_line((cx, cube_bottom, cx, img_h - 2), color=DIST_COLOR)
    img.draw_circle((cx, cube_bottom, 4), color=DIST_COLOR)
    img.draw_cross((cx, cube_bottom), color=DIST_COLOR)
    img.draw_line((cx - 10, cube_bottom, cx + 10, cube_bottom), color=DIST_COLOR)

    mid_y = cube_bottom + (distance_to_bottom // 2)
    label_x = cx + 6
    if label_x > img_w - 95:
        label_x = cx - 90
    img.draw_string((label_x, max(0, mid_y - 6)),
                    'dist {}'.format(distance_to_bottom), color=DIST_COLOR)

    status = 'CLOSE' if confirmed_close else 'FOLLOW'
    raw_status = 'RAW CLOSE' if raw_close else 'RAW FAR'
    img.draw_string((2, 2), 'BOTTOM DIST DEBUG', color=TEXT_COLOR)
    img.draw_string((2, 14), 'cube_bottom: {}'.format(cube_bottom), color=TEXT_COLOR)
    img.draw_string((2, 26), 'dist_bottom: {}'.format(distance_to_bottom), color=TEXT_COLOR)
    img.draw_string((2, 38), raw_status,
                    color=CLOSE_LINE_COLOR if raw_close else FAR_COLOR)
    img.draw_string((2, 50), status, color=color)
    img.draw_string((cube.x, max(0, cube.y - 12)),
                    'bottom {} dist {}'.format(cube_bottom, distance_to_bottom),
                    color=TEXT_COLOR)

# ================================================================
# Main loop
# ================================================================

while True:
    clock.tick()
    img = sensor.snapshot()
    target_x = img_w // 2

    # FOLLOW sees only the lower ROI.
    red_raw = img.find_blobs(
        red_threshold, roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size, merge=True, margin=10)
    green_raw = img.find_blobs(
        green_threshold, roi=cubes_roi,
        pixels_threshold=min_cube_size,
        area_threshold=min_cube_size, merge=True, margin=10)

    red_blobs = [b for b in red_raw if valid_cube_blob(b)]
    green_blobs = [b for b in green_raw if valid_cube_blob(b)]
    color_char, cube = choose_best_cube(red_blobs, green_blobs)

    # SIGN sees the entire frame, but does NOT steer.
    surprise_color = None
    surprise_cube = None
    if SURPRISE_RULE_ENABLED:
        surprise_red_raw = img.find_blobs(
            red_threshold, roi=surprise_roi,
            pixels_threshold=SURPRISE_MIN_BLOB_PIXELS,
            area_threshold=SURPRISE_MIN_BLOB_AREA,
            merge=True, margin=5)
        surprise_green_raw = img.find_blobs(
            green_threshold, roi=surprise_roi,
            pixels_threshold=SURPRISE_MIN_BLOB_PIXELS,
            area_threshold=SURPRISE_MIN_BLOB_AREA,
            merge=True, margin=5)

        surprise_color, surprise_cube = choose_surprise_cube(
            surprise_red_raw, surprise_green_raw, color_char, cube)

        if surprise_cube is not None:
            new_target = (
                surprise_tracked_visible_color is None or
                surprise_tracked_visible_color != surprise_color or
                (surprise_last_center_x is not None and
                 abs(surprise_cube.cx - surprise_last_center_x) > SURPRISE_NEW_TARGET_JUMP_PX) or
                (surprise_last_bottom is not None and
                 surprise_cube.y + surprise_cube.h <
                 surprise_last_bottom - SURPRISE_NEW_TARGET_BOTTOM_DROP_PX)
            )
            if new_target:
                reset_surprise_state()
                close_counter = 0
                pid_last_error = 0.0

            surprise_tracked_visible_color = surprise_color
            surprise_last_center_x = surprise_cube.cx
            surprise_last_bottom = surprise_cube.y + surprise_cube.h
            surprise_missing_frames = 0

            # FIX: side-clipped cube can never create/keep a class.
            if is_side_clipped(surprise_cube) or is_side_clipped(cube):
                invalidate_side_partial()
            else:
                update_surprise_state(surprise_cube, surprise_color, cube)
        else:
            # FIX: even if the full-image blob failed the ratio filter,
            # the follow ROI may still reveal that it touches the edge.
            if is_side_clipped(cube):
                invalidate_side_partial()

            if cube is not None and surprise_tracked_visible_color != color_char:
                reset_surprise_state()
                surprise_tracked_visible_color = None
                surprise_last_center_x = None
                surprise_last_bottom = None
            elif cube is None:
                surprise_missing_frames += 1
                if surprise_missing_frames >= SURPRISE_RESET_AFTER_MISSING_FRAMES:
                    reset_surprise_state()
                    surprise_tracked_visible_color = None
                    surprise_last_center_x = None
                    surprise_last_bottom = None

    # Draw after find_blobs so overlays are never detected as blobs.
    if DEBUG and DEBUG_ROI:
        img.draw_rectangle(cubes_roi, color=(0, 255, 0))
        img.draw_string((cubes_roi[0] + 3, cubes_roi[1] + 3),
                        'FOLLOW ROI', color=(0, 255, 0))
        if SURPRISE_RULE_ENABLED:
            img.draw_rectangle(surprise_roi, color=(255, 0, 255))

    if DEBUG and DEBUG_RAW:
        for b in red_raw:
            img.draw_rectangle(b.rect, color=(120, 0, 0))
        for b in green_raw:
            img.draw_rectangle(b.rect, color=(0, 120, 0))

    if DEBUG and DEBUG_SURPRISE and surprise_cube is not None:
        img.draw_rectangle(surprise_cube.rect, color=(255, 0, 255))
        early_used = surprise_effective_color if surprise_effective_color is not None else '?'
        draw_surprise_debug(img, surprise_cube, surprise_color, early_used)

    if cube is not None:
        draw_color = (255, 0, 0) if color_char == 'R' else (0, 255, 0)
        label = 'RED' if color_char == 'R' else 'GREEN'
        if DEBUG_ACCEPTED:
            draw_label(img, cube, draw_color, label)

        error = (cube.cx - target_x) / float(target_x)
        derivative = clamp(error - pid_last_error,
                           -max_cube_derivative, max_cube_derivative)
        pid_output = kp_cube * error + kd_cube * derivative
        pid_last_error = error

        cube_bottom, distance_to_bottom, raw_close = get_cube_distance_data(cube)
        if raw_close:
            close_counter += 1
        else:
            close_counter = 0
        confirmed_close = close_counter >= CLOSE_CONFIRM_FRAMES

        if DEBUG and DEBUG_CUBE_DISTANCE:
            draw_cube_distance_debug(img, cube, draw_color, cube_bottom,
                                     distance_to_bottom, raw_close, confirmed_close)

        if not confirmed_close:
            send('S{:+.3f}\n'.format(pid_output))
            if DEBUG and DEBUG_PRINT:
                print('{} FOLLOW pid:{:+.3f} bottom:{} sign:{} ratio:{} source:{}'.format(
                    label, pid_output, cube_bottom, surprise_height_mm,
                    surprise_observed_ratio, surprise_ratio_source))
        else:
            # FIX: never send a height-based command unless BOTH views
            # confirm the cube is fully visible from left to right.
            sides_visible = (
                surprise_cube is not None and
                not is_side_clipped(surprise_cube) and
                not is_side_clipped(cube)
            )

            if SURPRISE_RULE_ENABLED:
                used_color = surprise_effective_color if sides_visible else None
            else:
                used_color = color_char

            if SURPRISE_RULE_ENABLED and used_color is None:
                if not sides_visible or SURPRISE_REQUIRE_HEIGHT_FOR_COLOR:
                    send('S{:+.3f}\n'.format(pid_output))
                    if DEBUG and DEBUG_PRINT:
                        print('CLOSE WAIT HEIGHT/SIDES, no RED/GREEN command')
                else:
                    # If REQUIRE_HEIGHT is disabled, fallback is allowed
                    # only when both left and right sides are visible.
                    used_color = color_char

            if used_color is not None:
                command = 'RED\n' if used_color == 'R' else 'GREEN\n'
                send(command)
                if DEBUG_UART:
                    print('UART_TX={!r} visible:{} height:{} observed_ratio:{} source:{}'.format(
                        command, label, surprise_height_mm,
                        surprise_observed_ratio, surprise_ratio_source))
    else:
        pid_last_error *= 0.75
        close_counter = 0

    if DEBUG and DEBUG_PRINT:
        print('FPS:', clock.fps(), 'follow_red:', len(red_blobs),
              'follow_green:', len(green_blobs), 'surprise:',
              surprise_color if surprise_cube is not None else 'NONE',
              'height:', surprise_height_mm,
              'ratio:', surprise_observed_ratio,
              'source:', surprise_ratio_source)
