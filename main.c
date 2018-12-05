/* Main program that runs the overbed tray table. 
*
*/

#include <avr/io.h>
#include "util/delay.h"


int blockInputs;
int pivotStatus;    //0 = close, 1 = away
int home;			//1 when the table is at its home state
int zTop; 			//1 when the table is at the top
const int linDelay = 2000;		//time in milliseconds to reach the top
const int pivotDelay = 2000;	//time in milliseconds to reach the top


//setup function
int setup(void) {
	//Set the input and output pins

	//inputs
	//Wireless inputs
	DDRD &= ~(1 << PD2);     //Z-up- Wireless
	DDRD &= ~(1 << PD3);     //Z-down- Wireless
	DDRD &= ~(1 << PD4);     //Pivot- Wireless
	PORTD |= (1 << PD2);     //set to pull-up
	PORTD |= (1 << PD3);
	PORTD |= (1 << PD4); 

	//Button interface inputs
	DDRC &= ~(1 << PC0);	//Z-up- Button
	DDRC &= ~(1 << PC1);	//Z-down- Button
	DDRC &= ~(1 << PC2);	//Pivot- Button
	PORTC |= (1 << PC0);     //set to pull-up
	PORTC |= (1 << PC1);
	PORTC |= (1 << PC2); 

	//Kill Switches (from actuation)
	DDRC &= ~(1 << PC3);    //Kill-Switch Home
	DDRC &= ~(1 << PC4);	//Kill-Switch Away
	PORTC |= (1 << PC3);	//set to pull-up
	PORTC |= (1 << PC4); 

	//outputs 
	//Linear Actuator
	DDRB |= (1 << PB0);      //Linear- IA
	DDRB |= (1 << PB1);		 //Linear- PWM
	DDRB |= (1 << PB3);		 //Linear- IB	
	PORTB &= ~(1 << PB3);    //LIB is low 
	PORTB &= ~(1 << PB0);	 //LIA is low 


	//Pivot
	DDRD |= (1 << PD5); 	//Pivot- PWM
	DDRD |= (1 << PD6); 	//Pivot- IA
	DDRD |= (1 << PD7); 	//Pivot- IB
	PORTB &= ~(1 << PD6);    //LIB is low 
	PORTB &= ~(1 << PD7);	 //LIA is low 

	//Set Up PWM for motor drivers
	//Linear Actuators
	TCCR1A = 0b10100000;
	TCCR1B = 0b00010001;		
	ICR1 = 400;					//20kHz- toggle on 400. 
	OCR1A = 320; 				//320/400 = 80 percent duty cycle

	//Pivot
	TCCR0A |= 0x20;		//OC0B is output (PB5)
	TCCR0A |= 0x03;		//fast PWM Mode
	TCCR0B |= 0x08;	
	TCCR0B |= 0x05;		//turn on
	OCR0A = 400;		//freq
	OCR0B = 15;		//duty cycle

	//Initial board status 
	blockInputs = 0;			//inputs ready to be received
	zTop = 0; 					//we don't know if we're at the top
	home = 0; 					//we don't know if we're home
	pivotStatus = 0; 			//we don't really know about this either (yet)
}

//rotate the bed
int rotation(int speed) {
	blockInputs = 1;			//raise flag
	//turn off linear actuator
	PORTB &= ~(1 << PB0);     //set LIA to low
	PORTB &= ~(1 << PB3);     //set LIB to low
	if(!pivotStatus) {
		//pivotStatus = 0- we're currently close to the patient
		//Turn on pivot motor if we're not away already
		PORTD &= ~(1 << PD6);     //set PIA to low
		if((PINC & 0x08) >> 0x03) {
			PORTD |= (1 << PD7);     //set PIB to high
			while ((PINC & 0x08) >> 0x03)
			PORTD &= ~(1 << PD7);     //set PIB to low
		}
	}
	else {
		//pivotStatus = 1- we're currently far from the patient
		//Turn on pivot motor if we're not close already 
		PORTD &= ~(1 << PD7);     //set PIB to low
		if((PINC & 0x10) >> 0x04) {
			PORTD |= (1 << PD6);     //set PIA to high
			while ((PINC & 0x10) >> 0x04)
			PORTD &= ~(1 << PD6);     //set PIB to low
		}
	}
	
	blockInputs = 0;			//clear flag
	pivotStatus = !pivotStatus; //opposite status now
}


//set the table to its home state
int homeSet(void) {
	//calling this drives the table as high as it can go, and rotatest it away from the human (status 0) 
	if(!home) {
		blockInputs = 1;				//raise flag
		if(!pivotStatus) {
			rotation(pivotStatus);
		}
		if(!zTop) {
			//Turn off pivot motor
			PORTD &= ~(1 << PD6);     //set PIA to low
			PORTD &= ~(1 << PD7);     //set PIB to low
			//drive up
			PORTB &= ~(1 << PB3);	  //set LIB to low
			PORTB |= (1 << PB0);	  //set LIA to high
			//time to drive to the top
			_delay_ms(linDelay);
			//done driving
			PORTB &= ~(1 << PB0);     //set LIA to low 
			zTop = 1;				  //congrats, you've now reached the top!!!
		}	
		home = 1;
		blockInputs = 0;				//clear flag						
	}
}

int main(void) {
	setup();
	homeSet();
	//turn of everything unless it is actively driven
	//Turn off pivot motor
	PORTD &= ~(1 << PD6);     //set PIA to low
	PORTD &= ~(1 << PD7);     //set PIB to low
	//turn off linear actuator
	PORTB &= ~(1 << PB0);     //set LIA to low
	PORTB &= ~(1 << PB3);     //set LIB to low

	while(1) {
		//z-up button
		if(!blockInputs & (!((PIND & 0x04) >> 0x02) | !(PINC & 0x01))) {
			if(!zTop) {
				blockInputs = 1;		  //raise flag
				//Turn off pivot motor
				PORTD &= ~(1 << PD6);     //set PIA to low
				PORTD &= ~(1 << PD7);     //set PIB to low
				//drive up
				PORTB &= ~(1 << PB3);	  //set LIB to low
				PORTB |= (1 << PB0);	  //set LIA to high
				//drive until there's no more input
				while(!((PIND & 0x04) >> 0x02) | !(PINC & 0x01)) {
				}
				//done driving
				PORTB &= ~(1 << PB0);     //set LIA to low 
				blockInputs = 0; 		  //clear the flag
			}
		}	

		//z-down button
		if(!blockInputs & (!((PIND & 0x08) >> 0x03) | !((PINC & 0x02) >> 0x01))) {
			blockInputs = 1;			//raise flag
			//Turn off pivot motor
			PORTD &= ~(1 << PD6);     //set PIA to low
			PORTD &= ~(1 << PD7);     //set PIB to low
			//drive up
			PORTB &= ~(1 << PB0);	  //set LIA to low
			PORTB |= (1 << PB3);	  //set LIB to high
			//drive until there's no more input
			while(!((PIND & 0x08) >> 0x03) | !((PINC & 0x02) >> 0x01)) {
			}
			//done driving
			PORTB &= ~(1 << PB3);     //set LIB to low 
			blockInputs = 0; 		  //clear the flag
			zTop = 0;				  //can't be at the top anymore
			home = 0;				  //can't be home anymore
		}
		
		//pivot button - need to take care of the rotation (maybe using delay.h?)
		if(!blockInputs & (!((PIND & 0x10) >> 0x04) | !((PINC & 0x04)>> 0x02))) {
			rotation(pivotStatus);
			if(pivotStatus & zTop) {
					home = 1;
			}
			else {
				home = 0;
			}		
		}
	}
}


