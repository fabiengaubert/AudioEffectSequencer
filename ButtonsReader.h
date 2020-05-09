#ifndef BUTTONSREADER_H
#define BUTTONSREADER_H

#include <avr/io.h>

#define SR_LOAD_PIN 22
#define SR_CLOCK_PIN 23
#define SR_OUTPUT_PIN 24

#define BUTTON_ST 7
#define BUTTON_REC 6
#define BUTTON_RESET 5
#define SR_MAX_BUTTONS 8

struct inputsState {
	uint8_t btnToggled:1;
};

void ButtonsReader_Init(void);
uint8_t ButtonsReader_Read(uint8_t buttonNumber);
void ButtonsReader_Store(uint8_t buttonNumber, uint8_t value);
void ButtonsReader_Load(void);

#endif