/*
 * $safeprojectname$.c
 *
 * Created: 10/3/2014 9:35:37 PM
 *  Author: Quentin
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
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "Interrupts_atmega128a.h"
#include "Ports_lib_atmega128a.h"
#include "LED_Matrix_atmega128a.h"
#include "USART1_atmega128a.h"
#include "Timer_atmega128a.h"
FILE transmit = FDEV_SETUP_STREAM(usart_put_char, NULL, _FDEV_SETUP_WRITE );

/*Temporarily Stores the received Data for the processor to determine what action to take.*/
volatile uint8_t  Temp;		 //Data Received from the Terminal.
volatile uint8_t  ROW_Temp;  //Data stored here will be used to select the row of LEDs we want to control.
volatile uint8_t  DELAY_Temp;//Data stored here will be used to activate the delay options.
volatile uint8_t LED_Temp;  //Data stored here will be used to signify which LEDs we want to turn on.   

/*The SEG_* int arrays store all of our animations by Rows. Each individual array address will control all 8 LEDs on a Row.
  SEG_1 is associated with Row 1, SEG_2 with Row 2.......SEG_16 with Row 16. It is set to have 75 different animations. */			
int Segments[16][75];

/*The Dig_* store the Hexadecimal addresses to activate a specific Row for LED control. 
  Dig_1 activates Row 1, Dig_2 activates Row 2.......Dig_16 activates Row 16.  The controls the actual hardware.*/
enum Columns {Dig_1 = 0x01 ,Dig_2 = 0x02, Dig_3 = 0x03, Dig_4 = 0x04, Dig_5 = 0x05, Dig_6 = 0x06, Dig_7 = 0x07, Dig_8 = 0x08, Latch = 0x09, Start = 0x0D};
	
volatile int ROW_SELECTED[16];
volatile int ROW_SELECTION_BUSY = 0; //A row has been selected. The program is waiting for LED selections.
volatile int DELAY_SELECTED = 0; //Delay has been selected.
volatile int DELAY_SELECTION_BUSY = 0; //Delay selection is currently in progress.
volatile int LED_SELECTION_BUSY = 0; //LED Selection is currently in progress.

int delay_value[75]; //Stores our delay time for each animation stored
int ac = 0; //Counts up the number of animations. Animation counter
volatile int i = 0;

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address and data registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}

void Delay_Storage(int test)
{
	delay_value[ac] = test;
	printf("%d00 Second Delay\n" , test);
}

//Interrupt vector INT2 which is located on PIND0
ISR(USART1_RX_vect)
{
	Temp = UDR1; //Stores the received data in the Temp variable. 
	
	/*Row options can only be selected if Delay options are not active (DELAY_SELECTION_BUSY == 0). ROW_temp data will be loaded
	  Letters `,a,b,c,d,e.....o will represent 1-16 rows in the ROW_SELECTED[] array. To make sure one of those options has been selected,
	  the IF.. statement's condition logically ANDs the Temp data with 0xF0 to see if the remaining data is equal to 0x60 which all of 
	  these letters start with. To get the correct numbers for the ROW_Selected[ROW_Temp] array, 0x60 is subtracted from Temp and Stored in ROW_Temp.
	  It will give us the values 0-15. That is 16 rows.*/
	if ((Temp & 0xF0) == 0x60 && DELAY_SELECTION_BUSY == 0 && ROW_SELECTION_BUSY == 0) {ROW_Temp = Temp - 0x60;}   
		
		/*Delay option can only be selected if Row options are not active(ROW_SELECTION_BUSY == 0). DELAY_Temp data will be loaded.
		  Letters q,r,u,z and t will represent the four delay options. To make sure one of those options has been selected,
		  the IF.. statement's condition logically ANDs the Temp data with 0xF0 to see if the remaining data is equal to 0x70 which all of 
		  these letters start with.*/
		 
		else if ((Temp & 0xF0) == 0x70 && ROW_SELECTION_BUSY == 0) {DELAY_Temp = Temp;} 
			
			/*Can not select what lights to turn on until a Row selection has been made (ROW_SELECTION_BUSY == 1). LED_Temp data will be loaded.*/
			else if (ROW_SELECTION_BUSY == 1){LED_Temp = Temp;}							
	
	
	/*The conditional statement checks to see if a row button has been pressed by subtracting 0x60 from the Temp value (which should give a value of 1-16 for the ROW_SELECTED[] array)
	  to see if the new received row is the same as the one that was previously selected. A logical AND is performed with ROW_SELECTED[ROW_Temp] to check for a 1 or 0 for that row to see
	  if it is active. If a row is selected (ROW_SELECTED[*] = 1), pressing the same row again will deselect the row. Also makes sure only one row can be selected at a time*/
	
	if ((Temp - 0x60) == ROW_Temp && ROW_SELECTED[ROW_Temp] == 0 ) { //Makes sure the selected row is not already active. If not, it will activate it. ROW_SELECTED[ROW_Temp] = 1
		ROW_SELECTED[ROW_Temp] = 1;									//Selects the row
		ROW_SELECTION_BUSY = 1;										//A row is selected busy flag.
		printf("Row %d Selected\n",ROW_Temp);						//Gives selection feedback
	} else if ((Temp - 0x60) == ROW_Temp && ROW_SELECTED[ROW_Temp] == 1) { //Makes sure the selected row is  already active. If so, it will deactivate it. ROW_SELECTED[ROW_Temp] = o
		printf("Row %d Deactivated\n",ROW_Temp);					//Gives selection feedback
		ROW_SELECTED[ROW_Temp] = 0;
		ROW_SELECTION_BUSY = 0; 
		ROW_Temp = 0; 
	} 

	/*The conditional statement checks to see if the delay selection has been pressed. If delay is selected (DELAY_SELECTED = 1), pressing delay again will deactivate delay.
	  This will confirm the delay time selection as well.*/	
	
	if (DELAY_Temp == 0x7B && DELAY_SELECTED == 0){
		DELAY_SELECTED = 1;
		DELAY_SELECTION_BUSY = 1;
		printf("Delay Selected\n");
	}	else if (DELAY_Temp == 0x7B  && DELAY_SELECTED == 1){
		DELAY_SELECTED = 0;
		DELAY_SELECTION_BUSY = 0;
		DELAY_Temp = 0;
		Temp = 0;
		printf("Delay Not Selected\n");
	} else if ((Temp & 0xF0) == 0x70 && DELAY_SELECTION_BUSY == 1){
		Delay_Storage(Temp & 0x0F);
	}
		
	/*Will either turn on or turn off the LEDs for the selected row. It finds the equivalently hex value for each individual light
	(0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01) by first subtracting 0x31 from the incoming serial data to get 0 - 7 to represent
	the 8 LEDs.  It will then shift 0x80 over by by the led number to get the hexadecimal value for it. e.x.(5 would shift 0x80 5 times to get 0x04)
	The value is stored in LED_Shift.*/
	
	if ((LED_Temp & 0xF0) ==  0x30 && ROW_SELECTION_BUSY == 1){
		for (int LED_Shift = (0x80 >> (LED_Temp - 0x31)), i = 0; i <= 15; i++){ 
			if (ROW_Temp == i && (Segments[i][ac] & LED_Shift) == LED_Shift){
				printf("LED %d Off\n", LED_Temp - 0x30);
				/*tuns off the LED by subtracting its hex value*/
				LED_Temp = -LED_Shift;												 
				
				} else if (ROW_Temp == i && (Segments[i][ac] & LED_Shift) == 0x00){
				printf("LED %d On\n", LED_Temp - 0x30);
				/*tuns off the LED by adding its hex value*/ 
				LED_Temp = LED_Shift;
			}
		}
		LED_SELECTION_BUSY = 1;
	} 	
}

/*This transmit all of the data after all LEDs and delays have been set*/
void Transmit(void) {
	SPI_MasterTransmit(Dig_1, Segments[8][i], Dig_1, Segments[0][i]);
	SPI_MasterTransmit(Dig_2, Segments[9][i], Dig_2, Segments[1][i]);
	SPI_MasterTransmit(Dig_3, Segments[10][i], Dig_3, Segments[2][i]);
	SPI_MasterTransmit(Dig_4, Segments[11][i], Dig_4, Segments[3][i]);
	SPI_MasterTransmit(Dig_5, Segments[12][i], Dig_5, Segments[4][i]);
	SPI_MasterTransmit(Dig_6, Segments[13][i], Dig_6, Segments[5][i]);
	SPI_MasterTransmit(Dig_7, Segments[14][i], Dig_7, Segments[6][i]);
	SPI_MasterTransmit(Dig_8, Segments[15][i], Dig_8, Segments[7][i]);
}
/*This is the delay function*/
void Delay(void) {
	int delay_counter = delay_value[i] * 100;//Multiplies by 100 because storage array for delay is only 8 bits. Highest number can only be 255. 
	
	while (delay_counter--) {
		_delay_ms(1);
	}
}

void Save_Data(void){
	for(int sData = 0; sData <= 74; sData++){
		EEPROM_write(sData,Segments[0][sData]);
		EEPROM_write(sData + 75,Segments[1][sData]);
		EEPROM_write(sData + 150,Segments[2][sData]);
		EEPROM_write(sData + 225,Segments[3][sData]);
		EEPROM_write(sData + 300,Segments[4][sData]);
		EEPROM_write(sData + 375,Segments[5][sData]);
		EEPROM_write(sData + 450,Segments[6][sData]);
		EEPROM_write(sData + 525,Segments[7][sData]);
		EEPROM_write(sData + 600,Segments[8][sData]);
		EEPROM_write(sData + 675,Segments[9][sData]);
		EEPROM_write(sData + 750,Segments[10][sData]);
		EEPROM_write(sData + 825,Segments[11][sData]);
		EEPROM_write(sData + 900,Segments[12][sData]);
		EEPROM_write(sData + 975,Segments[13][sData]);
		EEPROM_write(sData + 1050,Segments[14][sData]);
		EEPROM_write(sData + 1125,Segments[15][sData]);
		EEPROM_write(sData + 1200,delay_value[i]);
	}
	EEPROM_write(1250, ac);
}

void Load_Data(void){
	for(int lData = 0; lData <= 74; lData++){
		Segments[0][lData] = EEPROM_read(lData);
		Segments[1][lData] = EEPROM_read(lData + 75);
		Segments[2][lData] = EEPROM_read(lData + 150);
		Segments[3][lData] = EEPROM_read(lData + 225);
		Segments[4][lData] = EEPROM_read(lData + 300);
		Segments[5][lData] = EEPROM_read(lData + 375);
		Segments[6][lData] = EEPROM_read(lData + 450);
		Segments[7][lData] = EEPROM_read(lData + 525);
		Segments[8][lData] = EEPROM_read(lData + 600);
		Segments[9][lData] = EEPROM_read(lData + 675);
		Segments[10][lData] = EEPROM_read(lData + 750);
		Segments[11][lData] = EEPROM_read(lData + 825);
		Segments[12][lData] = EEPROM_read(lData + 900);
		Segments[13][lData] = EEPROM_read(lData + 975);
		Segments[14][lData] = EEPROM_read(lData + 1050);
		Segments[15][lData] = EEPROM_read(lData + 1125);
		delay_value[lData] = EEPROM_read(lData + 1200);
	}
	ac = EEPROM_read(1250);
}
/*Main program*/
int main(void) 
{							
	DDRC = 0xFF;
	PORTC = 0xFF;
	DDRB = 0b00010000;
	PORTB = 0b11101111;

	USART_initialize1();
	SPI_MasterInit();
	LED_Matrix_init();
	Interrupt_initialize();
	stdout = &transmit;		
	SPI_MasterTransmit(0x01, 0x00, 0x01, 0x00);
	SPI_MasterTransmit(0x02, 0x00, 0x02, 0x00);
	SPI_MasterTransmit(0x04, 0x00, 0x04, 0x00);
	SPI_MasterTransmit(0x08, 0x00, 0x08, 0x00);

	while(1){
		while (ROW_SELECTED[ROW_Temp]) {
			while(LED_SELECTION_BUSY) {
				Segments[ROW_Temp][ac] += LED_Temp; 						
				LED_SELECTION_BUSY = 0;
			}
		}
		/*Delay has been selected. Values will be stored into DELAY_STORAGE[]*/
		while (DELAY_SELECTED) {
		
		}
		
		/*Latches the animation and increments to the next animation slot*/
		if (Temp == Latch) {
			ac++;
			Temp = 0; //Clears Temp after the Latch
			printf("%d\n", ac); //Gives selection feedback
		}
		
		/*Loads the saved data so the animation can be ran again after power down.*/
		if(Temp == 0x41){
			Load_Data();
			Temp = 0;
			printf("previous data has been loaded \n");
		}
		
		/*This is the start the animation slide show*/
		if (Temp == Start) {
			Save_Data();
			while(1){
				for(i = 0; i <= ac; i++) {	
					Transmit();
					Delay();
				}
			}
		}
	}
}
					
				