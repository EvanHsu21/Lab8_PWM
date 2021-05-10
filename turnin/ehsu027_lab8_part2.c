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

unsigned char i = 0;
double note[8] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 };

enum SM_ONOFF { SM1_Start, SM1_On, SM1_Off, SM1_WaitOn, SM1_WaitOff } SM1_State;
void Tick_OnOff() {
	switch(SM1_State) {
		case SM1_Start:
			SM1_State = SM1_Off;
		break;
		case SM1_Off:
			if ((PINA & 0x01) == 0x00) {
				SM1_State = SM1_WaitOff;
				PWM_on();
				//set_PWM(note[i]);
			}
			else if ((PINA & 0x01) == 0x01) {
				SM1_State = SM1_Off;
			}
		break;
		case SM1_WaitOff:
			if ((PINA & 0x01) == 0x01) {
				SM1_State = SM1_On;
			}
			else {
				SM1_State = SM1_WaitOff;
			}
		break;
		case SM1_On:
			if ((PINA & 0x01) == 0x00) {
				SM1_State = SM1_WaitOn;
				PWM_off();
			}
			else if ((PINA & 0x01) == 0x01) {
				SM1_State = SM1_On;
			}
		break;
		case SM1_WaitOn:
			if ((PINA & 0x01) == 0x01) {
				SM1_State = SM1_Off;
			}
			else {
				SM1_State = SM1_WaitOn;
			}
		break;
		default:
		break;
	}
}


enum SM_STATES { SM_Start, SM_Up, SM_Down, SM_Wait } SM_State;
void Tick_Octave() {
	switch(SM_State) {
		case SM_Start:
			SM_State = SM_Wait;

		break;

		case SM_Wait:
			if ((PINA & 0x06) == 0x04) {
				SM_State = SM_Up;
				if (i < 7) {
					++i;
				}
			}
			else if ((PINA & 0x06) == 0x02) {
				SM_State = SM_Down;
				if (i > 0) {
					--i;
				}
			}
			else if ((PINA & 0x06) == 0x06) {
				SM_State = SM_Wait;
			}
		break;
		case SM_Up:
			if ((PINA & 0x06) == 0x04) {
				SM_State = SM_Up;
			}
			else if ((PINA & 0x06) == 0x06) {
				SM_State = SM_Wait;
			}
		break;
		case SM_Down:
			if ((PINA & 0x06) == 0x02) {
				SM_State = SM_Down;
			}
			else if ((PINA & 0x06) == 0x06) {
				SM_State = SM_Wait;
			}
		break;
		default:
			SM_State = SM_Start;
		break;
	}
	switch(SM_State) {
		case SM_Start:
		break;
		case SM_Wait:
			set_PWM(note[i]);
		break;
		case SM_Up:
			set_PWM(note[i]);
		break;
		case SM_Down:
			set_PWM(note[i]);
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
	SM1_State = SM1_Start;
    /* Insert your solution below */
	while(1) {
		Tick_OnOff();
		Tick_Octave();
    }
    return 1;
}
