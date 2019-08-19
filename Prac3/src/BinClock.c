/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * --
 * EEE3096S Practical 3
 * Devon Pugin & Callum Tilbury
 * <PGNDEV001> <TLBCAL002>
 * 16 August 2019
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> 	//For printf functions
#include <stdlib.h> 	// For system functions
#include <signal.h> 	// For keyboard interrupt handling
#include <math.h> 	// For some operations
#include <softPwm.h> 	// For pwm on 'seconds' LED
#include <stdbool.h>  	// To allow bool data type

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int HH, MM, SS;		// Stores the hour, minute and second time values
int bounceTime = 200; 	// Minimum interval between interrupts (ms)
long lastInterruptTime = 0; // Used for button debounce
int RTC; 		// Holds the RTC instance
bool verbose = true;	// Specifies if program should print info to screen

/*
 * Initialize GPIO pins
 */
void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	if(verbose) printf("Setting up\n");
	wiringPiSetup(); // Default mode
	
	RTC = wiringPiI2CSetup(RTCAddr); // Set up the RTC
	
	// LED setup
	for(int i = 0; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
		pinMode(LEDS[i], OUTPUT); 	// Sets up LED pins as outputs
		// Short visual display to indicate LEDs are working and setup routine is busy
		digitalWrite(LEDS[i], 1);
		delay(50);
		digitalWrite(LEDS[i], 0);
	}
	
	// Seconds LED setup
	softPwmCreate(SECS, 0, 60); // PWM range between 0 and 60
	// Short visual display to indicate LED is working
	for(int i = 0; i < 60; i++) {
		softPwmWrite(SECS, i); 
		delay(10);
	}

	if(verbose) printf("LEDS done\n");
	
	// Buttons Setup
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);		// Button pins as inputs
		pullUpDnControl(BTNS[j], PUD_UP); 	// Pull-up resistor
	}
	
	// Attach interrupts to Buttons
	wiringPiISR(BTNS[0], INT_EDGE_RISING, &hourInc);	// Hour increment button
	wiringPiISR(BTNS[1], INT_EDGE_RISING, &minInc); 	// Minute increment button

	if(verbose) printf("BTNS done\n");
	
	if(verbose) printf("Setup done\n");
}

/*
 * Deinitialize and reset GPIO pins
 */
void cleanGPIO(void) {
	if(verbose) printf("\nCleaning up... Bye!\n");
	
	// Set all LED pins back to INPUTS, as this is safer
	for(int i = 0; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
		pinMode(LEDS[i], INPUT);
	}
	pinMode(SECS, INPUT); 	// Set Seconds LED back to INPUT
	exit(1);		// Can now safely exit the program
}

/*
 * Main function
 */
int main(int argc, char* argv[]) {
	// User has option to enable silent mode by passing in any second argument when invoking program
	if (argc >= 2) {
		verbose = false;
	} else verbose = true;

	initGPIO(); 			// Setup pins
	signal(SIGINT, cleanGPIO);	// Add Ctrl-C interrupt to clean GPIO pins on exit

	// Setup using system time, which may or may not be accurate
	getSystemTime();
	if(verbose) printf("Initialised with time: %x:%x:%x\n", HH, MM, SS);

	// Repeat this until we shut down
	for (;;){
		// Fetch the current time from the RTC and update the LEDs accordingly
		updateTime();
		// Print out the time we have stored on the RTC to the screen
		if(verbose) printf("Time: %d:%d:%d\n", HH, MM, SS);
		// Using a delay to make our program "less CPU hungry"
		delay(1000); // ms
	}
	return 0;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	// Simple algorithm for converting decimal value to binary, and then displaying result on the relevant LEDs
	for (int i = 0; i <= 3; i++) {
		int tmp = pow(2,3-i);
		digitalWrite(LEDS[i], units/tmp);
		units %= tmp;
	}
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	// Simple algorithm for converting decimal value to binary, and then displaying on the relevant LEDs
	// Note that the "minutes" LEDs are in the LED array from index 4 to 9
	for (int i = 4; i <= 9; i++) {
		int tmp = pow(2,9-i);
		digitalWrite(LEDS[i], units/tmp);
		units %= tmp;
	}
}

/*
 * PWM for the Seconds LED
 * Off == 0 seconds ---> Full brightness == 60 seconds
 */
void secPWM(int units){
	softPwmWrite(SECS, units);
}

/*
 * hexCompensation
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back to RTC
 * Software debouncing used
 */
void hourInc(void){
	// Debounce
	long interruptTime = millis();
	if (interruptTime - lastInterruptTime > bounceTime){
		// Fetch RTC Time
		updateTime();
		// Increase hours by 1, ensuring not to overflow
		HH = (HH+1)%12;
		// Display value on LEDs
		lightHours(HH);
		// Convert to BCD
		HH = decCompensation(HH);
		// Write hours back to the RTC
		wiringPiI2CWriteReg8(RTC, HOUR, HH);
	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Software Debouncing should be used
 */
void minInc(void){
	// Debounce
	long interruptTime = millis();
	if (interruptTime - lastInterruptTime > bounceTime){
		//Fetch RTC Time
		updateTime();
		// Increase minutes by 1, and update hours if necessary
		if ((++MM) >= 60) {
			hourInc();
			MM = 0;
		}
		// Display value on LEDs
		lightMins(MM);
		// Convert to BCD
		MM = decCompensation(MM);
		// Write minutes back to the RTC
		wiringPiI2CWriteReg8(RTC, MIN, MM);
	}
	lastInterruptTime = interruptTime;
}

// Grabs system time if possible
void getSystemTime(void){
	HH = getHours()%12;
	MM = getMins();
	SS = getSecs();
	
	// Hours
	lightHours(HH);
	HH = decCompensation(HH);
	wiringPiI2CWriteReg8(RTC, HOUR, HH);

	// Minutes
	lightMins(MM);
	MM = decCompensation(MM);
	wiringPiI2CWriteReg8(RTC, MIN, MM);

	// Seconds
	secPWM(SS);
	SS = decCompensation(SS);
	wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);
}

void updateTime(void){
	// Updates time from RTC reading
        HH = (hexCompensation(wiringPiI2CReadReg8(RTC, HOUR))%12);
	MM = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
        SS = hexCompensation(wiringPiI2CReadReg8(RTC, SEC) & 0b01111111);
	// Displays time on LEDs
	lightHours(HH);
	lightMins(MM);
	secPWM(SS);
}
