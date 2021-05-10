/*	Author: Evan Hsu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 8  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	Video Link: 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define c 554.37
#define b 493.88
#define a 440.00
#define d 587.33
#define e 659.26
#define r 0.0
#define six 150.0
#define eight 300.0
#define four 600.0

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;



	OCR1A = 125;


	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR (TIMER1_COMPA_vect) {

	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	 if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }

		else if (frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	 }
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

/*double c = 554.37;
double b = 493.88;
double a = 440.00;
double d = 587.33;
double e = 659.26;
double r = 0;

double six = 150;
double eight = six * 2;
double four = six * 4;
*/
unsigned char i = 0;
double notes[49] = {	c, b, c, r, c, r, c, b, c, r, b, c, r, 
			a, b, c, d, c, r, b, r, c, r, r, 
			c, b, c, r, c, r, d, c, b, r, e, c, r,
			c, b, c, d, c, r, a, r, c, r	
		};

double time[49] = {	
	six, six, six, six, six, six, six, six, six, six, six, six, eight, eight,
	six, six, six, six, six, six, six, six, eight, eight, four,
	six, six, six, six, six, six, six, six, six, six, six, six, eight, eight,
	six, six, six, six, six, six, six, six, four, four
	};

enum SM_STATES {SM_Start, SM_Wait, SM_Wait2, SM_Play  } SM_State;
void Tick_Melody() {
	switch(SM_State) {
		case SM_Start:
			SM_State = SM_Wait;
		break;
		case SM_Wait:
			i = 0;
			if ((PINA & 0x01) == 0x00) {
				SM_State = SM_Play;
			}
			else {
				SM_State = SM_Wait;
			}
		break;
		case SM_Play:
			PWM_on();
			set_PWM(notes[i]);
			TimerSet(time[i]);
			while(!TimerFlag);
			TimerFlag = 0;
		if (i < 48) {
				i++;
				SM_State = SM_Play;
			}
			else {
				SM_State = SM_Wait2;
				PWM_off();
			}
		break;
		case SM_Wait2:
			if ((PINA & 0x01) == 0x00) {
				SM_State = SM_Wait2;
			}
			else if ((PINA & 0x01) == 0x01) {
				SM_State = SM_Wait;
			}
		break;
		default:
		break;
	}
}

			

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	SM_State = SM_Start;
	TimerOn();
    /* Insert your solution below */
	while(1) {
		Tick_Melody();
    }
    return 1;
}
