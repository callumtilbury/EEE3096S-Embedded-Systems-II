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

# Variables:
leds = [11, 13, 15]

def setup():
	# Use BOARD numbering
	GPIO.setmode(GPIO.BOARD)
	# Turn off warning messages
	GPIO.setwarnings(False)
	# Set up LED pins as outputs, and turn them off
	GPIO.setup(leds, GPIO.OUT, initial=GPIO.LOW)

def loop():
	for i in range(3):
		GPIO.output(leds[i],1)
		time.sleep(1)
		GPIO.output(leds[i],0)


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
