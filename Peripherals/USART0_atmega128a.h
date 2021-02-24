/*
 * USART.c
 *
 * Created: 6/3/2013 2:02:42 AM
 *  Author: Quentin
 */ 
//This sets up the USART for 9600 baud and it creates sub-functions
//to send and receive data.

#include <util/delay.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/iom128a.h>
#include <math.h>
unsigned char USART_data = 0x00;

unsigned char receive_data0(void)
{
	
	while (! (UCSR0A &(1 << RXC0)));
	return UDR0;
}

int usart_put_char(char c, FILE *stream)
{
	if (c == '\n') usart_put_char('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0); // wait for UDR to be clear
	UDR0 = c;    //send the character
	return 0;
}

void transmit_data0 (unsigned char data)
{
	//Wait until the Transmitter is ready
	while (! (UCSR0A & (1 << UDRE0)) );

	//Sends out data
	UDR0 = data;
}

void USART_initialize0( void)
{
	unsigned int UBBR = 47;

	//Put the upper part of the baud number here (bits 8 to 11)
	UBRR0L = (unsigned char) (UBBR >> 8);
	//Put the remaining part of the baud number here
	UBRR0L = (unsigned char) UBBR;

	//Enable the receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0)| (1 << RXCIE0);
	UCSR0C = (0 << USBS0) | (3 << UCSZ00)  |(0 << UPM01) | (0 <<UPM00);
}
