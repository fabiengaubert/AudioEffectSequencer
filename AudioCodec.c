#include "AudioCodec.h"

#include <avr/io.h>
//==============================================================================================
//========================================= SPI ===============================================
//==============================================================================================


//output master clock to drive the codec
void SPIInit()
{
	PORTB |= (1<<PB4)|(1<<PB0);//ADC ~CS and DAC ~CS must be initially high
	//set MOSI, SDK, ~ADC_CS and ~DAC_CS to outputs
	DDRB |= (1<<DDB4)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);
	// Enable SPI, Master, Prescaler 8 => SCK=2MHz (with SPI2X activated)
	//SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(1<<SPR0);
	// Enable SPI, Master, Prescaler 4 => SCK=4MHz (with SPI2X activated), ADC sample time < 6us, DAC conversion time < 4us
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X);
}

void DACOutput(int16_t outSample)
{
	outSample+=2048;
	//set ~CS low to initialize communication
	PORTB &= ~(1<<PB0);
	//prepare data
	unsigned char MSBdata, LSBdata;
	MSBdata = ((outSample>>8)&0x00FF)|0x30; //4MSB bits + configuration: gain=1, SHDN=1, mask not necessary
	//transmit MSB
	SPDR = MSBdata;
	//prepare next data
	LSBdata = outSample&0xFF;
	//Wait for the first data to be transmitted
	while(!(SPSR & (1<<SPIF)));
	//transmit LSB
	SPDR = LSBdata;
	while(!(SPSR & (1<<SPIF)));
	//end of communication, ~CS is high
	PORTB |= (1<<PB0);
}

void ADCOutput(volatile int16_t* sample)
{
	int16_t inSample;
	//set ~CS low to initialize communication
	PORTB &= ~(1<<PB4);
	//transmit ADC configuration
	//SPDR = 0x04; //start bit, differential p.21 datasheet
	SPDR = 0x05;
	//Wait for the first data to be transmitted
	while(!(SPSR & (1<<SPIF)));
	//ADC configuration 2nd part
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	inSample = (SPDR&0x0F)<<8;
	// useless byte, to receive data
	SPDR = 0x00;//peut on le faire avant l'enregistrement de la donnée??
	while(!(SPSR & (1<<SPIF)));
	inSample |= SPDR;
	//end of communication, ~CS is high
	PORTB |= (1<<PB4);
	//remove offset for audio processing
	*sample=inSample-2048;
}
