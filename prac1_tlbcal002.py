#!/usr/bin/python3
"""
Python Practical 
Names: Callum Tilbury
Student Number: TLBCAL002
Prac: 1
Date: 21/07/2019
"""

# import Relevant Librares
import RPi.GPIO as GPIO
import time
import itertools

# Variables:
leds = [11, 13, 15]
buttons = [16, 18]

# LED states
states = list(itertools.product([0,1],repeat=len(leds)))

# Counter
count = 0

# Debounce time in milliseconds
debounce_ms = 300

def setup():
	# Use BOARD numbering
	GPIO.setmode(GPIO.BOARD)
	# Turn off warning messages
	GPIO.setwarnings(False)
	# Set up LED pins as outputs, and turn them off
	GPIO.setup(leds, GPIO.OUT, initial=GPIO.LOW)
	# Set up button pins as inputs, with internal pull-down resistors active
	GPIO.setup(buttons, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
	# Add event callbacks
	GPIO.add_event_detect(buttons[0], GPIO.RISING, callback=button_callback, bouncetime=debounce_ms)
	GPIO.add_event_detect(buttons[1], GPIO.RISING, callback=button_callback, bouncetime=debounce_ms)
	# Indicate to terminal that setup is done
	print("Setup complete.")

def loop():
	global count
	GPIO.output(leds,states[count])

def button_callback(channel):
	global count
	# If increment button was pressed
	if channel == buttons[0]: count += 1
	# If decrement button was pressed
	elif channel == buttons[1]: count -= 1
	else: print("Unknown callback detected!")
	# Bring value back into appropriate range for leds
	#count = count%(2**(len(leds)));


# Only run the functions if 
if __name__ == "__main__":
	# Make sure the GPIO is stopped correctly
	try:
		setup()
		while True:
			loop()
	except KeyboardInterrupt:
		print("Exiting gracefully")
		# Turn off your GPIOs here
		GPIO.cleanup()
	except e:
		GPIO.cleanup()
		print("Some other error occurred")
		print(e.message)
