#ifndef PATTERNLIGHTS_H
#define PATTERNLIGHTS_H

#include <avr/io.h>

#define OutSR_SCK_PIN 26
#define OutSR_SI_PIN 27
#define OutSR_RCK_PIN 25
#define OutSR_MAX_BEATS 8 

extern uint8_t PLBeat;

void PatternLights_StoreOn(void);
void PatternLights_StoreOff(void);
void PatternLights_Init(void);
void PatternLights_Reset(uint8_t numBeat);
void PatternLights_NextStep(void);

#endif
