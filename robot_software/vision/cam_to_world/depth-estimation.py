from imutils import paths
import imutils
import numpy as np
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


def find_marker(image):
    # convert the image to grayscale, blur it, and detect edges
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(gray, 35, 125)

    # find the contours in the edged image and keep the largest one;
    # we'll assume that this is our piece of paper in the image
    cnts = cv2.findContours(edged.copy(), cv2.RETR_LIST,
                            cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    c = max(cnts, key=cv2.contourArea)

    # compute the bounding box of the of the paper region and return it
    print(cv2.minAreaRect(c))
    return cv2.minAreaRect(c)


def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


# load the first image that contains an object that is KNOWN TO BE 25 CM from our camera
# then find the paper marker in the image, and initialize the focal length
image = cv2.imread("test-25.jpg")
marker = find_marker(image)
focalLength = (marker[1][0] * KNOWN_DISTANCE) / KNOWN_WIDTH

print(f"Focal Length: {focalLength}")

images = ["test-25.jpg"]


# loop over the images
def verify_distance():
    try:
        for imagePath in sorted(images):
            # load the image, find the marker in the image, then compute the
            # distance to the marker from the camera
            image = cv2.imread(imagePath)
            marker = find_marker(image)
            centimeters = distance_to_camera(
                KNOWN_WIDTH, focalLength, marker[1][0])11:54 AM
            # draw a bounding box around the image and display it
            box = cv2.cv.BoxPoints(
                marker) if imutils.is_cv2() else cv2.boxPoints(marker)
            box = np.int0(box)
            cv2.drawContours(image, [box], -1, (0, 255, 0), 2)
            cv2.putText(image, f"{round(centimeters, 2)}",
                        (image.shape[1] - 200, image.shape[0] -
                         20), cv2.FONT_HERSHEY_SIMPLEX,
                        2.0, (0, 255, 0), 3)
            cv2.imshow("image", image)
            cv2.waitKey(0)
    finally:
        cap.release()
        cv2.destroyAllWindows()


def capture_test_images() -> None:
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
            
            marker = find_marker(flip_frame)
            centimeters = distance_to_camera(
                KNOWN_WIDTH, focalLength, marker[1][0])
            # draw a bounding box around the image and display it
            box = cv2.cv.BoxPoints(
                marker) if imutils.is_cv2() else cv2.boxPoints(marker)
            box = np.int0(box)
            cv2.drawContours(flip_frame, [box], -1, (0, 255, 0), 2)
            cv2.putText(flip_frame, f"{round(centimeters, 2)}",
                        (flip_frame.shape[1] - 200, flip_frame.shape[0] -
                         20), cv2.FONT_HERSHEY_SIMPLEX,
                        2.0, (0, 255, 0), 3)
            
            
            cv2.imshow(live_window, flip_frame)

            if cv2.waitKey(1) & 0xFF == ord('l'):
                cv2.imwrite("test-26.jpg", flip_frame)

            if cv2.waitKey(1) == ord('q'):
                break
    finally:
        print("quitting")
        cap.release()
        cv2.destroyAllWindows()


capture_test_images()

# while True:
#     ret, frame = cap.read()

#     flip_frame = cv2.flip(frame, -1)  # flip both axes

#     live_window = 'Live Feed'
#     # Re-position Window
#     cv2.namedWindow(live_window)
#     cv2.moveWindow(live_window, 0, 0)
#     cv2.imshow(live_window, flip_frame)

#     if cv2.waitKey(1) & 0xFF == ord('l'):
#         cv2.imwrite("test-25.jpg", flip_frame)

#     if cv2.waitKey(1) == ord('q'):
#         break

# cap.release()
# cv2.destroyAllWindows()
