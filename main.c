#include "main.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "Pin.h"
#include "ADC.h"
#include "SequencerClock.h"
#include "PatternLights.h"
#include "Sequencer.h"
#include "ButtonsReader.h"
#include "AudioClock.h"
#include "AudioEffects.h"
#include "AudioCodec.h"

//==============================================================================================
int main(void)
{	
	//USART_Init();
	
	ADCStart();
	PIN_Set(LIGHT_PS, PIN_OUTPUT);
	PIN_Set(LIGHT_REC, PIN_OUTPUT);
	
	ButtonsReader_Init();
	PatternLights_Init();
	
	//MIDIInit();
	ADCStart();
	SequencerInit();
	ClockInit();
	SetTempo(120);
	
	SPIInit();
	AudioClockInit();
	
	//enable global interruptions
	sei();
	//DebugInit();
	
	//to try
	while(1){
		
		AudioProcessing();
		
		//_delay_ms(1);
		
	}
	return 0;
}