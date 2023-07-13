import signal
import sys
import time
import RPi.GPIO as GPIO

BUTTON_GPIO = 16

if __name__=="__main__":
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    while True:
        GPIO.wait_for_edge(BUTTON_GPIO, GPIO.FALLING)
        print("Button pressed!")

    '''pressed = False

    while True:
        # button is pressed  when pin is LOW
        if not GPIO.input(BUTTON_GPIO):
            if not pressed:
                print("Button pressed!")
                pressed = True
        # button not pressed (or released)
        else:
            pressed = False
        time.sleep(0.1)'''
