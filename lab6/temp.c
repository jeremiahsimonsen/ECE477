#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
//#define FOSC 1843200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#include <util/setbaud.h>
void UART_Init( void );
unsigned char ADC_Start_Conv(void);
void USART_Transmit(unsigned char);
//FILE usart0_str = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_RW);
void main( void )
{
	unsigned char data;
	USART_Init();
	TEMP_Init();
	while(1){
	data = ADC_Start_Conv();
	USART_Transmit(data);
	_delay_ms(1000);
	}
}


void USART_Init(void) //got this from the ATMEGA data sheet page 176
{
/* Set baud rate */
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
	UCSR0C = (3<<UCSZ00);  //(1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )  //may or may not need this
{	
	data = 'a';
/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
	UDR0 = data;  //send out the MS bits of the ADCH
}

unsigned char USART_Receive( void )  //may or may not need this
{
/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
;
/* Get and return received data from buffer */
	return UDR0;
}

void TEMP_Init( void )
{
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(1<<MUX3);  //I belive this sets the temp sensor check page 247 for more info. Also sets the internal 1.1V reference.
	ADCSRA = (1<<ADEN);  //enable the ADC
}

unsigned char ADC_Start_Conv( void )
{
	unsigned char data;
	ADCSRA = (1<<ADSC);  //Starts the conversion
	while ((ADCSRA & (1<<ADSC)) == 1);  //Wait for the conversion to finish
	data = ADCH;  //Grab the data from the two registers.
	return data;
}
