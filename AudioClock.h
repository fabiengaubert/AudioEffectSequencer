#ifndef AUDIOCLOCK_H
#define AUDIOCLOCK_H

#include <avr/io.h>

#define AUDIO_BUFF_SIZE 40
//==============================================================================================
//====================================== AudioTimer ============================================
//==============================================================================================
extern volatile int16_t *procBuffer;
extern volatile uint8_t dataReady;

void AudioClockInit(void);

#endif


