//==============================================================================================
//===================================== AudioEffetcs ===========================================
//==============================================================================================
#include "AudioEffects.h"
#include "AudioClock.h"

volatile int16_t mask=0xFFFF;
volatile uint8_t paramStep=127;
uint16_t temp;
uint8_t step=0;

void AudioProcessing()
{
	if(dataReady==1)
	{
		//DebugHigh();
		
		for(uint8_t i=0; i<AUDIO_BUFF_SIZE; i++)
		{
			//TRY IT!!!!
			//*(procBuffer+i)-=2048;
		}
		
		// ======================================
		
		for(uint8_t i=0; i<AUDIO_BUFF_SIZE; i++)
		{
			if(step<127)
			{
				*(procBuffer+i)=temp;
			}
			else
			{
				step-=127;
			}
			step+=paramStep;
			temp=*(procBuffer+i);
			*(procBuffer+i)&=mask;
		}
		
		
		// ======================================
		for(uint8_t i=0; i<AUDIO_BUFF_SIZE; i++)
		{
			//*(procBuffer+i)+=2048;
		}
		dataReady=0;
		//DebugLow();
	}
}

void SetParam1(uint8_t param){
	// pour mapper [0-127] des paramètres sur [0-12], nombre de bits masqués
	uint8_t numBits=param*12/127;
	// initialisation du mask à sa valeur maximale, seulement le douzième bit est conservé=é
	mask=0xF800;
	for(uint8_t i=0; i<numBits; i++)
	{
		mask=(mask>>1);
	}
}

void SetParam2(uint8_t param){
	paramStep=param;
}