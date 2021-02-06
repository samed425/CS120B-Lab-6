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
unsigned char timer;
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

enum states {INIT, wait, inc, dec, reset} state;

void Tick() {
	switch (state) {
		case (INIT) :
			state = wait;
			break;
		case (wait) :
			if (button == 0x01) {
				state = inc;
			}
			else if (button == 0x02) {
				state = dec;
			} 
			else if (button == 0x03) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;


		case (inc) :
			if (button == 0x01) {
				state = inc;
			}
			if (button == 0x02) {
				state = dec;
			}
			else if (button == 0x03) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;

		case (dec) :
			if (button == 0x01) {
				state = inc;
			}
			if (button == 0x02) {
				state = dec;
			}
			else if (button == 0x03) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;

		case (reset) :
			if (button == 0x00) {
				state = wait;
			}
			else  if (button == 0x01) {
				state = inc;
			}
			else if (button == 0x02) {
				state = dec;
			}
			else {
				state = reset;
			}
			break;

		default :
			state = wait;
			break;
	}	


	switch (state) {
		case (INIT) :
			break;

		case (wait) :
			timer = 10;
			break;

		case (inc) :
			if (timer >= 10) {
				timer = 0;
				if (B < 9) {
					B++;
				}
			}
			timer++;
			break;

		case (dec) :
			if (timer >= 10) {
				timer = 0;
				if (B > 0) {
					B--;
				}
			}
			timer++;
			break;

		case (reset) :
			B = 0;
			timer = 10;
			break;

		default :
			break;
	}
}
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	B = 0x00;
	timer = 0;
	button = 0x00;
	TimerSet(100);
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
