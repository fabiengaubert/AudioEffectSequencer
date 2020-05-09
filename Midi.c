#include "MIDI.h"

void MIDIInit(void)
{
	// Doc Atmega2560 USART 22.4 p210
	//baudrate configuration
	UBRR1H = (MIDI_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
	UBRR1L = MIDI_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	
	// Enable receiver and transmitter */
	UCSR1B |= (1 << RXEN1)|(1 << TXEN1);
	// Set frame format: 1stop bit , 8data,  */
	UCSR1C |= (0 << USBS1)|(3 << UCSZ10);
	
	UCSR1B |= 1 << RXCIE1;
}

// MIDI Receveive message
ISR(USART1_RX_vect) 
{
	uint8_t midiData = UDR1;
	char bufferMidi[12];
	switch (midiData) 
	{
			// Start
		case 0xFA:
			sprintf(bufferMidi, "MIDI Start\n");
			USART_TransmitString(bufferMidi);
			SequencerStart();
			break;
			
			// Stop
		case 0xFC:
			/*
			 Notice: when midi is playing and play button is pressed a STOP message comes.
			 We do not reinitialize the beat counter:
			 - if play pressed, a CONTINUE message comes and we start at 0
			 - else we'll use our saved index
			 */
			sprintf(bufferMidi, "MIDI Stop\n");
			USART_TransmitString(bufferMidi);
			SequencerStop();
			break;
			
			// Continue
		case 0xFB:
			sprintf(bufferMidi, "MIDI Cont\n");
			USART_TransmitString(bufferMidi);
			SequencerStart();
			break;
			
			// Clock
		case 0xF8:
			/*char buffer2[12];
			 sprintf(buffer2, "MIDI Clock\n");
			 USART_TransmitString(buffer2);*/
			break;
			
			// CC
		case 0xB0:
			/*char buffer2[12];
			 sprintf(buffer2, "MIDI CC\n");
			 USART_TransmitString(buffer2);*/
			
			/*
			 We have to control next messages
			 A State machine would be great
			 */
			break;
	}
}

void MIDISendByte(unsigned char byte) 
{
	/* Wait for empty transmit buffer */
	while (!( UCSR1A & (1<<UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = byte;
}

// MIDI Send Message
// taken from datasheet
void MIDISendCC(uint8_t channel, uint8_t controller, uint8_t value) 
{
	// channel should be between 1 to 16 and begin by 0xB0
	channel = 0xB0 | (0x0F & (channel - 1));
	// Send status message
	MIDISendByte(channel);
	
	// controller should be between 0 to 127
	// Send controller message
	MIDISendByte(controller);
	
	// controller should be between 0 to 127
	// Send controller message
	MIDISendByte(value);
}
