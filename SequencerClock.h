//==============================================================================================
//======================================== Clock ===============================================
//==============================================================================================
 #ifndef SEQUENCERCLOCK_H
#define SEQUENCERCLOCK_H

#include <avr/io.h>
#include "defines.h"

//======================================  Local variables  =====================================
enum PrescalerValue
{
	D8 = 8,
	D64 = 64,
	D256 = 256,
	D1024 = 1024
};

void TimerSetPrescaler(enum PrescalerValue value);
void ClockInit(void);
void SetTempo(uint8_t tempo);

#endif