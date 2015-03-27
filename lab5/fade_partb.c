#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define POT_CH 0

void init_pwm(void);
void init_adc(void);
uint16_t read_adc(uint8_t channel);

/* globals used by interrupt service routine */
int brightness=100;    // brightness that will be set by ADC

/************************************************************************************************************************/
/* This program demonstrates PWM on an AVR.  It controls the brightness of an LED driven my the OC1A output.            */
/* The function init_pwm performs the necessary initialization  for the PWM.  After this, timer1 interrupts are enabled */
/* and everything else happens in the timer1 overflow interrupt.                                                        */
/* The program causes the apparent brightness of an LED to smoothly increase from fully off to fully on and back to     */
/* fully off.  There are 100 brightness levels in addition to fully off.  The PWM period is adjusted as the square of   */
/* the brightness to have the RMS current change rather than the average current.                                       */
/************************************************************************************************************************/

int main(void) 
{
  init_pwm();
  init_adc();

  //old atmega8 code TIMSK=1<<TOIE1; //TOV1 interrupt
  TIMSK1=1<<TOIE1; 
  sei();

  while(1) {//stay here forever still service interrupts
    brightness = read_adc(POT_CH)/10;    // Read from the ADC and scale value
  }

  return 0;  //never get here
}


/* function init_pwm                                                                                                     */
/* sets up pwm using timer one.  Output is on OC1A to control the brightness of an LED.  Brightnes ranges from 0 to 100% */
/* actual compare values are squares 0, 1, 4, 9, 16...10000 to give a smoothly changing RMS (not average) output         */

void init_pwm(void)
{
  DDRB=1<<PB1;   //make RB1 (OCR1A) an output
  OCR1A=0;    //set initial compare at 0 (off)

  //Output compare OC1A 8 bit non inverted PWM
  TCCR1A=0xc0;  //inverted on A, nothing on B PWM mode 8

  //start timer without prescaler
  TCCR1B=0x11; //internal clock, no prescaler , PWM mode 8

  ICR1=10000;  //brightness levels from 0-100 control rms (steps are squares)
}


/* Function to set up the ADC for conversions */
void init_adc(void)
{
  ADMUX = (1 << REFS0);                         // For Aref=AVcc;
  ADCSRA = (1 << ADEN)|(1 << ADPS2); //Rrescalar div factor =128
}


/* Function to read from the ADC */
uint16_t read_adc(uint8_t channel)
{
  //Select ADC Channel ch must be 0-7
  channel = channel & 0b00000111;
  ADMUX &= ~(0b00000111);
  ADMUX |= channel;

  //Start Single conversion
  ADCSRA|=(1<<ADSC);

  //Wait for conversion to complete
  while(!(ADCSRA & (1 << ADIF)));

  //Clear ADIF by writing one to it
  //Note you may be wondering why we have write one to clear it
  //This is standard way of clearing bits in io as said in datasheets.
  //The code writes '1' but it result in setting bit to '0' !!!

  ADCSRA |= (1 << ADIF);

  return (ADCH<<8) | ADCL;
}




/* Timer1 overflow interrupt service routine
 * Updates output compare value to adjust brightness
 */

ISR(TIMER1_OVF_vect)
{
    OCR1A=brightness*brightness;
}
