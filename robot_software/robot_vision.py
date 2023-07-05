from ultralytics.yolo.utils.plotting import Annotator
from model_inference import infer, model
import RPi.GPIO as GPIO
from datetime import datetime
import time
import cv2
import json

# Setup Camera
cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L)
# Set Dimensions
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 640)


# Setup GPIO
pick_pin = 23
place_pin = 24
go_pin = 25

GPIO.setmode(GPIO.BCM)
GPIO.setup(pick_pin, GPIO.IN)
GPIO.setup(place_pin, GPIO.IN)
GPIO.setup(go_pin, GPIO.OUT)


mobile_platform_event = 0


# ---------- Camera Related Functions ----------
def camera_inference():
    x, y, z = 10, 20, 30

    return (x, y, z)


def get_centroids(coordinates: list):
    centroids = []

    for coordinate in coordinates:
        x1 = int(coordinate['x1'])
        x2 = int(coordinate['x2'])
        y1 = int(coordinate['y1'])
        y2 = int(coordinate['y2'])

        x = (x1 + x2) // 2
        y = (y1 + y2) // 2
        centroids.append((x, y))

    return centroids


def video_snap_infer():
    while cap.isOpened():
        now = datetime.now()

        ret, frame = cap.read()
        flipped_frame = cv2.flip(frame, -1)  # flip both axes
        if not ret:
            print("Failed to Read Camera Frame")
            break

        # cv2.imwrite('infer_from_snapshot.jpg', frame)

        # Get FPS
        FPS = cap.get(cv2.CAP_PROP_FPS)

        # Show Feed
        live_window = 'Live Feed'
        # Re-position Window
        cv2.namedWindow(live_window)
        cv2.moveWindow(live_window, 0, 0)
        cv2.imshow(live_window, flipped_frame)

        if cv2.waitKey(1) & 0xFF == ord('l'):
            # Infer on snapshot
            print(f"[{now}]> Infer on snapshot")
            inference_result = infer(flipped_frame)
            print(inference_result)

            centroids = get_centroids(inference_result)
            print(centroids)

            # Visualize Result
            '''
            cv2.rectangle(img, (x1, y1), (x2, y2), (255,0,0), 2)


            x1,y1 ------
            |          |
            |          |
            |          |
            --------x2,y2
            
            image = cv2.imread('testimage.jpg')
            height, width, channels = image.shape
            start_point = (0,0)
            end_point = (width, height)
            color = (0,0,255)
            thickness = 5

            image = cv2.rectangle(image, start_point, end_point, color, thickness)
            cv2.imshow('Rectangle',image)
            '''

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# # https://stackoverflow.com/questions/56115874/how-to-convert-bounding-box-x1-y1-x2-y2-to-yolo-style-x-y-w-h
# def yolobbox2bbox(x, y, w, h):
#     x1, y1 = x-w/2, y-h/2
#     x2, y2 = x+w/2, y+h/2

#     return x1, y1, x2, y2


video_snap_infer()

# * Super Loop
# while True:

#     # * Mobile-Platform - Pi Communication Simulator
#     # 1. Pi Polls GPIO for event from Mobile-Platform
#     # 2. If GPIO.23 is high, action = Pick
#     # 3. If GPIO.24 is high, action = Place

#     # 1. Pi Polls GPIO for event from Mobile-Platform
#     pick_event = GPIO.input(pick_pin)
#     place_event = GPIO.input(place_pin)

#     # 2. If GPIO.23 is high, action = Pick
#     if pick_event:
#         # mobile_platform_event = arm_comms_simulator(1, 0)
#         pass

#     # 3. If GPIO.24 is high, action = Place
#     elif place_event:
#         # mobile_platform_event = arm_comms_simulator(0, 1)
#         pass

#     time.sleep(5)
#     mobile_platform_event = 1

#     if mobile_platform_event:
#         print("Mobile Platform Go")
#         GPIO.output(go_pin, True)
#         place_event = 0
#         pick_event = 0

#         # 8. Mobile-Platform pulls GPIO.Pick or GPIO.Place low
#         # ? give mobile platform time to pull GPIO low
#         time.sleep(1)
#         pick_event = GPIO.input(pick_pin)

#         # 9. Pi pulls GPIO.Go low (for next cycle)
#         if not pick_event:
#             GPIO.output(go_pin, False)
#             mobile_platform_event = 0
#             print("Event Cycle Complete")
#             print("---------------------------------------\n")
