#include <avr/io.h>
#include "util/delay.h"

int setup(void) {
	DDRB |= (0x01);      //Linear- IA
	PORTB |= (0x01); 
}

int main(void) {
	setup(); 
	while(1); 
}