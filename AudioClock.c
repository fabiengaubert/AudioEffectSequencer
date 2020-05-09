#include <avr/io.h>
#include <avr/interrupt.h>

#include "AudioClock.h"
#include "AudioCodec.h"

//==============================================================================================
//====================================== AudioTimer ============================================
//==============================================================================================

int16_t buffer1[AUDIO_BUFF_SIZE];
int16_t buffer2[AUDIO_BUFF_SIZE]={2048};
int16_t buffer3[AUDIO_BUFF_SIZE]={2048};

volatile int16_t *ADCBuffer, *procBuffer, *DACBuffer;
volatile uint8_t dataReady=0;
uint8_t sampleIndex=0;

//TIMER 2 car plus haute priorité
//output master clock to drive the codec
void AudioClockInit()
{
	ADCBuffer=buffer1;
	procBuffer=buffer2;
	DACBuffer=buffer3;
	//compare output mode => Clear Timer on Compare or CTC mode (WGM22:0 = 2)
	TCCR2A=(0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)|(1<<WGM21)|(0<<WGM20);
	//prescaler 8, F=2Mhz, T=0.5us
	TCCR2B=(0<<WGM22)|(0<<CS22)|(1<<CS21)|(0<<CS20);
	//initialisation value
	TCNT2=0x00;
	//value to compare to, 44.100kHz -> T=22.675us
	//OCR2A=45;
	//value to compare to, 32kHz -> T=31.675us
	OCR2A=63;
	//interruptions enable
	TIMSK2=0x02;
	//to try
	//PORTB|=(1<<DDB4);
}

ISR(TIMER2_COMPA_vect)
{
	ADCOutput(ADCBuffer+sampleIndex);
	DACOutput(*(DACBuffer+sampleIndex));
	
	sampleIndex++;
	if(sampleIndex>=AUDIO_BUFF_SIZE){
		sampleIndex=0;
		// Debugging: look if dataReady==0, to assure processing is over
		dataReady=1;
		// Pas besoin de variable temporaire, DACBuffer peut être écrasé
		volatile int16_t* tempBuffer=ADCBuffer;
		ADCBuffer=DACBuffer;
		DACBuffer=procBuffer;
		procBuffer=tempBuffer;
	}
}
