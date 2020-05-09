#include "ButtonsReader.h"
#include "Pin.h"
#include "Sequencer.h"

uint8_t inputsValues = 0;
struct inputsState inputToggle[SR_MAX_BUTTONS];

inline static void ButtonsReader_StartTimerInt(void);
inline static void ButtonsReader_StopTimerInt(void);

void ButtonsReader_Init() {
	PIN_Set(SR_LOAD_PIN, PIN_OUTPUT);
	PIN_Set(SR_CLOCK_PIN, PIN_OUTPUT);
	PIN_Set(SR_OUTPUT_PIN, PIN_INPUT);
	
	PIN_On(SR_LOAD_PIN);
	PIN_Off(SR_CLOCK_PIN);
	
	// prescaler to 8
	TCCR3B |= (0<<CS32)|(1<<CS31)|(0<<CS30);
	// Toggle on compare match
	TCCR3A |= (0 << COM3A0)|(1 << COM3A1);
	// Enable compare interrupt
	TIMSK3 |= (1 << OCIE3A);
	// counter range = 10000
	
	OCR3AH  = 0x27;
	OCR3AL	= 0x10;
	// Initialize the counter to 0
	TCNT3	= 0;
	// Set CTC mode
	TCCR3B |= 1 << WGM32;
}

inline static void ButtonsReader_StartTimerInt() {
	// Enable compare interrupt
	TIMSK3 |= (1 << OCIE3A);
}

inline static void ButtonsReader_StopTimerInt() {
	// Enable compare interrupt
	TIMSK3 |= (0 << OCIE3A);
}

uint8_t ButtonsReader_Read(uint8_t buttonNumber) {
	uint8_t mask = 1 << buttonNumber;
	return (inputsValues & mask) >> buttonNumber;
}

void ButtonsReader_Store(uint8_t buttonNumber, uint8_t value) {
	if (value > 0)
	{
		inputsValues |= (1 << buttonNumber);
	}
	else
	{
		uint8_t mask = ~(1 << buttonNumber);
		inputsValues &= mask;
	}
}

ISR(TIMER3_COMPA_vect)
{
	ButtonsReader_StartTimerInt();
	// Store parallel inputs values in the shift register
	PIN_Off(SR_LOAD_PIN);
	// Inhibit parallel inputs loading 
	PIN_On(SR_LOAD_PIN);
	// Get values
	uint8_t buttonId = 0;
	for (buttonId = 0; buttonId < SR_MAX_BUTTONS; buttonId++) {
		// We got value at the pin, just read and store it
		uint8_t pinValue = PIN_Read(SR_OUTPUT_PIN);
		if (pinValue > 0) {
			pinValue = 1;
		}
		if (ButtonsReader_Read(buttonId) != pinValue) {
			ButtonsReader_Store(buttonId, pinValue);
			if (inputToggle[buttonId].btnToggled == 1) {
				inputToggle[buttonId].btnToggled = 0;
				/*
				 Here we will change corresponding button flag
				 */
				switch (buttonId) {
					case BUTTON_ST:
						sequencerSignals.PLAYING_START_STOP = 1;
						break;
					case BUTTON_REC:
						sequencerSignals.RECORDING_START_STOP = 1;
						break;
					case BUTTON_RESET:
						sequencerSignals.ERASE = 1;
						break;
				}
			} else {
				inputToggle[buttonId].btnToggled = 1;
			}
		}
		// We need a positive going edge to have the next value at the SR_OUTPUT_PIN
		PIN_On(SR_CLOCK_PIN);
		PIN_Off(SR_CLOCK_PIN);
	}
	ButtonsReader_StartTimerInt();
}

void ButtonsReader_Load() {
	
}