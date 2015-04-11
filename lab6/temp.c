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
UCSR0C = (1<<7)|(1<<3)|(3<<1);
}
