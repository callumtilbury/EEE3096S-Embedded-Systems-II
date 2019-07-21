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

# Logic that you write
def main():
	setup();
	loop();

def setup():
	GPIO.setup(11, GPIO.OUT, initial=GPIO.LOW)

def loop():
	GPIO.output(11, 1)
	time.sleep(1)
	GPIO.output(11, 0)
	time.sleep(1)


# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)
