/* Main program that runs the overbed tray table. 
*
*/

#include <avr/io.h>
#include "util/delay.h"


int blockInputs;

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

	blockInputs = 0;

	//flag that serves as a mutex to indicate that we are in the process

}

int main(void) {

	setup();

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
		//pivot button
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


