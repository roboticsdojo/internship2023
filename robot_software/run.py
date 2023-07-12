from vision.cam_to_world.cam_to_world import get_world_cooridinates_final
from vision.model_inference import infer
from datetime import date
from pathlib import Path
import RPi.GPIO as GPIO
import logging
import serial
import time
import cv2


# TODO: Remove intentional block used during development

print("\n------------\nInitializing Raspberry Pi...")

# Create logging directory
date_today = date.today()
time_now = time.strftime("%H:%M:%S", time.localtime())
print(f"Date: {date_today}, Time: {time_now}")
log_path = f"./vision/logs/{date_today}/runs"
img_path = f"./vision/logs/{date_today}/images"
label_path = f"./vision/logs/{date_today}/labels"
Path(log_path).mkdir(parents=True, exist_ok=True)
Path(img_path).mkdir(parents=True, exist_ok=True)
Path(label_path).mkdir(parents=True, exist_ok=True)


# Initialize Logger
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
# create handler
log_handler = logging.FileHandler(f"{log_path}/{time_now}.log", "a")
# log_handler.setLevel(logging.DEBUG)
# create formatter
log_format = logging.Formatter(
    "%(asctime)s - %(name)s - %(levelname)s - %(message)s")
log_handler.setFormatter(log_format)
# Add handler to logger
logger.addHandler(log_handler)
logger.info("Logger Initialized Successfully")
print("Logger Initialized Successfully")


# Initialize Camera
cap = cv2.VideoCapture('/dev/video0', cv2.CAP_V4L)
FRAME_WIDTH, FRAME_HEIGHT = 640, 640
cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)
logger.info("Camera Initialized Successfully")
print("Camera Initialized Successfully")

# Setup Serial Communication
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.reset_input_buffer()
logger.info("Serial Initialized Successfully")
print("Serial Initialized Successfully")

# Setup GPIO
pick_pin = 23
place_pin = 24
go_pin = 25

mobile_platform_event = 0

GPIO.setmode(GPIO.BCM)
GPIO.setup(go_pin, GPIO.OUT)
# Avoid floating state by attaching input pins to internal pulldown resistors
GPIO.setup(pick_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(place_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
logger.info("GPIO Initialized Successfully")
print("GPIO Initialized Successfully")
logger.info("Raspberry Pi Ready")
print("Raspberry Pi Ready\n------------\n")


def camera_inference():

    print("\nInferring on snapshot...")
    logger.info("Inferring on snapshot...")
    inference_result = snap_infer()
    logger.debug(f"Inference Result: {inference_result}")
    print(f"\nInference Result: {inference_result}")

    if inference_result:
        centroids = get_centroids(inference_result)
        logger.debug(f"Centroids: {centroids}")
        print(f"Centroids: {centroids}")
        
        widths = get_object_width(inference_result)
        logger.debug(f"Object widths: {widths}")
        print(f"Object widths: {widths}")

        print("\nResolving World Coordinate...")
        world_coordinates = get_world_cooridinates_final(centroids, widths)

        return world_coordinates

    else:
        # TODO: Implement retry logic
        logger.warning("No object detected! Retrying")
        print("No object detected! Retrying")
        return []


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


def get_object_width(coordinates: list):
    widths = []

    for coordinate in coordinates:
        x1 = int(coordinate['x1'])
        x2 = int(coordinate['x2'])

        width = x2 - x1
        widths.append(width)

    return widths



def snap_infer():
    ret, frame = cap.read()
    flipped_frame = cv2.flip(frame, -1)  # flip both axes

    # TODO: Add error handling
    if not ret:
        logger.error("Failed to Read Camera Frame")
        print("Failed to Read Camera Frame")

    inference_result = infer(flipped_frame)

    return inference_result


def arm_comms(gpio_pick: int = 0, gpio_place: int = 0, coordinates: list = []):
    print("\nArm Communication Initiated...")
    logger.info("Arm Communication Initiated...")
    # * Pi - Arduino Arm Communication
    # 1. Pi polls GPIO for event from Mobile-Platform
    # 2. If GPIO.Pick is detected, run camera_inference function
    # 3. Get coordinates from camera_inference function
    # 4. Pass the action + coordinates via serial to Arm
    # 5. Arm executes action -> Await message from Arm
    # 6. If message is SUCCESS, send GPIO.Go to Mobile-Platform
    #! 7. If message is FAIL, send GPIO.Fail to Mobile-Platform
    # 8. Mobile-Platform pulls GPIO.Pick low
    # ? 9. Pi pulls GPIO.Go low (for next cycle)

    # while True:
    # 2. If GPIO.Pick is detected, run camera_inference function
    if gpio_pick == 1:
        print("Pick Command")
        logger.debug("Pick Command")

        # # 3. Get coordinates from camera_inference function
        # coordinate_string = camera_inference()
        # print(f"coordinate_string: {coordinate_string}")

        # 4. Pass the action + coordinates via serial to Arm
        action = 0  # 0 = Pick, 1 = Place

        # Format message to send to Arm
        point = coordinates[0]
        formatted_msg = f"{action}|{point[0]}|{point[1]}|{point[2]}\n"
        logger.debug(f"Sending formatted_msg: {formatted_msg}")
        print(f"Sending formatted_msg: {formatted_msg}")

        # Send message to Arm
        ser.flushInput()
        ser.write(formatted_msg.encode('utf-8'))
        # 5. Arm executes action -> Await message from Arm

    # 5. Arm executes action -> # Await message from Arm
    ser.flushInput()
    ser.flushOutput()
    readall = ser.read_all()
    print(readall)
    arm_msg = ser.readline().decode('utf-8').rstrip()
    # if arm_msg:
    #     print(f"Received arm_msg: {arm_msg}")
    # else:
    #     print("Waiting for Arm message")

    # ? Await message from Arm
    # ? Use interrupts instead of polling
    while not arm_msg:
        logger.debug("Waiting for Arm message")
        print("Waiting for Arm message")
        arm_msg = ser.readline().decode('utf-8').rstrip()

    logger.debug(f"Received arm_msg: {arm_msg}")
    print(f"Received arm_msg: {arm_msg}")

    # 6. If message is SUCCESS, send GPIO.Go to Mobile-Platform
    if arm_msg == "SUCCESS":
        return 1

        # ? Run Loop again
        # break

    time.sleep(1)
    logger.info("Arm Sequence Complete")
    print("Arm Sequence Complete\n")
    #! remove this
    return 1


# Add an intentional block for debugging purposes
print("Press ENTER to start Cycle...")
logger.info("Awaiting kbd-event to start Cycle...")
input()

# * Super Loop
# TODO: Use interrupts instead of polling
try:
    while True:
        time.sleep(0.2)

        # * Mobile-Platform - Pi Communication
        # 1. Pi Polls GPIO for event from Mobile-Platform
        # 2. If GPIO.23 is high, action = Pick
        # 3. If GPIO.24 is high, action = Place

        # 1. Pi Polls GPIO for event from Mobile-Platform
        pick_event = GPIO.input(pick_pin)
        place_event = GPIO.input(place_pin)
        GPIO.output(go_pin, True)

        # 2. If GPIO.23 is high, action = Pick
        # print(readall)
        print(f"Pick event: {pick_event} Place event: {place_event}")
        logger.debug(f"Pick event: {pick_event} Place event: {place_event}")
        if pick_event:
            print("Pick event")
            logger.info("Pick event")
            world_coordinates = camera_inference()

            if world_coordinates:
                mobile_platform_event = arm_comms(
                    gpio_pick=1, coordinates=world_coordinates)
            else:
                print("No object detected! PANIC MODE!!!")
                logger.warning("No object detected! PANIC MODE!!!")
                mobile_platform_event = 1

        # 3. If GPIO.24 is high, action = Place
        elif place_event:
            # print("Place event")
            pass

        if mobile_platform_event:
            print("Mobile Platform Go")
            logger.info("Mobile Platform Go")
            GPIO.output(go_pin, False)
            place_event = 0
            pick_event = 0

            # 8. Mobile-Platform pulls GPIO.Pick or GPIO.Place low
            # give mobile platform time to pull GPIO low
            # TODO: Use interrupts instead of polling -> Remove unnecessary delays
            time.sleep(2)
            pick_event = GPIO.input(pick_pin)
            logger.debug(
                f"Pick event: {pick_event} Place event: {place_event}")
            print(f"Pick event: {pick_event} Place event: {place_event}")

            print("\n---------\n")
            # break

            # 9. Pi pulls GPIO.Go low (for next cycle)
            if not pick_event:
                GPIO.output(go_pin, True)
                mobile_platform_event = 0
                logger.info("EVENT CYCLE COMPLETE")
                print("EVENT CYCLE COMPLETE")
                print("\n---------------------------------------\n")

except KeyboardInterrupt:
    print("Keyboard Interrupt")
    logger.exception("Keyboard Interrupt")
except Exception as e:
    print(f"Unexpected Error: {e}")
    logger.exception(f"Unexpected Error: {e}")
finally:

    cap.release()
    GPIO.cleanup()

    logger.info("Program Terminated Gracefully")
    print("\n++++++++\nProgram Terminated Gracefully\n++++++++\n")
