#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

void init_pwm(void);

/* globals used by interrupt service routine */
int step_val=0;       //step brightness from 0 to 100 percent
int compare_val=0;    //value for PWM
char int_counter=0;   //counts interruts
char increment=1;     //set to +1 when getting brighter, -1 when getting dimmer

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

//old atmega8 code TIMSK=1<<TOIE1; //TOV1 interrupt
 TIMSK1=1<<TOIE1; 
 sei();

 while(1) ;//stay here forever still service interrupts


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



/*************************************************************************************************************************/
/* Timer1 overflow interrupt service routine                                                                             */
/* every 5 overflows this changes the PWM compare value, and therfore the brightness of the LED.  step_val contains      */
/* the percent of full scale from 0 to 100.  compare_val contains the 16 bit value between 0 and 10000 used for the      */
/* PWM compare.                                                                                                          */
/*************************************************************************************************************************/
ISR(TIMER1_OVF_vect)
{
 int_counter++;          //counts interrupts
 if(int_counter>0)       //change brightness every 5
 {
   int_counter=0;        //after 5, start back at 0

   if(step_val>=100) increment = -1;
   if(step_val<=0) increment = 1;

   step_val+=increment;
   compare_val=step_val*step_val;  //rms, not linear

   OCR1A=compare_val;
 }
}
