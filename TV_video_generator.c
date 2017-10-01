/*
 * TV_video_generator.c
 *
 * Created: 24.07.2017 23:05:55
 *  Author: integrator.sb@gmail.com
 */ 

/*
На основе кода
http://www.javiervalcarce.eu/html/arduino-tv-signal-generator-en.html
*/

#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "menu.h"
#include "TV_video_generator.h"


 void vsync_pulse()
{
      R2RPORT = LEVEL_SYNC; 
      _delay_us(30); 
      R2RPORT = LEVEL_BLACK; 
      _delay_us(2); 
}

 void equal_pulse()
{
      R2RPORT = LEVEL_SYNC; 
      _delay_us(2); 
      R2RPORT = LEVEL_BLACK; 
      _delay_us(30);
}

 void hsync_pulse()
{
      R2RPORT = LEVEL_SYNC; 
      _delay_us(4); //4.7us
      R2RPORT = LEVEL_BLACK; 
      _delay_us(7); //7.3us
}



void tv_vbars()
{
	uint16_t i;
	// 6-310 (305 lines):
	for( i = 0; i < 305; i++)
	{
		hsync_pulse();
		R2RPORT = LEVEL_GRAY; _delay_us(8);
		R2RPORT =LEVEL_BLACK; _delay_us(14);
		R2RPORT =LEVEL_WHITE; _delay_us(8);
		R2RPORT =LEVEL_BLACK; _delay_us(14);
		R2RPORT =LEVEL_GRAY; _delay_us(8);
	}
}

void tv_squares(uint8_t c1)
{
	uint16_t i;
	uint8_t l1, l2, c, c2;//, tmp;
	
	c = 0;
	c2 = LEVEL_WHITE;
	
	i = 305;
	while( i--)
	{
/*
		if(c == 0)
		{
			tmp = c1;
			c1 = c2;
			c2 = tmp;
			c = 305/4;
			l1 = LEVEL_BLACK;
			l2 = LEVEL_BLACK;
		}
		else
		{
			c--;
			l1 = c1;
			l2 = c2;
		}
*/
		asm volatile("tst %0		\n\t"
		"brne 1f				\n\t"
		"ldi %0, 305/4			\n\t"
		"mov __tmp_reg__, %3	\n\t"
		"mov %3, %4				\n\t"
		"mov %4, __tmp_reg__	\n\t"
		"ldi %1, 15				\n\t"
		"ldi %2, 15				\n\t"
		"rjmp 2f				\n\t"
		"1:						\n\t"
		"dec %0					\n\t"
		"mov %1, %3				\n\t"
		"mov %2, %4				\n\t"
		"nop					\n\t"
		"nop					\n\t"
		"nop					\n\t"
		"nop					\n\t"
		"2:						\n\t"
		:"+d"(c),"+d"(l1), "+d"(l2), "+d"(c1), "+d"(c2) );
		
		hsync_pulse();
		R2RPORT = l1; _delay_us(12);
		R2RPORT = LEVEL_BLACK; asm("nop");asm("nop");
		R2RPORT = l2; _delay_us(12);
		R2RPORT = LEVEL_BLACK; asm("nop");asm("nop");
		R2RPORT = l1; _delay_us(12);
		R2RPORT = LEVEL_BLACK; asm("nop");asm("nop");
		R2RPORT = l2; _delay_us(12);
		R2RPORT = LEVEL_BLACK; asm("nop");asm("nop");
		R2RPORT = l1; _delay_us(2);
	}
}

void tv_lattice()
{
	tv_squares(LEVEL_WHITE);
}

void tv_chessboard()
{
	tv_squares(LEVEL_BLACK);
}


void tv_gen(uint8_t t)
{
	void(*f)(void);
	
	switch(t)
	{
		case TV_VBARS:
			f = &tv_vbars;
			break;
		case TV_LATTICE:
			f = &tv_lattice;
			break;
		case TV_CHESSBOARD:
			f = &tv_chessboard;
			break;
		default: 
			return;
	}
	
	R2RPORT = 0;
	while(btnCheck(BTN_START) == 0)
	{
		
		// 1:
		vsync_pulse(); vsync_pulse();
		// 2:
		vsync_pulse(); vsync_pulse();
		// 3:
		vsync_pulse(); equal_pulse();
		// 4:
		equal_pulse(); equal_pulse();
		// 5:
		equal_pulse(); equal_pulse();
		
		f();
		
		// 311:
		equal_pulse(); equal_pulse();
		// 312:
		equal_pulse(); equal_pulse();
		// 313:
		equal_pulse(); vsync_pulse();
		// 314:
		vsync_pulse(); vsync_pulse();
		// 315:
		vsync_pulse(); vsync_pulse();
		// 316:
		equal_pulse(); equal_pulse();
		// 317:
		equal_pulse(); equal_pulse();
		
		// 318-622 (305 lines):
		f();
		
		// 623:
		equal_pulse(); equal_pulse();
		// 624:
		equal_pulse(); equal_pulse();
		// 625:
		equal_pulse(); equal_pulse();
	}
	R2RPORT = 0;
}