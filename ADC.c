#include "ADC.h"

#include <avr/interrupt.h>

//==============================================================================================
//========================================= ADC ================================================
//==============================================================================================

void ADCStart(void)
{
	//only AD0 is selected
	//ADLAR=1 result is left ajusted
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRB |= (0<<MUX5);
	// prescale division factor = 16, ADC clock = 1Mhz, 1 conversion = 13uS
	// ADC interrupts disabled
	ADCSRA |= (0<<ADIE)|(1<<ADPS2)|(0<<ADPS1)|(0<<ADPS0);
	//enable conversion, start ADC clock (doing after initialisations)
	ADCSRA |= (1<<ADEN);
}

//not tested useless?
void ADCStop(void)
{
	//disable conversion, stop ADC clock
	ADCSRA |= (0<<ADEN);
}

// blocking acquisition
uint8_t ADCRead(void)
{
	//if there is no current conversion
	//necessary???????
	while(ADCSRA&(1<<ADSC));
	//ask for next conversion
	ADCSRA |= (1<<ADSC);
	//wait for the end of the conversion
	while(ADCSRA&(1<<ADSC));
	//local variable needed??
	uint8_t potValue = ADCH;
	return potValue;
}

uint8_t ADCRead7Bits(void)
{
	uint8_t potValue = ADCRead()>>1;
	return potValue;
}

void ADCChangeChannel(uint8_t channel)
{
	//if there is a conversion going on already
	while(ADCSRA&(1<<ADSC));
	//change channel
	ADMUX = (ADMUX&0xf8)|(channel&~0xf8);
}

//non-blocking acquisition, result will be given by an interruption
/*void ADCStartConversion()
{
	//if there is no current conversion
	while(ADCSRA&(1<<ADSC));
	//ask for next conversion
	ADCSRA |= (1<<ADSC);
}*/

/*ISR(ADC_vect)
{
	potValue = ADCH;
	unsigned char buffer[5];
	sprintf(buffer, "%d\n", potValue);
	USART_TransmitString(buffer);
}*/
