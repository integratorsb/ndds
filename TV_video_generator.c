/*
 * TV_video_generator.c
 *
 * Created: 24.07.2017 23:05:55
 *  Author: integrator.sb@gmail.com
 */ 
/*
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
      _delay_us(5); //4.7us
      R2RPORT = LEVEL_BLACK; 
      _delay_us(7); //7.3us
}

/*
void tv_out()
{
	uint16_t i;
	uint8_t c, n, k;
	
	n =0;
	c= 0;
	for( i = 0; i < 305; i++)
	{
		hsync_pulse();
		if(c == 0)
		{
			c = 25;
			n+=16;
		}
		else
			c--;

		for(k = 13; k!=0;k--)
		{
			R2RPORT = buf[ n + c];
			_delay_us(4);
		}

	}
	
}
void tv_out1()
{
	uint16_t i;
	uint8_t c, k;
	

	c= 0;
	for( i = 0; i < 305; i++)
	{
		hsync_pulse();
		if(c == 0)
		{
			c = 76;
			R2RPORT = LEVEL_WHITE;
			_delay_us(52);	
			R2RPORT = LEVEL_BLACK;
		}
		else
		{
			for(k = 13; k != 0;k--)
			{
				R2RPORT = LEVEL_BLACK;
				_delay_us(12);
				R2RPORT = LEVEL_WHITE;
				_delay_us(1);
			}
			c--;			
		}

	}
}
*/


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




void tv_gen(void(*f)(void))
{
  
      R2RPORT = 0;
      while(!btnCheck(BTN_START))
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