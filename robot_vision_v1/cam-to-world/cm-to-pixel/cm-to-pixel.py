import cv2
import math
from datetime import datetime


# Setup Camera
cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L)
# Set Dimensions
FRAME_WIDTH, FRAME_HEIGHT = 640, 640
cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)


X_AXIS_CM_TO_PIXEL = 0.03625
Y_AXIS_CM_TO_PIXEL = 0.03168

X_AXIS_MIDPOINT_CAMERA = (FRAME_WIDTH // 2) * X_AXIS_CM_TO_PIXEL
# Similar as the camera is mounted at the center of robot arm i.e. origin of arm's coordinate system
X_AXIS_MIDPOINT_WORLD = X_AXIS_MIDPOINT_CAMERA

coordinate_list = [
    {
        "x1": 153.46231079101562,
        "y1": 482.2259521484375,
        "x2": 273.2603759765625,
        "y2": 581.88916015625
    },
    {
        "x1": 336.2560119628906,
        "y1": 432.6934814453125,
        "x2": 468.0475158691406,
        "y2": 575.989013671875
    }
]


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


def get_world_coordinates(cameraCoordinates: list):
    world_coordinates = []

    for coordinate in cameraCoordinates:

        x = float(f"{(coordinate[0] * X_AXIS_CM_TO_PIXEL):0,.3f}")
        y = float(f"{coordinate[1] * Y_AXIS_CM_TO_PIXEL:0,.3f}")
        world_coordinates.append((x, y))

    return world_coordinates


def resolve_world_coordinates(worldCoordinates: list):
    world_coordinates = []

    for coordinate in worldCoordinates:

        # distance_from_midpoint = X_AXIS_MIDPOINT_CAMERA - world_coordinate
        # y_distance = z_distance = frame_height_cm - object_height_cm

        x = int(X_AXIS_MIDPOINT_CAMERA - coordinate[0])
        y = int((FRAME_HEIGHT * Y_AXIS_CM_TO_PIXEL) - coordinate[1])
        world_coordinates.append((x, y))

    return world_coordinates


def get_world_coordinates_3d(worldCoordinates: list):
    world_coordinates = []
    depth = 0

    for coordinate in worldCoordinates:

        # distance_from_midpoint = X_AXIS_MIDPOINT_CAMERA - world_coordinate

        x = depth
        y = coordinate[0]
        z = coordinate[1]
        world_coordinates.append((x, y, z))

    return world_coordinates


def take_snapshot():

    print("Press 'l' to take a snapshot")
    while cap.isOpened():

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

        if cv2.waitKey(1) & 0xFF == ord('l'):
            now = datetime.now()
            print("captured")
            cv2.imwrite(f'snapshot - {now}.jpg', flipped_frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


# take_snapshot()

centroids = get_centroids(coordinate_list)
print(f"centroids: {centroids}")

world_coordinates = get_world_coordinates(centroids)
print(f"world_coordinates: {world_coordinates}")

resolved_world_coordinates = resolve_world_coordinates(world_coordinates)
print(f"resolved_world_coordinates: {resolved_world_coordinates}")

world_coordinates_3d = get_world_coordinates_3d(resolved_world_coordinates)
print(f"3D_world_coordinates: {world_coordinates_3d}")

# validation_image = cv2.imread('validation-image.jpg')
# cv2.imshow('validation_image', validation_image)
# cv2.waitKey(0)
