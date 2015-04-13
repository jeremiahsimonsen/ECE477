#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 1000000UL
#include <util/delay.h>

int serial_putchar(char, FILE *);
static FILE serial_stream = FDEV_SETUP_STREAM (serial_putchar, NULL, _FDEV_SETUP_RW);

void init_serial(void);
void init_ADC(void);
int read_ADC(void);

int main(void)
{
	int temp = 0;

	init_ADC();
	init_serial();

	FILE *fp;
	fp = stdout;

	while (1) {
		temp = read_ADC();
		fprintf(fp, "%d\n\r", temp);
		_delay_ms(100);
	}

	return 0;
}

/* Initializes AVR USART for 1200 baud (assuming 1MHz clock) */
/* 1MHz/(16*(51+1)) = 1202 about 0.2% error                  */
void init_serial(void)
{
   UBRR0H=0;
   UBRR0L=12; // 9600 BAUD FOR 1MHZ SYSTEM CLOCK
   UCSR0A= 1<<U2X0;
   UCSR0C= (1<<USBS0)|(3<<UCSZ00) ;  // 8 BIT NO PARITY 2 STOP
   UCSR0B=(1<<TXEN0)  ; //ENABLE TX
   // stdin=&serial_stream;
   stdout=&serial_stream;

}

//simplest possible putchar, waits until UDR is empty and puts character
int serial_putchar(char val, FILE * fp)
{
  while((UCSR0A&(1<<UDRE0)) == 0); //wait until empty 
   UDR0 = val;
   return 0;
}

void init_ADC(void)
{
	ADMUX = (3<<REFS0) | (1<<MUX3);  //I belive this sets the temp sensor check page 247 for more info. Also sets the internal 1.1V reference.
	ADCSRA = (1<<ADEN) | (1<<ADIE);  //enable the ADC
} 

int read_ADC(void)
{
	int retval = 0;

	// Start conversion
	ADCSRA |= (1<<ADSC);

	// Wait until conversion is finished
	while (!(ADCSRA & (1<<ADSC)));

	retval = ADCL;
	retval |= ADCH<<8;

	return retval;
}
