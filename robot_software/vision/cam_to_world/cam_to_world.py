# Current Algorithm: cm to pixel factor

# Camera Resolution
FRAME_WIDTH, FRAME_HEIGHT = 640, 640


X_AXIS_CM_TO_PIXEL = 0.03625
Y_AXIS_CM_TO_PIXEL = 0.03168
Y_AXIS_SCALE_FACTOR = 1.21

X_AXIS_MIDPOINT_CAMERA = (FRAME_WIDTH // 2) * X_AXIS_CM_TO_PIXEL
Y_AXIS_MIDPOINT_CAMERA = (FRAME_HEIGHT // 2) * Y_AXIS_CM_TO_PIXEL

# Similar as the camera is mounted at the center of robot arm i.e. origin of arm's coordinate system
# This is only true for world Y and Z axis
Y_AXIS_MIDPOINT_WORLD = X_AXIS_MIDPOINT_CAMERA
Z_AXIS_MIDPOINT_WORLD = Y_AXIS_MIDPOINT_CAMERA


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
    depth = 20

    for coordinate in worldCoordinates:

        # convert to mm by multiplying with 10
        x = coordinate[0] * 10
        y = depth * 10
        z = coordinate[1] * 10
        world_coordinates.append((x, y, z))

    return world_coordinates


def get_world_cooridinates_final(centroids: list):
    world_coordinates = get_world_coordinates(centroids)
    print(f"camera_world_coordinates: {world_coordinates}")

    resolved_world_coordinates = resolve_world_coordinates(world_coordinates)
    print(f"resolved_camera_world_coordinates: {resolved_world_coordinates}")

    world_coordinates_3d = get_world_coordinates_3d(resolved_world_coordinates)
    print(f"3D_world_coordinates: {world_coordinates_3d}")

    return world_coordinates_3d
