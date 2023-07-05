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
Y_AXIS_SCALE_FACTOR = 1.21

X_AXIS_MIDPOINT_CAMERA = (FRAME_WIDTH // 2) * X_AXIS_CM_TO_PIXEL
Y_AXIS_MIDPOINT_CAMERA = (FRAME_HEIGHT // 2) * Y_AXIS_CM_TO_PIXEL

# Similar as the camera is mounted at the center of robot arm i.e. origin of arm's coordinate system
# This is only true for world Y and Z axis
Y_AXIS_MIDPOINT_WORLD = X_AXIS_MIDPOINT_CAMERA
Z_AXIS_MIDPOINT_WORLD = Y_AXIS_MIDPOINT_CAMERA


# validation-images
coordinate_list_0 = [
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

coordinate_list_1 = [
    {
        "x1": 101.38385009765625,
        "y1": 491.5885009765625,
        "x2": 220.65567016601562,
        "y2": 594.26318359375
    },
    {
        "x1": 238.24143981933594,
        "y1": 481.2621765136719,
        "x2": 357.203125,
        "y2": 589.399658203125
    },
    {
        "x1": 386.10809326171875,
        "y1": 472.27685546875,
        "x2": 522.5375366210938,
        "y2": 596.74267578125
    }
]

coordinate_list_2 = [
    {
        "x1": 97.16504669189453,
        "y1": 487.7280578613281,
        "x2": 205.7490234375,
        "y2": 591.25341796875
    },
    {
        "x1": 250.60635375976562,
        "y1": 344.41949462890625,
        "x2": 376.4057312011719,
        "y2": 442.16717529296875
    },
    {
        "x1": 392.09490966796875,
        "y1": 474.839599609375,
        "x2": 539.689208984375,
        "y2": 604.5635986328125
    }

]

coordinate_list_3 = [
    {
        "x1": 97.1309585571289,
        "y1": 488.7786865234375,
        "x2": 207.25466918945312,
        "y2": 590.373046875
    },
    {
        "x1": 239.624755859375,
        "y1": 61.00007629394531,
        "x2": 378.1429443359375,
        "y2": 153.8924560546875
    },
    {
        "x1": 398.8424072265625,
        "y1": 472.7543029785156,
        "x2": 535.5258178710938,
        "y2": 598.8170166015625
    }

]

coordinate_list_4 = [
    {
        "x1": 221.8655242919922,
        "y1": 524.8685913085938,
        "x2": 361.63189697265625,
        "y2": 639.1655883789062
    },
    {
        "x1": 221.26693725585938,
        "y1": 525.0765380859375,
        "x2": 362.2241516113281,
        "y2": 639.8843994140625
    },
    {
        "x1": 390.501708984375,
        "y1": 472.94232177734375,
        "x2": 540.3988647460938,
        "y2": 602.2704467773438
    }
]

coordinate_list_5 = [
    {
        "x1": 54.059356689453125,
        "y1": 490.96197509765625,
        "x2": 196.0463409423828,
        "y2": 623.2000122070312
    },
    {
        "x1": 398.25164794921875,
        "y1": 434.69219970703125,
        "x2": 535.6445922851562,
        "y2": 551.322021484375
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
        # Use scaling factor to correct Y axis conversion.
        y = float(
            f"{(coordinate[1] * Y_AXIS_CM_TO_PIXEL * Y_AXIS_SCALE_FACTOR):0,.3f}")
        world_coordinates.append((x, y))

    return world_coordinates


def resolve_world_coordinates(worldCoordinates: list):
    world_coordinates = []

    for coordinate in worldCoordinates:

        # distance_from_midpoint = X_AXIS_MIDPOINT_CAMERA - world_coordinate
        # method-1 (Simple Implementation): y_distance = z_distance = frame_height_cm - object_height_cm
        # method-2 (use camera mid-y as z-axis center): y_distance = Y_AXIS_MIDPOINT_CAMERA - world_coordinate

        x = int(X_AXIS_MIDPOINT_CAMERA - coordinate[0])
        # y = int((FRAME_HEIGHT * Y_AXIS_CM_TO_PIXEL) - coordinate[1])
        # y = int(Y_AXIS_MIDPOINT_CAMERA - coordinate[1])
        y = int((FRAME_HEIGHT * Y_AXIS_CM_TO_PIXEL) - coordinate[1])

        #! validation-img-5 failed: Lack of depth-information results in negative values!
        if y < 0:
            y = 0

        world_coordinates.append((x, y))

    return world_coordinates


def get_world_coordinates_3d(worldCoordinates: list):
    world_coordinates = []
    depth = 0

    for coordinate in worldCoordinates:

        x = coordinate[0]
        y = depth
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


take_snapshot()


print(f"\nX_AXIS_CM_TO_PIXEL: {X_AXIS_CM_TO_PIXEL}")
print(f"Y_AXIS_CM_TO_PIXEL: {Y_AXIS_CM_TO_PIXEL}")
print(
    f"FRAME_WIDTH: {FRAME_WIDTH} -> {int(FRAME_WIDTH * X_AXIS_CM_TO_PIXEL)} cm")
print(
    f"FRAME_HEIGHT: {FRAME_HEIGHT} -> {int(FRAME_HEIGHT * Y_AXIS_CM_TO_PIXEL)} cm")
print(f"X_AXIS_MIDPOINT_CAMERA: {int(X_AXIS_MIDPOINT_CAMERA)}")
print(f"Y_AXIS_MIDPOINT_CAMERA: {int(Y_AXIS_MIDPOINT_CAMERA)}")

print("\n------\n")

centroids = get_centroids(coordinate_list_5)
print(f"centroids: {centroids}")

world_coordinates = get_world_coordinates(centroids)
print(f"world_coordinates: {world_coordinates}")

resolved_world_coordinates = resolve_world_coordinates(world_coordinates)
print(f"resolved_world_coordinates: {resolved_world_coordinates}")

world_coordinates_3d = get_world_coordinates_3d(resolved_world_coordinates)
print(f"3D_world_coordinates: {world_coordinates_3d}")


print("\n")
validation_image = cv2.imread('./validation-images/validation-img-5.jpg')
cv2.imshow('validation_image', validation_image)
cv2.waitKey(0)
