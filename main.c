/* Main program that runs the overbed tray table. 
*
*/

#include <avr/io.h>
#include "util/delay.h"


int blockInputs;
int pivotStatus;    //0 = close, 1 = away
int home;			//1 when the table is at its home state
int zTop; 			//1 when the table is at the top
int linDelay;		//time in milliseconds to reach the top
int pivotDelay;		//time in milliseconds to reach the top


//setup function
int setup(void) {
	//Set the input and output pins

	//inputs
	DDRB &= ~(1 << PORTB0);     //Z-up
	DDRB &= ~(1 << PORTB1);     //Z-down
	DDRB &= ~(1 << PORTB2);     //Pivot
	PINB &= ~(1 << PORTB0);     //set to low 
	PINB &= ~(1 << PORTB1);
	PINB &= ~(1 << PORTB2); 

	//outputs 
	DDRD |= (1 << PORTD0);      //Linear-Up
	DDRD |= (1 << PORTD1);		//Linear-Down
	DDRD |= (1 << PORTD2);		//Servo
	PIND &= ~(1 << PORTD0);     //set to low
	PIND &= ~(1 << PORTD1);     //set to low
	PIND &= ~(1 << PORTD2);     //set to low

	blockInputs = 0;			//flag that serves as a mutex to indicate that we are in the process
}

//set the table to its home state
int home(void) {
	//calling this drives the table as high as it can go, and rotatest it away from the human (status 0) 
	if(!home) {
		blockInputs = 1;				//raise flag
		if(!pivotStatus) {
			//all other values are 0
			PIND &= ~(1 << PORTD0);     //set to low
			PIND &= ~(1 << PORTD1);     //set to low
			//set to high
			PIND |= (1 << PORTD2);
			//keep it high
			delay(pivotDelay);
			//done driving
			PIND &= ~(1 << PORTD2);     //set to low 
			pivotStatus = 1;
		}
		if(!zTop) {
			//all other values are 0
			PIND &= ~(1 << PORTD1);     //set to low
			PIND &= ~(1 << PORTD2);     //set to low
			//set to high
			PIND |= (1 << PORTD0);
			//time to drive to the top
			delay(linDelay);
			//done driving
			PIND &= ~(1 << PORTD0);     //set to low 
			zTop = 1;
		}	
		home = 1;
		blockInputs = 0;				//clear flag						
	}

	

}

int main(void) {

	setup();
	home();

	while(1) {
		//z-up button
		if(!blockInputs && (PINB && 0x01)) {
			if(!zTop) {
				blockInputs = 1;			//raise flag
				//all other values are 0
				PIND &= ~(1 << PORTD1);     //set to low
				PIND &= ~(1 << PORTD2);     //set to low
				//set to high
				PIND |= (1 << PORTD0);
				//keep it high
				while(PINB && 0x01) {
				}
				//done driving
				PIND &= ~(1 << PORTD0);     //set to low 
				blockInputs = 0;			//clear flag
			}
		}

		//z-down button
		if(!blockInputs && ((PINB && 0x02) >> 0x01)) {
			blockInputs = 1;			//raise flag
			//all other values are 0
			PIND &= ~(1 << PORTD0);     //set to low
			PIND &= ~(1 << PORTD2);     //set to low
			//set to high
			PIND |= (1 << PORTD1);
			//keep it high
			while((PINB && 0x02) >> 0x01) {
			}
			//done driving
			PIND &= ~(1 << PORTD1);     //set to low 
			blockInputs = 0;			//clear flag
			zTop = 0;					//can't be at the top anymore
			home = 0;					//can't be home anymore
		}
		//pivot button - need to take care of the rotation (maybe using delay.h?)
		if(!blockInputs && ((PINB && 0x03) >> 0x02)) {
			blockInputs = 1;			//raise flag
			//all other values are 0
			PIND &= ~(1 << PORTD0);     //set to low
			PIND &= ~(1 << PORTD1);     //set to low
			//set to high
			PIND |= (1 << PORTD2);
			//keep it high
			delay(pivotDelay);
			//done driving
			PIND &= ~(1 << PORTD2);     //set to low 
			blockInputs = 0;			//clear flag
			pivotStatus = !pivotStatus; //opposite status now
			if(pivotStatus && zTop) {
				home = 1;
			}
			else {
				home = 0;
			}							//reset the home status
			blockInputs = 0;			//clear flag
		}

	}
	
}


