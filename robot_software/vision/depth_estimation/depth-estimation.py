from ultralytics.yolo.utils.plotting import Annotator
from ultralytics import YOLO
from imutils import paths
import imutils
import numpy as np
import json
import cv2

# Initialize Camera
# cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L)
cap = cv2.VideoCapture(0)
FRAME_WIDTH, FRAME_HEIGHT = 640, 640
cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)

# Initialize the known distance from the camera to the object -> 26 CM
KNOWN_DISTANCE = 26

# Initialize the known object width
# Box -> 20.2 CM
# Styrofoam -> 20.6 CM
# Trailer -> 10.2 CM
KNOWN_WIDTH = 10.2

model = YOLO('../model/w_best.pt')


def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


# load the first image that contains an object that is KNOWN TO BE 25 CM from our camera
# then find the paper marker in the image, and initialize the focal length
# image = cv2.imread("test-25.jpg")
# marker = find_marker(image)
# focalLength = (marker[1][0] * KNOWN_DISTANCE) / KNOWN_WIDTH

# print(f"Focal Length: {focalLength}")
focalLength = 708.3333

img_count = 13
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Frame not captured")
            break

        flip_frame = cv2.flip(frame, -1)  # flip both axes

        live_window = 'Live Feed'
        # Re-position Window
        cv2.namedWindow(live_window)
        cv2.moveWindow(live_window, 0, 0)

        result = model(flip_frame)[0]

        annotator = Annotator(flip_frame)
        boxes = result.boxes

        for box in boxes:

            # get box coordinates in (top, left, bottom, right) format
            b = box.xyxy[0]
            c = box.cls
            annotator.box_label(b, model.names[int(c)])

        inference_frame = annotator.result()
        inference_win = 'YOLO V8 Inference Result'

        inference_result_json = result.tojson()
        inference_result_list = json.loads(inference_result_json)

        filtered_json_list = []
        for object in inference_result_list:
            filtered_json_list.append(object["box"])

        x_dict = filtered_json_list[0]

        measured_width = int(x_dict["x2"]) - int(x_dict["x1"])
        print(f"Measured Width: {measured_width} px")
        print("\n----\n")

        centimeters = distance_to_camera(
            KNOWN_WIDTH, focalLength, measured_width)

        cv2.putText(flip_frame, f"{round(centimeters, 2)} cm",
                    (flip_frame.shape[1] - 200, flip_frame.shape[0] -
                        20), cv2.FONT_HERSHEY_SIMPLEX,
                    2.0, (0, 255, 0), 3)

        cv2.imshow(live_window, flip_frame)

        if cv2.waitKey(1) & 0xFF == ord('l'):
            cv2.imwrite(f"results/result-{img_count}.jpg", flip_frame)
            img_count += 1

        if cv2.waitKey(1) == ord('q'):
            break
finally:
    print("quitting")
    cap.release()
    cv2.destroyAllWindows()
