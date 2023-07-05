import RPi.GPIO as GPIO
import serial
import time
# import cv2


# Setup Serial Communication
# ser = serial.Serial('/dev/ttyACM2', 9600, timeout=1)
# ser.reset_input_buffer()

# Setup GPIO
pick_pin = 23
place_pin = 24
go_pin = 25

GPIO.setmode(GPIO.BCM)
GPIO.setup(pick_pin, GPIO.IN)
GPIO.setup(place_pin, GPIO.IN)
GPIO.setup(go_pin, GPIO.OUT)

print("\n-----\nPi Ready")

mobile_platform_event = 0

# Camera Inference Function
def camera_inference():
    x, y, z = 10, 20, 30

    return (x, y, z)


def arm_comms_simulator(gpio_pick: int = 0, gpio_place: int = 0):
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

    while True:
        # 2. If GPIO.Pick is detected, run camera_inference function
        if gpio_pick == 1:
            print("GPIO.Pick detected")

            # 3. Get coordinates from camera_inference function
            coordinate_string = camera_inference()
            print(f"coordinate_string: {coordinate_string}")

            # 4. Pass the action + coordinates via serial to Arm
            action = 0  # 0 = Pick, 1 = Place

            # Format message to send to Arm
            formatted_msg = f"{action}|{coordinate_string[0]}|{coordinate_string[1]}|{coordinate_string[2]}\n"
            print(f"Sending formatted_msg: {formatted_msg}")

            # Send message to Arm
            # ser.flushInput()
            # ser.write(formatted_msg.encode('utf-8'))
            # 5. Arm executes action -> Await message from Arm

        # 5. Arm executes action -> # Await message from Arm
        # ser.flushInput()
        # ser.flushOutput()
        # readall = ser.read_all()
        # print(readall)
        # arm_msg = ser.readline().decode('utf-8').rstrip()
        # if arm_msg:
        #     print(f"Received arm_msg: {arm_msg}")
        # else:
        #     print("Waiting for Arm message")

        # ? Await message from Arm
        # while not arm_msg:
        #     print("Waiting for Arm message")
        #     arm_msg = ser.readline().decode('utf-8').rstrip()

        # print(f"Received arm_msg: {arm_msg}")

        # 6. If message is SUCCESS, send GPIO.Go to Mobile-Platform
        # if arm_msg == "SUCCESS":
        #     return 1

            # ? Run Loop again
            # break

        time.sleep(1)
        return 1


# arm_comms_simulator(1)


# * Super Loop
while True:
    
    try:
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
        if pick_event:
            print("Pick event")
            camera_inference()
            # // action = 0
            # mobile_platform_event = arm_comms_simulator(1, 0)
            counter = 0
            for i in range(10):
                counter += 1
                time.sleep(1)
                print(f"waiting: {counter}")

            # time.sleep(60)
            mobile_platform_event = 1

        # 3. If GPIO.24 is high, action = Place
        elif place_event:
            # print("Place event")
            pass

        if mobile_platform_event:
            print("Mobile Platform Go")
            GPIO.output(go_pin, False)
            place_event = 0
            pick_event = 0
            print(f"Pick event: {pick_event} Place event: {place_event}")

            # 8. Mobile-Platform pulls GPIO.Pick or GPIO.Place low
            # give mobile platform time to pull GPIO low
            time.sleep(2)
            pick_event = GPIO.input(pick_pin)
            print(f"Pick event: {pick_event} Place event: {place_event}")

            print("\n---------\n")
            break

            # # 9. Pi pulls GPIO.Go low (for next cycle)
            # if not pick_event:
            #     GPIO.output(go_pin, False)
            #     mobile_platform_event = 0
            #     print("Event Cycle Complete")
            #     print("---------------------------------------\n")
    except KeyboardInterrupt:
        print("Keyboard Interrupt")
    except Exception as e:
        print(f"Unexpected Error: {e}")
    finally:
        GPIO.cleanup()
        break