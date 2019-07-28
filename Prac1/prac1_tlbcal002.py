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

# Run once
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

# Repeat forever
def loop():
	# Use the global count value
	global count
	# Update the leds with count value
	GPIO.output(leds,states[count])

# Callback for buttons pushed
def button_callback(channel):
	# Use the global count value
	global count
	# If increment button was pressed
	if channel == buttons[0]: count += 1
	# If decrement button was pressed
	elif channel == buttons[1]: count -= 1
	# Shouldn't get here
	else: print("Unknown callback detected!")
	# Bring value back into appropriate range for leds
	count = count%(2**(len(leds)));


# Only run the functions if invoked directly
if __name__ == "__main__":
	try:
		# Once
		setup()
		# Forever
		while True:
			loop()
	except KeyboardInterrupt:
		print("Exiting gracefully")
		# Turn off GPIOs
		GPIO.cleanup()
	except Exception as e:
		GPIO.cleanup()
		print("An error occurred!")
		print(e.message)
