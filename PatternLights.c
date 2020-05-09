#include "PatternLights.h"

#include "Pin.h"

uint8_t PLBeat = 0;

void PatternLights_StoreOn() {
	// Get HIGH
	PIN_On(OutSR_SI_PIN);
	PIN_On(OutSR_SCK_PIN);
	PIN_Off(OutSR_SCK_PIN);
	PIN_On(OutSR_RCK_PIN);
	PIN_Off(OutSR_SI_PIN);
	PIN_Off(OutSR_RCK_PIN);
}

void PatternLights_StoreOff() {
	// Get HIGH
	PIN_On(OutSR_SCK_PIN);
	PIN_Off(OutSR_SCK_PIN);
	PIN_On(OutSR_RCK_PIN);
	PIN_Off(OutSR_RCK_PIN);
}

void PatternLights_Init() {
	PIN_Set(OutSR_SI_PIN, PIN_OUTPUT);
	PIN_Set(OutSR_SCK_PIN, PIN_OUTPUT);
	PIN_Set(OutSR_RCK_PIN, PIN_OUTPUT);
	PIN_Off(OutSR_SI_PIN);
	PIN_Off(OutSR_SCK_PIN);
	PIN_Off(OutSR_RCK_PIN);
	for (PLBeat = 0; PLBeat < OutSR_MAX_BEATS; PLBeat++) {
		PatternLights_StoreOff();
	}
	PLBeat = 0;
}

void PatternLights_Reset(uint8_t numBeat) {
	for (numBeat = 0; numBeat < OutSR_MAX_BEATS; numBeat++) {
		PatternLights_StoreOff();
	}
}

void PatternLights_NextStep() {
	if (PLBeat == 0) {
		PatternLights_StoreOn();
		PLBeat++;
	} else {
		PatternLights_StoreOff();
		PLBeat++;
		if (PLBeat == OutSR_MAX_BEATS) {
			PLBeat = 0;
		}
	}
}
