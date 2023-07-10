import RPi.GPIO as GPIO


# I noticed that the pick and place pins are high when mobile platform is first powered
# This script verifies that

# * RESULT: pick_pin goes high when mobile platform is first powered!

pick_pin = 23
place_pin = 24

GPIO.setmode(GPIO.BCM)
# Avoid floating state by attaching input pins to internal pulldown resistors
GPIO.setup(pick_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(place_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

try:
    while True:
        pick_event = GPIO.input(pick_pin)
        place_event = GPIO.input(place_pin)

        print(f"Pick event: {pick_event} Place event: {place_event}")
finally:
    GPIO.cleanup()
