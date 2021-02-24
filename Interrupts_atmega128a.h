/*
 * Interrupts.c
 *
 * Created: 5/24/2013 12:42:42 AM
 *  Author: Quentin
 */ 


//This program shows how to set up interrupts with the atmega128m mini development board.
//It also shows how to setup input and outputs on the DDRs and it shows how to tell if a
//pin is high or low.

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


//Interrupt vector INT3 which is located on PIND3
ISR(INT1_vect)
{

}

//Interrupt vector INT3 which is located on PIND3
ISR(INT2_vect)
{

}


void Interrupt_initialize(void)
{
	EICRA = 0xEA; //Sets up INT3:0 to be activated whenever a logic low is detected
	EIMSK = 0x07; //Sets it up so that INT0, INT1, INT2, and INT3 are activated
	sei();  //Enables interrupts
}