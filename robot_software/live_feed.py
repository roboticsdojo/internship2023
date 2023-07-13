from vision.cam_to_world.cam_to_world import get_world_cooridinates_final
from vision.model_inference import infer
import RPi.GPIO as GPIO
from datetime import datetime
import cv2

# Setup Camera
cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L)
# Set Dimensions
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 640)


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


def camera_inference():
    try:
        while cap.isOpened():
            now = datetime.now()

            ret, frame = cap.read()
            flipped_frame = cv2.flip(frame, -1)  # flip both axes
            if not ret:
                print("Failed to Read Camera Frame")
                break

            # Show Feed
            live_window = 'Live Feed'
            # Re-position Window
            cv2.namedWindow(live_window)
            cv2.moveWindow(live_window, 0, 0)
            cv2.imshow(live_window, flipped_frame)
            # cv2.line(img=flipped_frame, pt1=(320, 0), pt2=(320, 640),
            #          color=(0, 255, 255), thickness=2, lineType=8, shift=0)

            if cv2.waitKey(1) & 0xFF == ord('l'):
                # Infer on snapshot
                print(f"[{now}]> Infer on snapshot")
                inference_result = infer(flipped_frame)
                print(inference_result)

                centroids = get_centroids(inference_result)
                print(f"Centroids: {centroids}")

                world_coordinates = get_world_cooridinates_final(centroids)
                print(f"World Coordinates: {world_coordinates}")

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    finally:
        cap.release()
        cv2.destroyAllWindows()


camera_inference()
