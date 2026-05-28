import cv2
import mediapipe as mp
import serial
import time
import sys
from flask import Flask, Response

# ---------------- SERIAL ---------------- #
SERIAL_PORT = '/dev/ttyUSB0'
try:
    arduino = serial.Serial(SERIAL_PORT, 9600, timeout=1, write_timeout=2)
    time.sleep(2)
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()
except Exception:
    try:
        SERIAL_PORT = '/dev/ttyACM0'
        arduino = serial.Serial(SERIAL_PORT, 9600, timeout=1, write_timeout=2)
        time.sleep(2)
    except Exception as e:
        print(f"Could not open serial connection: {e}")
        sys.exit(1)

# ---------------- CAMERA ---------------- #
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
cap.set(cv2.CAP_PROP_FPS, 15)

# ---------------- FLASK ---------------- #
app = Flask(__name__)

# ---------------- MEDIAPIPE (LIGHT MODE) ---------------- #
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils

hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,
    model_complexity=0,
    min_detection_confidence=0.6,
    min_tracking_confidence=0.5
)

# ---------------- CONTROL VARIABLES ---------------- #
last_cmd = None
stable_cmd = None
cmd_start_time = 0
HOLD_TIME = 0.4

frame_skip = 2
frame_count = 0

# ---------------- GESTURE LOGIC ---------------- #
def fingers_up(lm):
    tips = [4, 8, 12, 16, 20]
    f = []
    # Thumb check (Handles natural mirroring)
    f.append(1 if lm.landmark[4].x < lm.landmark[3].x else 0)
    # Remaining fingers check
    for i in range(1, 5):
        f.append(1 if lm.landmark[tips[i]].y < lm.landmark[tips[i]-2].y else 0)
    return f

def gesture(f):
    # Fixed explicitly defined matching states
    if f == [0, 0, 0, 0, 0]: return "GRIP_CLOSE"  # Fist
    if f == [1, 1, 1, 1, 1]: return "GRIP_OPEN"   # Open Hand
    if f == [0, 1, 0, 0, 0]: return "LEFT"        # Index finger up
    if f == [0, 1, 1, 0, 0]: return "RIGHT"       # Index + Middle finger up
    return None

# ---------------- STREAM ---------------- #
def gen_frames():
    global last_cmd, stable_cmd, cmd_start_time, frame_count

    while True:
        time.sleep(0.03)

        success, frame = cap.read()
        if not success:
            break

        frame_count += 1
        if frame_count % frame_skip != 0:
            continue

        frame = cv2.flip(frame, 1)
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        result = hands.process(rgb)
        current_cmd = None

        if result.multi_hand_landmarks:
            for hand in result.multi_hand_landmarks:
                mp_draw.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)
                f = fingers_up(hand)
                current_cmd = gesture(f)

                if current_cmd:
                    cv2.putText(frame, current_cmd, (20, 40),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)

        # ---------------- STABILITY FILTER ---------------- #
        now = time.time()
        if current_cmd == stable_cmd:
            if now - cmd_start_time >= HOLD_TIME:
                if current_cmd:
                    print("Sending:", current_cmd)
                    try:
                        arduino.write((current_cmd + "\n").encode())
                    except Exception as e:
                        print(f"Write error: {e}")
                    last_cmd = current_cmd
                    cmd_start_time = now - 0.2
        else:
            stable_cmd = current_cmd
            cmd_start_time = now

        # ---------------- STREAM OUTPUT ---------------- #
        _, buffer = cv2.imencode('.jpg', frame, [int(cv2.IMWRITE_JPEG_QUALITY), 40])
        frame_bytes = buffer.tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

@app.route('/')
def index():
    return """
    <html>
        <head><title>Adeept 2-Axis Controller</title></head>
        <body style="background:#111; color:#fff; text-align:center; font-family:sans-serif;">
            <h2>Adeept Arm Control (Base & Gripper Only)</h2>
            <img src="/video_feed" style="border:2px solid #333; border-radius:4px;">
            <p style="font-size:1.1em; color:#bbb;">
                <strong>Fist:</strong> Close Gripper |
                <strong>All Open:</strong> Open Gripper |
                <strong>Index Up:</strong> Turn Left |
                <strong>Index+Middle Up:</strong> Turn Right
            </p>
        </body>
    </html>
    """

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8080, debug=False)
