import sensor
import time
from machine import UART, LED


uart = UART(1, 19200)

# ================================================================
#      RT1062 CUBE DETECTION
#      CUBE CLOSE DETECTION ONLY BY DISTANCE TO BOTTOM
# ================================================================

# ---------------- DEBUG OPTIONS ----------------
DEBUG = True

DEBUG_ROI = True
DEBUG_RAW = False
DEBUG_ACCEPTED = True
DEBUG_PRINT = True
DEBUG_CUBE_DISTANCE = True


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

sensor.set_auto_whitebal(False, rgb_gain_db=(60.2, 61.2, 67.7))  # 60.2, 61.2, 67.7
sensor.set_auto_exposure(False, exposure_us=40000)  # 40000
sensor.set_auto_gain(False, gain_db=27)  # 27

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

# Cubes only
red_threshold = [
    (0, 61, 16, 52, -2, 50)  # (0, 46, 21, 65, -3, 32)
]

green_threshold = [
    (0, 65, -45, -9, -2, 35)  # (0, 50, -22, -11, -4, 27)
]


# ================================================================
# ROIs
# ================================================================

img_h = sensor.height()
img_w = sensor.width()

# Cube ROI only
cubes_roi = (0, int(img_h * 0.45), img_w, int(img_h * 0.30))


# ================================================================
# Filters
# ================================================================

min_cube_size = 500
min_valid_cube_area = 500
min_cube_density = 0.1
min_cube_width = 6
min_cube_height = 6


# ================================================================
# Close Detection Using Only Cube Bottom Y
# ================================================================

# This is the Y value where the cube is considered close.
CLOSE_BOTTOM_Y = 148

# Require close for multiple frames to avoid false triggers
CLOSE_CONFIRM_FRAMES = 1
close_counter = 0


# ================================================================
# Cube PD Follow
# ================================================================

kp_cube = 0.80
kd_cube = 3.20
pid_last_error = 0.0
max_cube_derivative = 0.35


# ================================================================
# Surprise Rule - Height Detection
# ================================================================

SURPRISE_RULE = True

# All three signs are 50 mm wide.
# A 50 mm sign has a square front face, so H/W is about 1:1.
# From the real RT1062 screenshots:
# 50 mm: 0.88, 0.98, 1.02, 1.15
# 100 mm: 1.72, 1.90 (an old incomplete view gave 1.19)
# 150 mm: 2.15, 2.25 (top cropping may reduce the ratio)
# Leave gaps between classes instead of forcing uncertain cases.
HEIGHT_50_MAX_RATIO = 1.30
HEIGHT_100_MIN_RATIO = 1.43
HEIGHT_100_MAX_RATIO = 2.02
HEIGHT_150_MIN_RATIO = 2.12

# Use a narrow vertical color strip through the center of the cube.
# This stops a person's arm or a red background from making the
# measured rectangle much wider than the actual traffic sign.
HEIGHT_STRIP_WIDTH_FACTOR = 0.36
HEIGHT_MAX_VERTICAL_GAP = 0.25
HEIGHT_MIN_WIDTH = 16
HEIGHT_MIN_COVERAGE = 0.42
HEIGHT_EDGE_MARGIN = 2

# Respond on the first reliable frame, then confirm with the next one.
# A lock normally stays until the same cube is lost. Before the close
# trigger, two strongly contradictory FULL measurements may repair a
# mistaken early lock. Never change it from a cropped/unclear image.
HEIGHT_CONFIRM_FRAMES = 2
HEIGHT_MAX_RATIO_CHANGE = 0.26
HEIGHT_CORRECTION_FRAMES = 2
TRACK_LOST_FRAMES = 6
TRACK_MAX_COUNT = 8
cube_tracks = []
next_cube_id = 1

# The Arduino already uses RED and GREEN. When height is genuinely
# unknown, keep the original visible-color behavior. This is NOT
# guaranteed to pass the modified sign; see the debug label DEFAULT.
UNKNOWN_CUBE_ACTION = "VISIBLE"  # "VISIBLE" or "UNKNOWN"

DEBUG_HEIGHT = True
DEBUG_HEIGHT_SHAPE = True


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
    return max(blobs, key=lambda b: b.area())


def draw_label(img, blob, color, text):
    img.draw_rectangle(blob.rect(), color=color)
    img.draw_cross(blob.cx(), blob.cy(), color=color)
    img.draw_string(blob.x(), blob.y() + blob.h() - 10, text, color=color)


def valid_cube_blob(blob):
    if not blob:
        return False

    if blob.area() < min_valid_cube_area:
        return False

    if blob.w() < min_cube_width or blob.h() < min_cube_height:
        return False

    if blob.density() < min_cube_density:
        return False

    # Reject thin reflections or vertical artifacts.
    if blob.w() > 0:
        ratio = blob.h() / blob.w()
        if ratio > 3.2:
            return False

    return True


def choose_best_cube(red_blobs, green_blobs):
    red_cube = get_largest_blob([b for b in red_blobs if valid_cube_blob(b)])
    green_cube = get_largest_blob([b for b in green_blobs if valid_cube_blob(b)])

    if red_cube and green_cube:
        # Area is only used to choose between two visible cubes,
        # not to decide if the cube is close.
        if red_cube.area() > green_cube.area() * 1.15:
            return "R", red_cube

        if green_cube.area() > red_cube.area() * 1.15:
            return "G", green_cube

        # If similar, the lower cube is usually closer.
        if red_cube.cy() >= green_cube.cy():
            return "R", red_cube
        else:
            return "G", green_cube

    if red_cube:
        return "R", red_cube

    if green_cube:
        return "G", green_cube

    return None, None


def get_cube_distance_data(cube):
    cube_bottom = cube.y() + cube.h()
    distance_to_bottom = img_h - cube_bottom

    # Only close condition.
    # No area, no width, no score.
    raw_close = cube_bottom >= CLOSE_BOTTOM_Y

    return cube_bottom, distance_to_bottom, raw_close


def draw_cube_distance_debug(img, cube, color, cube_bottom, distance_to_bottom, raw_close, confirmed_close):
    cx = cube.cx()

    DIST_COLOR = (0, 255, 255)        # cyan
    TEXT_COLOR = (255, 0, 255)        # magenta
    CLOSE_LINE_COLOR = (255, 255, 0)  # yellow
    BOTTOM_COLOR = (0, 255, 0)        # green
    FAR_COLOR = (0, 120, 255)         # blue-ish

    # Close trigger line
    img.draw_line(0, CLOSE_BOTTOM_Y, img_w, CLOSE_BOTTOM_Y, color=CLOSE_LINE_COLOR)
    img.draw_string(2, CLOSE_BOTTOM_Y - 10, "CLOSE Y {}".format(CLOSE_BOTTOM_Y), color=CLOSE_LINE_COLOR)

    # Image bottom reference line
    img.draw_line(0, img_h - 2, img_w, img_h - 2, color=BOTTOM_COLOR)
    img.draw_string(2, img_h - 14, "BOTTOM", color=BOTTOM_COLOR)

    # Distance from cube bottom to image bottom
    img.draw_line(cx, cube_bottom, cx, img_h - 2, color=DIST_COLOR)

    # Cube bottom marker
    img.draw_circle(cx, cube_bottom, 4, color=DIST_COLOR)
    img.draw_cross(cx, cube_bottom, color=DIST_COLOR)

    # Small horizontal marker on cube bottom
    img.draw_line(cx - 10, cube_bottom, cx + 10, cube_bottom, color=DIST_COLOR)

    # Distance label
    mid_y = cube_bottom + (distance_to_bottom // 2)

    label_x = cx + 6
    if label_x > img_w - 95:
        label_x = cx - 90

    img.draw_string(
        label_x,
        max(0, mid_y - 6),
        "dist {}".format(distance_to_bottom),
        color=DIST_COLOR
    )

    status = "CLOSE" if confirmed_close else "FOLLOW"
    raw_status = "RAW CLOSE" if raw_close else "RAW FAR"

    img.draw_string(2, 2, "BOTTOM DIST DEBUG", color=TEXT_COLOR)
    img.draw_string(2, 14, "cube_bottom: {}".format(cube_bottom), color=TEXT_COLOR)
    img.draw_string(2, 26, "dist_bottom: {}".format(distance_to_bottom), color=TEXT_COLOR)
    img.draw_string(2, 38, raw_status, color=CLOSE_LINE_COLOR if raw_close else FAR_COLOR)
    img.draw_string(2, 50, status, color=color)

    img.draw_string(
        cube.x(),
        max(0, cube.y() - 12),
        "bottom {} dist {}".format(cube_bottom, distance_to_bottom),
        color=TEXT_COLOR
    )


# ================================================================
# Surprise Rule - Measure Full Height Without Enlarging Width
# ================================================================

def classify_cube_height(ratio, top_clipped):
    # A cropped top does not prove a cube is short or standard.
    # Its visible height is only a lower bound on its true height.
    if top_clipped:
        return 150 if ratio >= HEIGHT_150_MIN_RATIO else None

    if ratio <= HEIGHT_50_MAX_RATIO and ratio >= 0.40:
        return 50

    if HEIGHT_100_MIN_RATIO <= ratio <= HEIGHT_100_MAX_RATIO:
        return 100

    if HEIGHT_150_MIN_RATIO <= ratio <= 4.50:
        return 150

    return None


def measure_cube_height(img, color_char, cube):
    # Returns: (height, ratio, reliable, reason, full_rectangle)
    # Only the bottom cube ROI chooses which object to measure.
    # All HEIGHT pixels are measured from a NEW full-height strip.
    width = cube.w()
    cube_bottom = cube.y() + cube.h()

    if width < HEIGHT_MIN_WIDTH:
        return None, None, False, "SMALL", None

    # If the cube nearly fills the view, use an existing track only.
    # Its pixel outline is no longer good evidence of physical height.
    if width > int(img_w * 0.52):
        return None, None, False, "TOO CLOSE", None

    if cube.x() <= HEIGHT_EDGE_MARGIN or cube.x() + width >= img_w - HEIGHT_EDGE_MARGIN:
        return None, None, False, "SIDE EDGE", None

    if cube_bottom >= img_h - HEIGHT_EDGE_MARGIN:
        return None, None, False, "BOTTOM EDGE", None

    strip_width = max(10, int(width * HEIGHT_STRIP_WIDTH_FACTOR))
    strip_width = min(strip_width, width)
    strip_x = max(0, min(img_w - strip_width, cube.cx() - strip_width // 2))
    strip_roi = (strip_x, 0, strip_width, img_h)

    # STRICT original color thresholds. The wider height thresholds
    # from v9 also detected a nearby arm in the 150 mm red example.
    threshold = red_threshold if color_char == "R" else green_threshold
    pieces = img.find_blobs(
        threshold,
        roi=strip_roi,
        pixels_threshold=24,
        area_threshold=30,
        merge=False,
        x_stride=1,
        y_stride=1
    )

    if not pieces:
        return None, None, False, "NO STRIP", None

    # Start from the colored component touching the SAME cube's lower
    # ROI. Never choose a large unrelated blob high in the image.
    seed = None
    best_score = -100000
    max_bottom_gap = max(9, int(width * 0.18))

    for piece in pieces:
        piece_bottom = piece.y() + piece.h()
        vertical_overlap = min(piece_bottom, cube_bottom) - max(piece.y(), cube.y())
        bottom_gap = abs(piece_bottom - cube_bottom)

        if bottom_gap > max_bottom_gap or vertical_overlap < 3:
            continue

        score = vertical_overlap * 2 - bottom_gap * 3 + piece.pixels() // 10
        if score > best_score:
            best_score = score
            seed = piece

    if seed is None:
        return None, None, False, "NO BASE", None

    # Grow upward through narrowly separated color fragments.
    # Joining is restricted to the SAME center strip and a short gap.
    top = seed.y()
    used = [seed]
    max_gap = max(4, int(width * HEIGHT_MAX_VERTICAL_GAP))

    for _ in range(5):
        next_piece = None
        next_top = top

        for piece in pieces:
            if piece in used:
                continue
            piece_bottom = piece.y() + piece.h()
            gap = top - piece_bottom
            if -2 <= gap <= max_gap and piece.y() < next_top:
                next_top = piece.y()
                next_piece = piece

        if next_piece is None:
            break

        used.append(next_piece)
        top = next_top

    measured_height = cube_bottom - top
    if measured_height <= 0:
        return None, None, False, "BAD HEIGHT", None

    # Full cube WIDTH comes from the original lower ROI, not the
    # strip width or a whole-frame blob merged with the background.
    ratio = float(measured_height) / float(width)
    full_rect = (cube.x(), top, width, measured_height)
    top_clipped = top <= HEIGHT_EDGE_MARGIN
    height = classify_cube_height(ratio, top_clipped)

    # A nearly empty stripe is not reliable silhouette geometry.
    pixel_count = 0
    for piece in used:
        pixel_count += piece.pixels()
    coverage = float(pixel_count) / float(max(1, strip_width * measured_height))

    if height is None:
        return None, ratio, False, "AMBIGUOUS", full_rect

    if top_clipped:
        # Cropped 150 is only usable if the VISIBLE height itself
        # exceeds the calibrated complete-100 limit by a margin.
        reliable = ratio >= HEIGHT_150_MIN_RATIO + 0.12 and coverage >= HEIGHT_MIN_COVERAGE
        return height, ratio, reliable, "TOP 150", full_rect

    if height == 50:
        far_from_boundary = ratio <= HEIGHT_50_MAX_RATIO - 0.04
    elif height == 100:
        far_from_boundary = (ratio >= HEIGHT_100_MIN_RATIO + 0.04 and
                             ratio <= HEIGHT_100_MAX_RATIO - 0.05)
    else:
        far_from_boundary = ratio >= HEIGHT_150_MIN_RATIO + 0.05

    reliable = bool(far_from_boundary and coverage >= HEIGHT_MIN_COVERAGE)
    return height, ratio, reliable, "FULL" if reliable else "WEAK", full_rect


# ================================================================
# Surprise Rule - Track Each Cube
# ================================================================

def make_cube_track(color_char, cube):
    global next_cube_id

    track = {
        "id": next_cube_id,
        "color": color_char,
        "x": cube.cx(),
        "bottom": cube.y() + cube.h(),
        "width": cube.w(),
        "blob": cube,
        "lost": 0,
        "height": None,
        "candidate": None,
        "candidate_reliable": False,
        "confirm": 0,
        "last_ratio": None,
        "ratio": None,
        "reason": "NEW",
        "full_rect": None,
        "correction": None,
        "correction_count": 0
    }
    next_cube_id += 1
    return track


def cube_track_cost(track, color_char, cube):
    if track["color"] != color_char:
        return None

    width_ratio = float(cube.w()) / float(max(1, track["width"]))
    dx = abs(cube.cx() - track["x"])
    dy = abs(cube.y() + cube.h() - track["bottom"])

    if width_ratio < 0.55 or width_ratio > 1.85:
        return None
    if dx > max(18, min(65, cube.w() * 0.90)):
        return None
    if dy > max(15, min(45, cube.w() * 0.50)):
        return None

    return (dx / float(max(15, cube.w())) +
            dy / float(max(18, cube.w())) +
            abs(width_ratio - 1.0))


def update_cube_height(img, track, color_char, cube):
    # Keep measuring while there is a complete, usable view.
    # Once the sign approaches the close line, freeze the final height.
    # This prevents a near-field crop from changing the passing direction.
    cube_bottom = cube.y() + cube.h()

    if track["height"] is not None and cube_bottom >= CLOSE_BOTTOM_Y:
        track["reason"] = "LOCKED CLOSE"
        track["full_rect"] = None
        return

    height, ratio, reliable, reason, full_rect = measure_cube_height(img, color_char, cube)
    track["ratio"] = ratio if ratio is not None else track["ratio"]
    track["reason"] = reason
    track["full_rect"] = full_rect

    # ============================================================
    # Already locked: prevent a wrong early lock from lasting forever.
    # Only correct with TWO clean, consistent, full-outline readings.
    # No corrections from an ambiguous or top-cropped detection.
    # ============================================================
    if track["height"] is not None:
        if (not reliable or height is None or reason != "FULL"):
            track["correction"] = None
            track["correction_count"] = 0
            track["reason"] = "LOCKED"
            return

        if height == track["height"]:
            track["correction"] = None
            track["correction_count"] = 0
            track["reason"] = "LOCKED"
            return

        if track["correction"] != height:
            track["correction"] = height
            track["correction_count"] = 1
            track["reason"] = "RECHECK 1/2"
            return

        track["correction_count"] += 1
        if track["correction_count"] >= HEIGHT_CORRECTION_FRAMES:
            track["height"] = height
            track["candidate"] = height
            track["candidate_reliable"] = True
            track["confirm"] = HEIGHT_CONFIRM_FRAMES
            track["last_ratio"] = ratio
            track["correction"] = None
            track["correction_count"] = 0
            track["reason"] = "RELOCKED"
        else:
            track["reason"] = "RECHECK"
        return

    # ============================================================
    # Not yet locked: first full measurement = immediate candidate.
    # Weak measurements cannot count toward height confirmation.
    # ============================================================
    if height is None:
        if track["candidate"] is not None:
            track["reason"] = "HOLD " + reason
        return

    if not reliable:
        if track["candidate"] is None:
            track["candidate"] = height
            track["candidate_reliable"] = False
            track["confirm"] = 0
        track["reason"] = "WEAK"
        return

    if track["candidate"] != height or not track["candidate_reliable"]:
        track["candidate"] = height
        track["candidate_reliable"] = True
        track["confirm"] = 1
        track["last_ratio"] = ratio
        track["reason"] = "FIRST"
        return

    if track["last_ratio"] is not None and abs(ratio - track["last_ratio"]) > HEIGHT_MAX_RATIO_CHANGE:
        track["confirm"] = 1
        track["last_ratio"] = ratio
        track["reason"] = "UNSTABLE"
        return

    track["confirm"] += 1
    track["last_ratio"] = ratio
    if track["confirm"] >= HEIGHT_CONFIRM_FRAMES:
        track["height"] = height
        track["reason"] = "LOCKED"
    else:
        track["reason"] = "CHECK"


def update_cube_tracks(img, observations):
    global cube_tracks

    for track in cube_tracks:
        track["blob"] = None

    candidates = []
    for i, track in enumerate(cube_tracks):
        for j, item in enumerate(observations):
            color_char, cube = item
            cost = cube_track_cost(track, color_char, cube)
            if cost is not None:
                candidates.append((cost, i, j))

    candidates.sort()
    used_tracks = set()
    used_cubes = set()
    paired = []

    for cost, i, j in candidates:
        if i in used_tracks or j in used_cubes:
            continue
        used_tracks.add(i)
        used_cubes.add(j)
        track = cube_tracks[i]
        color_char, cube = observations[j]
        track["x"] = cube.cx()
        track["bottom"] = cube.y() + cube.h()
        track["width"] = cube.w()
        track["blob"] = cube
        track["lost"] = 0
        update_cube_height(img, track, color_char, cube)
        paired.append((color_char, cube, track))

    for i, track in enumerate(cube_tracks):
        if i not in used_tracks:
            track["lost"] += 1

    cube_tracks = [track for track in cube_tracks if track["lost"] < TRACK_LOST_FRAMES]

    for j, item in enumerate(observations):
        if j in used_cubes or len(cube_tracks) >= TRACK_MAX_COUNT:
            continue
        color_char, cube = item
        track = make_cube_track(color_char, cube)
        update_cube_height(img, track, color_char, cube)
        cube_tracks.append(track)
        paired.append((color_char, cube, track))

    return paired


def get_cube_height(track):
    if track is None:
        return None
    if track["height"] is not None:
        return track["height"]
    if track["candidate_reliable"]:
        return track["candidate"]
    return None


def get_passing_color(color_char, track):
    if not SURPRISE_RULE:
        return color_char

    height = get_cube_height(track)
    if height == 50 or height == 150:
        return "G" if color_char == "R" else "R"
    if height == 100:
        return color_char

    return "?" if UNKNOWN_CUBE_ACTION == "UNKNOWN" else color_char


def draw_cube_height_debug(img, color_char, track, passing_color):
    if track is None:
        height = None
        candidate = None
        ratio = None
        reason = "NO TRACK"
        cube_id = 0
        confirm = 0
    else:
        height = track["height"]
        candidate = track["candidate"]
        ratio = track["ratio"]
        reason = track["reason"]
        cube_id = track["id"]
        confirm = track["confirm"]

    height_label = str(height) if height else (str(candidate) + "?" if candidate else "UNKNOWN")
    visible_label = "RED" if color_char == "R" else "GREEN"
    passing_label = "RED" if passing_color == "R" else "GREEN" if passing_color == "G" else "UNKNOWN"
    if get_cube_height(track) is None:
        passing_label += " DEFAULT" if passing_color != "?" else ""

    text_color = (255, 255, 0)
    img.draw_string(2, 62, "HEIGHT: {}".format(height_label), color=text_color)
    img.draw_string(2, 74, "VISIBLE: {}".format(visible_label), color=text_color)
    img.draw_string(2, 86, "PASS: {}".format(passing_label), color=text_color)
    img.draw_string(2, 98, "H/W: {}".format("{:.2f}".format(ratio) if ratio is not None else "-"), color=text_color)
    img.draw_string(2, 110, "CONFIRM: {}/{}".format(min(confirm, HEIGHT_CONFIRM_FRAMES), HEIGHT_CONFIRM_FRAMES), color=text_color)
    img.draw_string(2, 122, "SHAPE: {}".format(reason), color=text_color)
    img.draw_string(2, 134, "CUBE ID: {}".format(cube_id), color=text_color)


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

    # ============================================================
    # Surprise Rule - Measure and Track Cube Height
    # ============================================================

    observations = [("R", b) for b in red_blobs] + [("G", b) for b in green_blobs]
    paired = update_cube_tracks(img, observations)

    # ============================================================
    # Cube PD Following
    # UART commands:
    # S+0.123 while following
    # RED / GREEN when close
    # ============================================================

    color_char, cube = choose_best_cube(red_blobs, green_blobs)

    if cube:
        if color_char == "R":
            draw_color = (255, 0, 0)
            label = "RED"
        else:
            draw_color = (0, 255, 0)
            label = "GREEN"

        # Find this exact ROI blob's track. Different cubes can have
        # different height classifications even if they share a color.
        cube_track = None
        for tracked_color, tracked_blob, track in paired:
            if tracked_blob is cube and tracked_color == color_char:
                cube_track = track
                break

        passing_color = get_passing_color(color_char, cube_track)

        # Steering uses center X only.
        error = (cube.cx() - target_x) / float(target_x)

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

        if not confirmed_close:
            send("S{:+.3f}\n".format(pid_output))

            if DEBUG and DEBUG_PRINT:
                print(
                    "{} FOLLOW err:{:+.3f} pid:{:+.3f} bottom:{} dist_to_bottom:{} close_count:{}".format(
                        label, error, pid_output, cube_bottom, distance_to_bottom, close_counter
                    )
                )

        else:
            if passing_color == "R":
                send("RED\n")
            elif passing_color == "G":
                send("GREEN\n")
            else:
                send("UNKNOWN\n")

            if DEBUG and DEBUG_PRINT:
                print(
                    "{} CLOSE pass:{} height:{} bottom:{} dist_to_bottom:{}".format(
                        label, passing_color, get_cube_height(cube_track), cube_bottom, distance_to_bottom
                    )
                )

        # ========================================================
        # Debug Draw - AFTER All Blob Detection
        # ========================================================

        if DEBUG and DEBUG_ROI:
            img.draw_rectangle(cubes_roi, color=(0, 255, 0))
            img.draw_string(cubes_roi[0] + 3, cubes_roi[1] + 3, "CUBES ROI", color=(0, 255, 0))

        if DEBUG and DEBUG_RAW:
            for b in red_raw:
                img.draw_rectangle(b.rect(), color=(120, 0, 0))
            for b in green_raw:
                img.draw_rectangle(b.rect(), color=(0, 120, 0))

        if DEBUG and DEBUG_HEIGHT_SHAPE:
            for track in cube_tracks:
                if track["blob"] is not None and track["full_rect"] is not None:
                    shape_color = (0, 255, 255) if track["height"] else (255, 255, 0)
                    img.draw_rectangle(track["full_rect"], color=shape_color)

        if DEBUG and DEBUG_ACCEPTED:
            draw_label(img, cube, draw_color, label)

        if DEBUG and DEBUG_HEIGHT:
            draw_cube_height_debug(img, color_char, cube_track, passing_color)

        if DEBUG and DEBUG_CUBE_DISTANCE:
            draw_cube_distance_debug(
                img, cube, draw_color, cube_bottom, distance_to_bottom,
                raw_close, confirmed_close
            )

    else:
        # Prevent steering jump when cube appears again.
        pid_last_error *= 0.75
        close_counter = 0

    # ============================================================
    # General Debug Print
    # ============================================================

    if DEBUG and DEBUG_PRINT:
        print(
            "FPS:", clock.fps(),
            "red:", len(red_blobs),
            "green:", len(green_blobs),
            "cube:", color_char if cube else "NONE"
        )
