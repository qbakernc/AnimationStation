/*
 * Tetris.c
 *
 * Created: 6/14/2013 8:37:01 PM
 *  Author: Quentin
	
	SPI is initiated to operate the MAX 7219 chip.  CC is located on PinB.0
	MOSI is located on PinB.2 Clock is located on PinB.1
	The MSB of data is transmitted first during data transfer.  The max 7219 chip accepts the MSB first.
 */ 


#ifndef F_CPU
#define F_CPU 7372800UL // frequency of the atmega128a clock used to make delay work properly.
#endif
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/iom128a.h>
#include <math.h>


void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB = (1 << PORTB1)|(1 << PORTB2) | (1 << PORTB0)| (0 << PORTB3);
	PORTB = 0b01111000;
	/* Enable SPI, Master, set clock rate fck/16 */
	SPSR = (1 << SPI2X);
	SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (CPHA << 0) | (1 << CPOL)
	| (0 << SPR1) | (0 << SPR0);
}

void SPI_MasterTransmit(char dData, char eData, char fData, char gData)
{
	/* Start transmission */
	SPDR = dData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	
	/* Start transmission */
	SPDR = eData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	
	/* Start transmission */
	SPDR = fData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
		
	/* Start transmission */
	SPDR = gData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	
	//latches data into the seven segment driver.
	PB_0 = 1;
	PB_0 = 0;
}


void LED_Matrix_init (void)
{
	//Display test register.  It is set for normal operation
	SPI_MasterTransmit(0x0F, 0x00, 0x0F, 0x00); 

	//Shutdown register. It is set for normal operation
	SPI_MasterTransmit(0x0C, 0x01, 0x0C, 0x01); 
	
	//Decode mode register. It is set for no decode mode
	//Each segment can be addressed individually.
	SPI_MasterTransmit(0x09, 0x00, 0x09, 0x00); 
	
	//Scan limit register. It is setup so that dig0-7 will display
	SPI_MasterTransmit(0x0B, 0x07, 0x0B, 0x07); 

	//Intensity register. It is setup so that the intensity is at max
	SPI_MasterTransmit(0x0A, 0x00, 0x0A, 0x00); 
}

