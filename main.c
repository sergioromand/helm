/* Main program that runs the overbed tray table. 
*
*/

#include <avr/io.h>
#include "util/delay.h"


int blockInputs;
int pivotStatus;    //0 = parallel to bed, 1 = perp to bed
int home;			//1 when the table is at its home state

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
	blockInputs = 1; 
	if(status) {
		//rotate home
	}
	

}

int main(void) {

	setup();
	home();

	while(1) {
		//z-up button
		if(!blockInputs && (PINB && 0x01)) {
			blockInputs = 1;
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
			blockInputs = 0;			//raise flag
		}

		//z-down button
		if(!blockInputs && ((PINB && 0x02) >> 0x01)) {
			blockInputs = 1;
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
			blockInputs = 0;			//raise flag
		}
		//pivot button - need to take care of the rotation (maybe using delay.h?)
		if(!blockInputs && ((PINB && 0x03) >> 0x02)) {
			blockInputs = 1;
			//all other values are 0
			PIND &= ~(1 << PORTD0);     //set to low
			PIND &= ~(1 << PORTD1);     //set to low
			//set to high
			PIND |= (1 << PORTD2);
			//keep it high
			while(((PINB && 0x03) >> 0x02)) {
			}
			//done driving
			PIND &= ~(1 << PORTD2);     //set to low 
			blockInputs = 0;			//raise flag
		}

	}
	
}


