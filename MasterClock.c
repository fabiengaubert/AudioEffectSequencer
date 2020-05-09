#include <avr/io.h>
//==============================================================================================
//================================== MasterClockOutput =========================================
//==============================================================================================


//output master clock to drive the codec
void MasterClockOutputInit()
{
	//compare output mode => Clear Timer on Compare or CTC mode (WGM02:0 = 2)
	TCCR0A=(0<<COM0A1)|(0<<COM0A0)|(0<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(0<<WGM00);
	//for prescaler 1024
	//TCCR0B=(0<<WGM02)|(1<<CS02)|(0<<CS01)|(1<<CS00);
	//for 8Mhz clock
	TCCR0B=(0<<WGM02)|(0<<CS02)|(0<<CS01)|(1<<CS00);
	//initialisation value
	TCNT0=0;
	//value to compare to, 0 for Fclk/2 Register is used to manipulate the counter resolution
	OCR0B=0;
	//no interruptions
	TIMSK0=0;
	//set DDR as output in last
	DDRG|=(1<<DDG5); //pin4 arduino
}
