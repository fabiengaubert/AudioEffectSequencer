#ifndef PIN_H
#define PIN_H

#include <avr/interrupt.h>
#include <avr/io.h>

#define PIN_INPUT 0
#define PIN_OUTPUT 1

void PIN_Set(uint8_t pinNb, uint8_t pinDir);
char PIN_Read(uint8_t pinNb);
void PIN_On(uint8_t pinNb);
void PIN_Off(uint8_t pinNb);

#endif