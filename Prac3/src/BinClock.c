/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <PGNDEV001> <TLBCAL002>
 * Date
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include <signal.h> // For keyboard interrupt handling
#include <math.h> // For some operations
#include <softPwm.h> // For pwm on 'seconds' LED

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
//int hours, mins, secs;
int HH, MM, SS;

long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i = 0; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
		pinMode(LEDS[i], OUTPUT);
		// Quick fun visual display
		digitalWrite(LEDS[i], 1);
		delay(50);
		digitalWrite(LEDS[i], 0);
	}
	
	//Set Up the Seconds LED for PWM
	softPwmCreate(SECS, 0, 60);
	for(int i = 0; i < 60; i++) {
		softPwmWrite(SECS, i);
		delay(10);
	}

	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
	
	//Attach interrupts to Buttons
	//Write your logic here
	wiringPiISR(BTNS[0], INT_EDGE_RISING, &hourInc);
	//wiringPiISR(BTNS[0], INT_EDGE_RISING, &plusHours);
	wiringPiISR(BTNS[1], INT_EDGE_RISING, &minInc); 

	
	printf("BTNS done\n");
	printf("Setup done\n");
}

void cleanGPIO(void) {
	printf("\nCleaning up... Bye!\n");
    	digitalWrite(LEDS[0],0);
	for(int i = 0; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
		pinMode(LEDS[i], INPUT);
	}
	pinMode(SECS, INPUT);
	exit(1);
}

/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();
	signal(SIGINT, cleanGPIO);

	// Setup using system time, which may or may not be accurate
	getSystemTime();
	printf("Initialised with time: %x:%x:%x\n", HH, MM, SS);

	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		updateTime();
		// Print out the time we have stored on our RTC
		printf("Time: %d:%d:%d\n", HH, MM, SS);
		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	for (int i = 3; i >= 0; i--) {
		int tmp = pow(2,i);
		digitalWrite(LEDS[i], units/tmp);
		units %= tmp;
	}
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	for (int i = 9; i >= 4; i--) {
		int tmp = pow(2,i-4);
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
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
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
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Button 1 pressed: increase hours\n");
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
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Button 2 pressed: increase minutes\n");
		//Fetch RTC Time
		updateTime();
		//Increase minutes by 1, ensuring not to overflow
		MM = (MM+1)%60;
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
	MM = getMins()%60;
	SS = getSecs()%60;

	lightHours(HH);
	HH = decCompensation(HH);
	wiringPiI2CWriteReg8(RTC, HOUR, HH);

	lightMins(MM);
	MM = decCompensation(MM%60);
	wiringPiI2CWriteReg8(RTC, MIN, MM);

	secPWM(SS);
	SS = decCompensation(SS%60);
	wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);
}

void updateTime(void){
        HH = (hexCompensation(wiringPiI2CReadReg8(RTC, HOUR))%12);
	MM = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
        SS = hexCompensation(wiringPiI2CReadReg8(RTC, SEC) & 0b01111111);
	lightHours(HH);
	lightMins(MM);
	secPWM(SS);
}
