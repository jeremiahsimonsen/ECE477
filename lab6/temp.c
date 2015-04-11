#include <avr/io.h>
//#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define F_CPU 1843200// Clock Speed
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void UART_Init( unsigned int ubrr );

void main( void )
{
	USART_Init ( MYUBRR );
}



USART_Init( unsigned int ubrr) //got this from the ATMEGA data sheet page 135
{
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<4)|(1<<3); //4 = RXEN 3 = TXEN
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<7)|(1<<3)|(3<<1);  //(1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )  //may or may not need this
{
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) )
;
/* Put data into buffer, sends the data */
UDR0 = data;
}

unsigned char USART_Receive( void )  //may or may not need this
{
/* Wait for data to be received */
while ( !(UCSR0A & (1<<RXC0)) )
;
/* Get and return received data from buffer */
return UDR0;
}
