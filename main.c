/* Main program that runs the overbed tray table. 
*
*/

#include <avr/io.h>
#include "util/delay.h"
#include "uart.h"
#include "DualVNH5019MotorShield.h"


int blockInputs;
int pivotStatus;    //0 = close, 1 = away
int home;			//1 when the table is at its home state
int zTop; 			//1 when the table is at the top
const int linDelay = 2000;		//time in milliseconds to reach the top
const int pivotDelay = 2000;		//time in milliseconds to reach the top
const int posSpeed = 400;
const int negSpeed = -400; 
DualVNH5019MotorShield md;


//setup function
int setup(void) {
	//Set the input and output pins

	//inputs
	DDRB &= ~(1 << PB0);     //Z-up
	DDRB &= ~(1 << PB1);     //Z-down
	DDRB &= ~(1 << PB2);     //Pivot
	PORTB |= (1 << PB0);     //set to pull-up
	PORTB |= (1 << PB1);
	PORTB |= (1 << PB2); 

	//outputs 
	DDRB |= (1 << PB3);      //Linear-Up
	DDRB |= (1 << PB4);		//Linear-Down
	DDRB |= (1 << PB5);		//Servo
	PORTB &= ~(1 << PB3);     //set to low
	PORTB &= ~(1 << PB4);     //set to low
	PORTB &= ~(1 << PB5);     //set to low

	blockInputs = 0;			//flag that serves as a mutex to indicate that we are in the process
	zTop = 0; 
	home = 0; 
	pivotStatus = 0; 
	md.init();
}


//set the table to its home state
int homeSet(void) {
	//calling this drives the table as high as it can go, and rotatest it away from the human (status 0) 
	if(!home) {
		blockInputs = 1;				//raise flag
		if(!pivotStatus) {
			rotation(posSpeed);
		}
		if(!zTop) {
			//all other values are 0
			PORTB &= ~(1 << PB4);     //set to low
			PORTB &= ~(1 << PB5);     //set to low
			//set to high
			PORTB |= (1 << PB3);
			//time to drive to the top
			_delay_ms(linDelay);
			//done driving
			PORTB &= ~(1 << PB3);     //set to low 
			zTop = 1;
		}	
		home = 1;
		blockInputs = 0;				//clear flag						
	}
}

//rotate the bed
int rotation(int speed) {
	blockInputs = 1;			//raise flag
	//all other values are 0
	PORTB &= ~(1 << PB3);     //set to low
	PORTB &= ~(1 << PB4);     //set to low
	md.setM1Speed(speed); 
	_delay_ms(pivotDelay);
	md.setM1Speed(0);
	blockInputs = 0;			//clear flag
	pivotStatus = !pivotStatus; //opposite status now
}


int main(void) {
	uart_init();
	setup();
	homeSet();

	while(1) {
		//z-up button
		if(!blockInputs & (PINB & 0x01)) {
			if(!zTop) {
				blockInputs = 1;
				//all other values are 0
				PORTB &= ~(1 << PB4);     //set to low
				PORTB &= ~(1 << PB5);     //set to low
				//set to high
				PORTB |= (1 << PB3);
				//keep it high
				while(PINB & 0x01) {
				}
				//done driving
				PORTB &= ~(1 << PB3);     //set to low 
				blockInputs = 0;
			}
		}	

		//z-down button
		if(!blockInputs & ((PINB & 0x02) >> 0x01)) {
			blockInputs = 1;			//raise flag
			//all other values are 0
			PORTB &= ~(1 << PB3);     //set to low
			PORTB &= ~(1 << PB5);     //set to low
			//set to high
			PORTB |= (1 << PB4);
			//keep it high
			while(((PINB & 0x02) >> 0x01)) {
			}
			//done driving
			PORTB &= ~(1 << PB4);     //set to low 
			blockInputs = 0;			//clear flag
			zTop = 0;					//can't be at the top anymore
			home = 0;					//can't be home anymore
		}
		
		//pivot button - need to take care of the rotation (maybe using delay.h?)
		if(!blockInputs & ((PINB & 0x04) >> 0x02)) {
			if(home) {
				rotation(posSpeed); 			
			}
			else {
				rotation(negSpeed); 
			}
			if(pivotStatus & zTop) {
					home = 1;
			}
			else {
				home = 0;
			}		
		}
	}
}


