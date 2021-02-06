/*	Author: bbaid001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char button;
unsigned char cycle;
unsigned char B;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum states {INIT, s0, s1, s2, wait1, wait2, reset} state;

void Tick() {
	switch (state) {
		case (INIT) :
			state = s0;
			break;
		case (s0) :
			if (button == 0x00) {
				state = s1;
			}
			else {
				state = wait1;
			}
			break;

		case (s1) :
			if (cycle == 0) {
				if (button == 0x00) {
					state = s2;
				}
				else {
					state = wait1;
				}
			}
			else {
				if (button == 0x00) {
					state = s0;
				}
				else {
					state = wait1;
				}
			}
			break;

		case (s2) :
			if (button == 0x00) {
				state = s1;
			}
			else {
				state = wait1;
			}
			break;
		case (wait1) :
			if (button == 0x01) {
				state = wait1;
			}
			else {
				state = reset;
			}
			break;
		case (reset) :
			if (button == 0x00) {
				state = reset;
			}
			else {
				state = wait2;
			}
			break;

		case (wait2) :
			if (button == 0x01) {
				state = wait2;
			}
			else {
				state = s0;
			}
			break;
		default :
			state = INIT;
			break;
	}	


	switch (state) {
		case (INIT) :
			break;
		case (s0) :
			cycle = 0;
			B = 0x01;
			break;
		case (s1) :
			B = 0x02;
			break;
		case (s2) :
			cycle = 1;
			B = 0x04;
			break;
		case (wait1) :
			break;
		case (reset) :
			break;
		case (wait2) :
			break;
		default :
			B = 0x00;
			break;
	}
}
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	B = 0x00;
	button = 0x00;
	TimerSet(300);
	TimerOn();
	state = INIT;
	while (1) {
		button = ~PINA & 0x01;
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
		PORTB = B;
    	}
    return 1;
}
