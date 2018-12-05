#include <avr/io.h>

int main(void) {
DDRD |= 0x20;
TCCR0A |= 0x20;
TCCR0A |= 0x03;
TCCR0B |= 0x08;
TCCR0B |= 0x05;
OCR0A = 400;
OCR0B = 320;
while(1); 	
}