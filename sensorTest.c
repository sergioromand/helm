#include <avr/io.h>
#include "uart.h"
#include <avr/interrupt.h>

int voltage;

void startConversion() {
	ADCSRA |= (1 << ADSC);
} 

void setupADC() {
	ADMUX |= (1 << REFS0); 
	ADMUX |= (1 << MUX0); 
	ADMUX |= (1 << MUX2); 
	ADCSRA |= (1 << ADEN); 
	ADCSRA |= (1 << ADIE); 
	ADCSRA |=  (1 << ADPS0); 
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);
	startConversion(); 
}

int main() {
	uart_init(); 
	setupADC(); 
	sei();
	while (1) {
		printf("%d\n", voltage);
	}
}

ISR(ADC_vect) {
	voltage = ADC; 
	startConversion();
}