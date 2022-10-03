/*
 * project2.c
 *
 *  Created on: Sep 15, 2022
 *      Author: hp 22
 */
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char seconds=0;
unsigned char minutes=0;
unsigned char hours=0;

ISR(TIMER1_COMPA_vect)			// Interrupt Service Routine for timer1 compare mode channel A
{
	seconds++;
	if(60==seconds){
		minutes++;
		seconds=0;
	}
	if (60==minutes)
	{
		hours++;
		minutes=0;
	}
}

/* External INT0 Interrupt Service Routine with push buttom to make 
 * stopwatch start from zero*/
ISR(INT0_vect)
{
	seconds=0;
	minutes=0;
	hours=0;
}

// External INT1 Interrupt Service Routine
ISR(INT1_vect)
{
	// Pause the stop watch by disable the timer
	// Clear the timer clock bits (CS10=0 CS11=0 CS12=0) to stop the timer clock.
	TCCR1B &=~(1<<CS10) ;
	TCCR1B &=~(1<<CS12) ;
}

// External INT2 Interrupt Service Routine
ISR(INT2_vect)
{
	// resume the stop watch by enable the timer through the clock bits.
	TCCR1B |=(1<<CS10) ;
	TCCR1B |=(1<<CS12) ;
}

void timer1_init(void){
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);				//set A channel
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);	//set prescaler and mode
	/*
		 * 4. Clock = F_CPU/1024 CS10=1 CS11=0 CS12=1
	*/
	TCNT1=0;										//counter
	OCR1A=1000;										//compare mode
	TIMSK|=(1<<OCIE1A);								//interrupt enable
}

// External INT0 Enable and Configuration function
void int0_init(void){
	// Configure INT0/PD2 as input pin
	DDRD&=~(1<<2);
	// Enable internal pull up resistor at INT0/PD2 pin
	PORTD|=(1<<2);
	// Trigger INT0 with the falling edge
	MCUCR|=(1<<ISC01);
	// Enable external interrupt pin INT0
	GICR|=(1<<INT0);
}

// External INT1 Enable and Configuration function
void int1_init(void){
	// Configure INT1/PD3 as input pin
	DDRD&=~(1<<3);
	// Trigger INT1 with the raising edge
	MCUCR|=(1<<ISC11)|(1<<ISC10);
	// Enable external interrupt pin INT1
	GICR|=(1<<INT1);
}

void int2_init(void){
	// Configure INT2/PB2 as input pin
	DDRB&=~(1<<2);
	// Enable internal pull up resistor at INT2/PB2 pin
	PORTB|=(1<<2);
	// Enable external interrupt pin INT2
	GICR|=(1<<INT2);
}

int main(){

	DDRC|=0X0F;				//set decoder pins output
	DDRA|=0X3F;				//set  7segment pins output
	PORTC&=0XF0;			//zero decoder output
	PORTA&=0XC0;			//zero 7 segments output
	SREG  |= (1<<7);		//set general interupt
	int0_init();			
	int1_init();
	int2_init();
	timer1_init();

	while(1){
		PORTA = (PORTA & 0xC0) | (1<<0);					//print in first segment
		PORTC = (PORTC & 0XF0)|(seconds%10);				//print ones digit of seconds
		_delay_ms(3);

		PORTA = (PORTA & 0xC0) | (1<<1);					//print in second segment
		PORTC = (PORTC & 0XF0)|(seconds/10);				//print tens digit of seconds
		_delay_ms(3);

		PORTA = (PORTA & 0xC0) | (1<<2);					////print in third segment
		PORTC = (PORTC & 0XF0)|(minutes%10);				//print ones digit of minutes
		_delay_ms(3);

		PORTA = (PORTA & 0xC0) | (1<<3);					//print in fourth segment
		PORTC = (PORTC & 0XF0)|(minutes/10);				//print tens digit of minutes
		_delay_ms(3);

		PORTA = (PORTA & 0xC0) | (1<<4);					//print in fifth segment
		PORTC = (PORTC & 0XF0)|(hours%10);					//print ones digit of hours
		_delay_ms(3);

		PORTA = (PORTA & 0xC0) | (1<<5);					//print in sixth segment
		PORTC = (PORTC & 0XF0)|(hours/10);					//print tens digit of hours
		_delay_ms(3);
	}
}
