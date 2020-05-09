#include "Pin.h"


/*
 *PIN Methods
 */

void PIN_Set(uint8_t pinNb, uint8_t pinDir) {
	uint8_t PA = 22;
	// Pin 22 -> PA0
	if ( pinNb >= PA && pinNb < ( PA + 8 ) ) {
		if(pinDir){//a tester
			DDRA |= 1 << (pinNb - PA);
		}
		else
		{
			DDRA &= ~(1 << (pinNb - PA));
		}
	}
}

char PIN_Read(uint8_t pinNb) {
	uint8_t PA = 22;
	if ( pinNb >= PA && pinNb < ( PA + 8 ) ) {
		uint8_t mask = 1 << (pinNb - PA);
		return (PINA & mask);
	}
	return -1;
}

void PIN_On(uint8_t pinNb) {
	uint8_t PA = 22;
	if ( pinNb >= PA && pinNb < ( PA + 8 ) ) {
		char pinDec = 1 << (pinNb - PA);
		PORTA |= pinDec;
	}
}

void PIN_Off(uint8_t pinNb) {
	uint8_t PA = 22;
	if ( pinNb >= PA && pinNb < ( PA + 8 ) ) {
		char pinDec = ~(1 << (pinNb - PA));
		PORTA = PORTA & pinDec;
	}
}