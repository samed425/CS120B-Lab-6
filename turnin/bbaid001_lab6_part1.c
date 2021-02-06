/*	Author: bbaid001
 *	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	*I acknowledge all content contained herein, excluding template or example
 *	*code, is my own original work.
 *
 *      Demo Link: https://www.youtube.com/watch?v=WQzCCZaqL98
 *	*/

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum states {INIT, s0, s1, s2} state;

void Tick() {
	switch (state) {
		case (INIT) :
			state = s0;
			break;
		case (s0) :
			state = s1;
			break;
		case (s1) :
			state = s2;
			break;
		case (s2) :
			state = s0;
			break;
		default :
			state = INIT;
			break;
	}	
	switch (state) {
		case (INIT) :
			break;
		case (s0) :
			PORTB = 0x01;
			break;
		case (s1) :
			PORTB = 0x02;
			break;
		case (s2) :
			PORTB = 0x04;
			break;
		default :
			PORTB = 0x00;
			break;
	}
}
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	state = INIT;
	while (1) {
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
    	}
    return 1;
}
