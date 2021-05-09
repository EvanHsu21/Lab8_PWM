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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum SM_STATES { SM_Start, SM_C4, SM_D4, SM_E4, SM_WaitPress, SM_WaitC4, SM_WaitD4, SM_WaitE4 } SM_State;
void Tick_3Notes() {
	switch(SM_State) {
		case SM_Start:
			SM_State = SM_WaitPress;
		break;
		case SM_WaitPress:
			if ((PINA & 0x07) == 0x07) {
				SM_State = SM_WaitPress;
			}
			else if ((PINA & 0x07) == 0x06) {
				SM_State = SM_C4;
			}
			else if ((PINA & 0x07) == 0x05) {
				SM_State = SM_D4;
			}
			else if ((PINA & 0x07) == 0x03) {
				SM_State = SM_E4;
			}
		break;
		case SM_C4:
			SM_State = SM_WaitC4;
		break;
		case SM_D4:
			SM_State = SM_WaitD4;
		break;
		case SM_E4:
			SM_State = SM_WaitE4;
		break;
		case SM_WaitC4:
			if ((PINA & 0x07) == 0x06) {
				SM_State = SM_WaitC4;
			}
			else {
				SM_State = SM_WaitPress;
			}
		break;
		case SM_WaitD4:
			if ((PINA & 0x07) == 0x05) {
                                SM_State = SM_WaitD4;
                        }
                        else {
                                SM_State = SM_WaitPress;
                        }
		break;
		case SM_WaitE4:
			if ((PINA & 0x07) == 0x03) {
                                SM_State = SM_WaitE4;
                        }
                        else {
                                SM_State = SM_WaitPress;
                        }
		break;
	}
	switch (SM_State) {
		case SM_Start:
		break;
		case SM_WaitPress:
			PWM_off();
		break;
		case SM_C4:
			PWM_on();
			set_PWM(261.63);
		break;
		case SM_D4:
			PWM_on();
			set_PWM(293.66);
		break;
		case SM_E4:
			PWM_on();
			set_PWM(329.63);
		break;
		default:
		break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
	while(1) {
		Tick_3Notes();
    }
    return 1;
}
