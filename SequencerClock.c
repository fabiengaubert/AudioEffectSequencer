#include "SequencerCLock.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "defines.h"
#include "Sequencer.h"
//==============================================================================================
//======================================== Clock ===============================================
//==============================================================================================


//======================================  Local variables  =====================================
/*
 * Timer method
 *
 * Many informations at point 16.7.(1 or 2) of Atmega2560's documentation
 *  
 * Normal mode / Clear timer on compare match mode (CTC)
 
 Get a the Timer control register
 TCCR0A
 
 Set the normal mode:
 Mode value:	WGM02:0 = 0 
 Timer counter: TCNT0
 Interrupts:	TOV0 (Timer overflow)
 
 Set the CTC mode:
 Mode value:			WGM02:0 = 2
 Counter resolution:	OCR0A
 Interrupts:			OCF0A (Occurs when TCNT0 reaches TOP (equivalent to OCR0A))
 
 Prescaler setting: (Atmega's doc, point 18)
 CSn2:0 to (1: 1, 8: 2, 64: 3, 256: 4, 1024: 5)
 */

 
enum PrescalerValue prescalerValue;

void TimerSetPrescaler(enum PrescalerValue value)
{
	prescalerValue=value;
	if (value == 8) 
	{
		TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10);
	} 
	else if (value == 64) 
	{
		TCCR1B |= (0<<CS12)|(1<<CS11)|(1<<CS10);
	} 
	else if (value == 256) 
	{
		TCCR1B |= (1<<CS12)|(0<<CS11)|(0<<CS10);
	} 
	else if (value == 1024) 
	{
		TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);
	}
}

void ClockInit()
{
	TimerSetPrescaler(D8);
	// Toggle on compare match
	TCCR1A |= (0 << COM1A0)|(1 << COM1A1);
	// Enable compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	// counter range = 62500
	
	OCR1AH  = 0xF4;
	OCR1AL	= 0x24;
	// Initialize the counter to 0
	TCNT1	= 0;
	// Set CTC mode
	TCCR1B |= 1 << WGM12;
}

void SetTempo(uint8_t tempo)
{
	int numTickTimer=(60.0/(tempo*PPQN))/((1.0/F_CPU)*prescalerValue);
	// MSB first!!
	OCR1AH  = numTickTimer>>8;
	OCR1AL	= numTickTimer;
}

ISR(TIMER1_COMPA_vect)
{
	SequencerTick();
	static uint16_t index=0;
	index++;
	if(index>=(PPQN/2))
	{
		index=0;
		PORTB ^= 1 << 7;    /* toggle the LED */
	}
}