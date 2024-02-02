/*
 *  Mini_Project2 (Stop Watch)
 *  Created on: 29/1/2024
 *  Author: Nada Waleed
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/****************************Global variable*****************************/
unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;
unsigned char Interrupt_Flag = 0;

/*******************Function prototypes *******************/
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void Timer1_CTC_Init(void);
void StopWatch(void);
void StopWatch_TimeCounting(void);

/************************ISR Functions*********************/
ISR(INT0_vect)
{
	second = 0;
	minute = 0;
	hour = 0;            //reset all stop watch digits
	PORTC &= 0xF0;       // clear 7-segment

}
ISR(INT1_vect)
{
	/* Configure timer control register TCCR1B:
	 * No clock CS10=0 CS11=0 CS12=0
	 */
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS12);
}
ISR(INT2_vect)
{
	/* Configure timer control register TCCR1B:
     * Clock ON again CS10=1 CS11=0 CS12=1
     */

		if (!(TCCR1B &(1<<CS10)))  // Check if bit (CS10) is cleared or not
		{
			TCCR1B |= (1 << CS10);
		}

		if (!(TCCR1B &(1<<CS12)))  // Check if bit (CS12) is cleared or not
		{
			TCCR1B |= (1 << CS12);
		}
}
ISR(TIMER1_COMPA_vect)
{
	Interrupt_Flag = 1;
}

/***********************Main Function****************************/
int main(){

	    DDRC |= 0x0F; // Set 7447 decoder input pins as output

	    DDRA |= 0x3F; // Set six 7-segment as output

	    DDRD &= ~(1 << PD2); //Set INT0 pin as input
	    PORTD |= (1 << PD2); //internal pull-up enabled

	    DDRD &= ~(1 << PD3);   //Set INT1 pin as input
	    PORTD &= ~(1 << PD3);  //internal pull-up enabled

	    DDRB &= ~(1 << PB2);   //Set INT2 pin as input
	    PORTB |= (1 << PB2);   //internal pull-up enabled

	    Timer1_CTC_Init();    // TIMER1 Compare mode

	    INT0_Init();          // INT0 RESET interrupt
	    INT1_Init();          // INT1 PAUSE interrupt
	    INT2_Init();          // INT2 RESUME interrupt

	    while (1)
	    	{
	    		StopWatch();

	    		if (Interrupt_Flag == 1)
	    		{
	    			StopWatch_TimeCounting();

	    			Interrupt_Flag = 0;     /* Clear flag to be Set when timer interrupt is triggered again */
	    			_delay_us (.2);
	    		}
	    	}
	    	return 0;
}

/**********************Function Definition***************/
/*INT0*/
void INT0_Init(void){
	MCUCR &=~(1<<ISC00);// Trigger INT0 with the falling edge
	MCUCR |=(1<<ISC01); // Trigger INT0 with the falling edge
	GICR  |= (1<<INT0); // Enable external interrupt pin INT0
	SREG |=(1<<7);      // Enable interrupts by setting I-bit
}
/*INT1*/
void INT1_Init(void){
	MCUCR |= (1<<ISC11)|(1<<ISC10); // Trigger INT1 with the rising edge
	GICR  |= (1<<INT1);             // Enable external interrupt pin INT1
	SREG  |= (1<<7);                // Enable interrupts by setting I-bit
}
/*INT2*/
void INT2_Init(void){
	MCUCSR &= ~(1<<ISC2);     // Trigger INT2 with the falling edge
	GICR   |= (1<<INT2);	  // Enable external interrupt pin INT2
	SREG   |= (1<<7);         // Enable interrupts by setting I-bit
}
/*Timer1 CTC mode*/
void Timer1_CTC_Init(void)
{
	TCNT1 = 0;              //timer1 initial count to zero
	OCR1A = 977;            // compare value 977 ( To tigger an interrupt each second ) */
	TIMSK = (1 << OCIE1A);  // enable timer1 compare interrupt
	TCCR1A = (1 << FOC1A) | (1 << COM1A1);
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
}
void StopWatch(void)
{
	unsigned char count = 0;
	unsigned int timeDigits[] = {(second % 10),(second / 10),
			                (minute % 10),(minute / 10),
			                (hour % 10),(hour / 10)};      //array that contains the stop watch digits

	for(count = 0; count < 6; count++)
	{
		PORTA = (1 << count);        //enable all 7-Segments
		PORTC = (PORTC & 0xF0) | timeDigits[count];

		_delay_us (.2);    //delay between each 7-segment enable to make the stop watch display looks normal
	}
}

void StopWatch_TimeCounting(void)
{
	if (second == 59) // check if seconds exceeded the range (0 --> 59) or not
	{
		second = 0;       //clear seconds if exceeded */
		minute++;         // increment minute
	}
	else
		second++;         //increment seconds

	if (minute > 59)
	{
		minute = 0;
		hour++;
	}

	if (hour > 23)
	{
		hour = 0;
	}
}


