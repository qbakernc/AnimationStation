/*
 * Timer_Counter.c
 *
 * Created: 5/25/2013 4:01:05 PM
 *  Author: Quentin
 */ 
//This program shows how to set up an 8-bit PWM Timer/Counter with the ATMEGA128A board
//This one particularly shows how to set up Pulse Width Modulation with Phase Correct
//The registers used are TCCR0, OCR0, and ASSR
//A value can be read from TCNT0
//With the PWM the counter counts from 0 to 0xFF regardless and it counts back down
//Frequency of PWM signal = Fclk or board/(pre-scaler*510) This is the time it takes
//to count from up to down.
//Right now the pre-scaler is 1024 and the FPWM is = 30.63Hz = 32.64ms                    
//This also houses the random number generator

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
int num = 79;
int a = 10;
int b = 20;

int random_num ()
{
	num = ((a*num + b)%TCNT0);
	return num;
}

void Timer_initialize()
{
	TCCR0 = 0x61; //This chooses your Compare output mode, your pre-scaler and
	//and what you want your OC0 to do.  The OC0 pin has to be set as an output to get a reading.
	OCR0 = 127;   //This value is determined by what you want your duty cycle and average voltage
	// to be.  It is set to 127 for a %50 duty cycle because OC0 toggles
	// every time there is a compare match between TCNT0 and OCR0.

}