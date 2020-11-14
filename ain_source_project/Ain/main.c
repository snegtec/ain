/*****************************************************************************
*
* AIN Led Module
* a simple led module with a microcontroller for Petzl Duo Led headlamp
* 
* Note: This is just a hobby project. I am not connected in any way with Petzl company.
* 
* version: 0.1 (March 2014)
* compiler: Atmel Studio 6
* author: Erik Kos
*          snegtec.com
*          snegtec@outlook.com
* 
* Copyright (C) 2014-2020 Erik Kos
* 
****************************************************************************
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
* 
****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

// cpu frequency is defined in menu: Project/...properties/Toolchain/AVR/GNU C compiler/Symbols/F_CPU=128000UL
// cpu freqency is set by fuse bits not by definition by F_CPU; F_CPU is needed by delay.h

long timeCounter = 0;
short clicks = 0;
const short MAXTIME = 750;
long lastClickTime = 0;
long pcint3Time = 0;

short initialized = 0;
#define LEDA 1
#define LEDB 2
#define LEDRED 3

// attiny 13a
#define PIN_0 PB0
#define PIN_1 PB1
#define PIN_R PB2
#define PIN_VCC PB3

// change it in loadLastMode function
short mode = LEDA;

short brightness_a = 0;
short brightness_b = 0;
short brightness_r = 0;

short brightnesses[] = {255, 128, 64, 10};

void disablePin0()
{
	// disconnect pin OC0A
	// def TCCR0A &=~(1<<COM0A0);
	TCCR0A &=~(1<<COM0A1);
	
	// set low state
	PORTB &=~(1 << PIN_0);
}

void enablePin0()
{
	// connect pin OC0A
	// def TCCR0A &=~(1<<COM0A0);
	TCCR0A |=(1<<COM0A1);
}


void disablePin1()
{
	// disconnect pin OC0B
	// def TCCR0A &=~(1<<COM0B0);
	TCCR0A &=~(1<<COM0B1);

	// set low state
	PORTB &=~(1 << PIN_1);
}

void enablePin1()
{
	// connect pin OC0B
	// def TCCR0A &=~(1<<COM0B0);
	TCCR0A |= (1<<COM0B1);
}

void enableRed()
{
	// set pins as output
	DDRB = DDRB | (1 << PIN_R);
	
	// high state
	PORTB = PORTB | (1 << PIN_R);
}

void disableRed()
{
	// set pins as output
	// def DDRB = DDRB | (1 << PIN_R);
	
	// set low state
	PORTB &=~(1 << PIN_R);
}

void enablePwm()
{
	//************
	// pwm
	
	// pin configuration for pwm
	
	// DDR - set driver for OC0A and OC0B pins
	// set pins as output
	DDRB |= (1 << PIN_0);
	DDRB |= (1 << PIN_1);
	
	// waveform generation mode
	
	// mode 3, Fast PWM
	// def TCCR0B = TCCR0B & ~(1 << WGM02);
	TCCR0A = TCCR0A | (1 << WGM01);
	TCCR0A = TCCR0A |(1 << WGM00);
	
	
	// clock select
	// TCCR0B: CS02 CS01 CS00
	
	// clkio/1
	// def TCCR0B &=~ (1<<CS02);
	// def TCCR0B &=~ (1<<CS01);
	TCCR0B |= (1<<CS00);
}

void initialize_app()
{
	enablePwm();
	
	// enable interrupts for timer0
	TIMSK0 |= (1 << TOIE0);
	TIFR0 |= (1 << TOV0);
	
	
	//************
	// for checking presence of vcc
	
	// set pin as input
	DDRB = DDRB & ~(1 << PIN_VCC);
	
	// disable pullup
	PORTB &=~ (1<<PIN_VCC);

	// enable PCINT0 pin for interrupt
	PCMSK = PCMSK | (1<<PCINT3);
	
	// enable PCINT7..0 pins
	GIMSK = GIMSK | (1<<PCIE);
	
	//************
	// start interrupt system
	sei();
}


void disableTimer()
{
	// clkio/1
	// def TCCR0B &=~ (1<<CS02);
	// def TCCR0B &=~ (1<<CS01);
	TCCR0B &=~ (1<<CS00);
	
}

void enableTimer()
{
	// clkio/1
	// def TCCR0B &=~ (1<<CS02);
	// def TCCR0B &=~ (1<<CS01);
	TCCR0B |= (1<<CS00);
	
}

void loadBrightness()
{
	OCR0A = brightnesses[brightness_a];
	OCR0B = brightnesses[brightness_b];
}


void changeBrightness()
{
	if (mode == LEDA)
	{
		brightness_a++;
		if (brightness_a > 3)
		brightness_a = 0;
	}
	else if (mode == LEDB)
	{
		brightness_b++;
		if (brightness_b > 3)
		brightness_b = 0;
	}
	else if (mode == LEDRED)
	{
		brightness_r++;
		if (brightness_r > 3)
		brightness_r = 0;
	}
	loadBrightness();
}

void enableLed()
{
	if (mode == LEDA)
	{
		enablePin0();
		disablePin1();
		disableRed();
	}
	else if (mode == LEDB)
	{
		disablePin0();
		enablePin1();
		disableRed();
	}
	else if (mode == LEDRED)
	{
		disablePin0();
		disablePin1();
		enableRed();
	}
}


void changeMode()
{
	if (mode == LEDA)
	mode = LEDB;
	else if (mode == LEDB)
	{
		mode = LEDRED;
	}
	else if (mode == LEDRED)
	mode = LEDA;
	
	enableLed();
}

void disableLed()
{
	disablePin0();
	disablePin1();
	disableRed();
}

int main()
{
	initialize_app();
	
	brightness_a = 0;
	brightness_b = 0;
	brightness_r = 0;
	loadBrightness();
	
	mode = LEDA;
	enableLed();
	
	// main loop
	while (1)
	{}
	
}


ISR(TIM0_OVF_vect)
{
	
	// increment counters
	timeCounter++;
	
	// service of clicks
	if ((clicks == 1) && (timeCounter >= lastClickTime + (2 * MAXTIME)))
	{
		// wait one MAXTIME period to handle/service 1-clicks
		
		changeMode();
		
		lastClickTime = 0;
		
		timeCounter = 0;
		clicks = 0;
		} else if (clicks == 2) {
		// 2-click has to be taken immediately
		
		changeBrightness();
		
		lastClickTime = 0;
		
		timeCounter = 0;
		clicks = 0;
	}
	
	//************
	
	if (pcint3Time > 0 && timeCounter >= pcint3Time + 100)
	{
		pcint3Time = 0;
		
		// if vcc detection on PIN_VCC
		if (PINB & (1 << PIN_VCC))
		{
			lastClickTime = timeCounter;
			clicks++;
			
			enableLed();
		}
		else if ((PINB & (1 << PIN_VCC)) == 0)
		{
			// vcc is missing
			
			disableLed();
			
			disableTimer();
			
			// disable interrupts for timer0
			TIMSK0 &=~ (1 << TOIE0);
			TIFR0 &=~ (1 << TOV0);
			
			// start interrupt system
			sei();
			
			// enable idle state
			MCUCR |= (1 << SE);
			
			// power down
			MCUCR |= (1 << SM1);
			MCUCR &=~ (1 << SM0);
			
			// enter idle state
			asm ("sleep");
		}
	}
}


ISR(PCINT0_vect)
{
	pcint3Time = timeCounter;

	// if this interrupt disabled idle state
	if (MCUCR & (1 << SE))
	{
		// disable idle state
		MCUCR &=~ (1 << SE);
		
		enableTimer();
		
		// enable interrupts for timer0
		TIMSK0 |= (1 << TOIE0);
		TIFR0 |= (1 << TOV0);
		
	}
}






