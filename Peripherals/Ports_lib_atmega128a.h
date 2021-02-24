//Makes it so that individual pins can be refered to on each port

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT_A (* (volatile bits *) &PORT_A)
#define PIN_A (* (volatile bits *) &PINA)
#define DDR_A (* (volatile bits *) &DDRA)
#define PA_7 PORT_A.b7
#define PA_6 PORT_A.b6
#define PA_5 PORT_A.b5
#define PA_4 PORT_A.b4
#define PA_3 PORT_A.b3
#define PA_2 PORT_A.b2
#define PA_1 PORT_A.b1
#define PA_0 PORT_A.b0

#define PORT_B (* (volatile bits *) &PORTB)
#define PIN_B (* (volatile bits *) &PINB)
#define DDR_B (* (volatile bits *) &DDRB)
#define PB_7 PORT_B.b7
#define PB_6 PORT_B.b6
#define PB_5 PORT_B.b5
#define PB_4 PORT_B.b4
#define PB_3 PORT_B.b3
#define PB_2 PORT_B.b2
#define PB_1 PORT_B.b1
#define PB_0 PORT_B.b0

#define PORT_C (* (volatile bits *) &PORTC)
#define PIN_C (* (volatile bits *) &PINC)
#define DDR_C (* (volatile bits *) &DDRC)
#define PC_7 PORT_C.b7
#define PC_6 PORT_C.b6
#define PC_5 PORT_C.b5
#define PC_4 PORT_C.b4
#define PC_3 PORT_C.b3
#define PC_2 PORT_C.b2
#define PC_1 PORT_C.b1
#define PC_0 PORT_C.b0

#define PORT_D (* (volatile bits *) &PORTD)
#define PIN_D (* (volatile bits *) &PIND)
#define DDR_D (* (volatile bits *) &DDRD)
#define PD_7 PORT_D.b7
#define PD_6 PORT_D.b6
#define PD_5 PORT_D.b5
#define PD_4 PORT_D.b4
#define PD_3 PORT_D.b3
#define PD_2 PORT_D.b2
#define PD_1 PORT_D.b1
#define PD_0 PORT_D.b0

#define PORT_E (* (volatile bits *) &PORTE)
#define PIN_E (* (volatile bits *) &PINE)
#define DDR_E (* (volatile bits *) &DDRE)
#define PE_7 PORT_E.b7
#define PE_6 PORT_E.b6
#define PE_5 PORT_E.b5
#define PE_4 PORT_E.b4
#define PE_3 PORT_E.b3
#define PE_2 PORT_E.b2
#define PE_1 PORT_E.b1
#define PE_0 PORT_E.b0

#define PORT_F (* (volatile bits *) &PORTF)
#define PIN_F (* (volatile bits *) &PINF)
#define DDR_F (* (volatile bits *) &DDRF)
#define PF_7 PORT_F.b7
#define PF_6 PORT_F.b6
#define PF_5 PORT_F.b5
#define PF_4 PORT_F.b4
#define PF_3 PORT_F.b3
#define PF_2 PORT_F.b2
#define PF_1 PORT_F.b1
#define PF_0 PORT_F.b0

typedef struct{ uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
uint8_t b7:1; } bits;