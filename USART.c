#include "USART.h"

//==============================================================================================
//============================================ USART ===========================================
void USART_Init()
{
	//baudrate configuration
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	
	// Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_CS   PB0
#define CE 24
#define PE 23
inline unsigned char SPI_WriteRead(unsigned char dataout)
{
	unsigned char datain;
	_delay_ms(1);
	PIN_Off(CE);
	// Start transmission (MOSI)
	SPDR = dataout;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Get return Value;
	datain = SPDR;
	// Return Serial In Value (MISO)
	return datain;
}

void USART_InitSPIMode()
{
	/* Set MOSI and SCK output, all others input */
	// MOSI = PB2 - SCK = PB1
	char buffer[20];
	sprintf(buffer, "InitSPI\n");
	USART_TransmitString(buffer);
	
	// Initial the AVR ATMega168 SPI Peripheral
	// Set MOSI and SCK as output, others as input
	SPI_DDR = (1<<PB0)|(1<<PB1)|(1<<PB2);//FAUX, FAUX, c'est pas SPI_DDR (nom générique) qui doit etre utilisé, mais PORTB (voir datasheet)
	// Latch Disable (RCK Low)
	SPI_PORT &= ~(1<<SPI_CS);
	// Enable SPI, Master, set clock rate fck/2 (maximum)
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
	// Reset the 74HC595 register
	
	PIN_On(PE);
	_delay_ms(1);
	PIN_Off(CE);
	//SPI_WriteRead(0);
	
	for(;;) {
		PIN_Off(PE);
		_delay_ms(10);
		PIN_On(CE);
		_delay_ms(1);
		PIN_On(PE);
		
		char data = SPI_WriteRead(0);
		
		sprintf(buffer, "SPIData: %d\n", data);
		USART_TransmitString(buffer);
		
		_delay_ms(200);
	}
	/*
	UBRR2 = 0;
	// Setting the XCKn port pin as output, enables master mode. 
	DDRH |= (1<<PH2);
	// Enable receiver and transmitter. 
	UCSR2B = (1<<RXEN2)|(0<<TXEN2);
	// Set MSPI mode of operation and SPI data mode 0. 
	UCSR2C = (1<<UMSEL21)|(1<<UMSEL20);//|(0<<UCSZ20)|(0<<UCPOL2);
	// Set baud rate. 
	// IMPORTANT: The Baud Rate must be set after the transmitter is enabled 
	//baudrate configuration
	UBRR2H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
	UBRR2L = BAUD_PRESCALE;*/
}

// taken from datasheet
void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

// the string must be ended by a '\n' or a '\0'
void USART_TransmitString(char* data)
{
	uint16_t i = 0;
	while ( data[i]!='\n' && data[i]!='\0'){
		USART_Transmit(data[i]);
		i++;
	}
	USART_Transmit('\n');
	/* Put data into buffer, sends the data */
}

// MIDI Receveive message
/*ISR(USART2_RX_vect) 
{
	uint8_t inputsData = UDR2;
	char buffer2[30];
	sprintf(buffer2, "ShiftRegister: %d\n", inputsData);
	USART_TransmitString(buffer2);
}*/
