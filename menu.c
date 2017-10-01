/*
 * menu.c
 *
 * Created: 14.07.2017 0:36:15
 *  Author: integrator.sb@gmail.com
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "suart.h"
#include "menu.h"
#include "hd44780.h"


gen_t generator;

void n_putchar(char c)
{
	
	if(c == '\r')
	{
		lcd_clrscr();
		//lcd_home();
	}
	else if(c == '\n')
	{

		lcd_goto(0x40);
		#ifdef USE_SW_UART
		c = ' ';
		#endif
	}
	else
	{
		lcd_putc(c);
	}
	#ifdef USE_SW_UART
		SerialPutChar(c);
	#endif
	//return 0;
}

void nnl_puts_P(const char* s)
{
	char c;
	while((c = pgm_read_byte(s++)) != 0)
	{
		n_putchar(c);
	}
}



uint8_t btnCheckAll()
{
	uint8_t i, msk;
	
	msk = 1;
	
	for(i = 0; i < 8; i++)
	{
		if(BTN_MASK & msk)
		{
			if(btnCheck(msk))
			return msk;
		}
		msk = msk<<1;
	}
	return 0;
}


uint8_t btn_wait()
{
	
	static uint8_t st = 0, msk=0, c = 0;;
	uint8_t d, p;
	
	p = BTN_PORT;
	d = BTN_DDR;
	
	BTN_DDR &= ~(BTN_MASK);	
	//_delay_ms(2);
	
	#ifdef SYM
	BTN_PORT &= ~(BTN_MASK);	
	#else
	BTN_PORT |= BTN_MASK;
	#endif

	_delay_ms(2);
	while(1)
	{
		_delay_ms(20);
		if(c != 0) c--;
		
		switch(st)
		{
			case 0:
			{
				if((msk = btnCheckAll())!=0)
					st++;
				break;
			}
			case 1:
			{
				//_delay_ms(10);
				if(btnCheck(msk) != 0)
				{
					st++;
					c = 25;
				}
				else
					st = 0;
				break;
			}
			case 2:
			{
				if((btnCheck(msk) == 0) && c != 0)
				{
					//обычное нажатие
					st = 0;
					BTN_PORT = p;
					BTN_DDR = d;
					return msk;
				}
				else if((btnCheck(msk) != 0) && c == 0)
				{
					//долгое нажатие
					//c = 10;
					st = 3;
				}

				break;
			}
			case 3:
			{
				if(c == 0)
				{
					
					if((btnCheck(msk) == 0))
					{
						st = 0;
					}
					c = 2;
					//_delay_ms(200);
					BTN_PORT = p;
					BTN_DDR = d;
					return msk;
				}
			}
			default:
				st =0;
				break;
		}

	}
}

void btn_wait_up(uint8_t b)
{
	while(btnCheck(b))
	_delay_ms(20);
}


void print_u(uint32_t a, uint8_t p)
{
	char s[9];//, c;
	uint8_t i;
	
	i = 8;
	do
	{
		s[i] = (a % 10) + '0';
		a /= 10;
		i--;
	}
	while(i > 0);
	
	i = 0;
	while(s[++i] == '0' && i < 8){};
	for(; i<9; i++)
	{
		n_putchar(s[i]);
		if(9 - i == p)
		n_putchar(',');
	}

}

uint16_t print_fq(uint32_t f)
{
	char p, pr;
	uint16_t d;
	
	
	d = 1;
	pr = 0;
	p = 0;// после какого разряда стоит дес. точка

	if(f < 10000)
	{
		if(generator.m == M_SQPWM)
		{
			if(f > 1000)
			{
				d = 100;
				p = 2;
				pr ='K';
			}
		}

	}
	else if (f < 100000)
	{
		pr ='K';
		if(generator.m == M_SQPWM)
		{
			d = 1000;
			p = 0;
		}
		else
		{
			d = 10;
			p = 3;
		}
	}
	else if (f < 1000000)
	{

		pr ='K';
		d = 100;
		p = 2;
	}
	else
	{
		d = 10000;
		p = 3;
		pr ='M';

	}
	print_u(f/d, p);
	if(pr)
		n_putchar(pr);
	nnl_puts_P(PSTR("Hz"));
	return d;
}

void print_on_off( uint8_t st)
{

	lcd_goto(13);

	if(st)
	{
		#ifdef USE_SW_UART
			SerialPutChar('\r');
		#endif
		nnl_puts_P(PSTR("ON \n"));
	}
	else
		nnl_puts_P(PSTR("OFF\n"));
}

void input_fd(uint32_t* p, uint8_t is_run)
{
	uint8_t b;
	uint16_t d;
	uint32_t multipler;
	uint32_t max, f;
	
	f = *p;
	if(generator.m == M_SQPWM)
		max = MAXFREQ_PWM;
	else if(generator.m == M_SQW)
		max = MAXFREQ_SQW;
	else
		max = MAXFREQ_DDS;

	multipler = 0;
	while(1)
	{

		nnl_puts_P(PSTR("\rF="));
		d = print_fq(f);
		f -= f % d;
		if(multipler == 0)
			multipler = d;
		
		nnl_puts_P(PSTR("\n(+/-) "));
		print_fq(multipler);
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(f > 1 + multipler )
			f-= multipler;
		}
		else if(b == BTN_UP)
		{
			if(f < max-multipler )
			f += multipler;
		}
		else if(b == BTN_MODE)
		{
			if(multipler < 100000)
			multipler *= 10;
			else
			multipler = d;
		}
		else if(b == BTN_SET)
		{
			break;
		}

		if(generator.m == M_SQPWM && is_run)
			pwm_start(f, generator.pwm_d, 0);
	}
	
	*p = f;
}

inline void input_dc(uint8_t is_run)
{
	uint8_t b, dc;
	dc = generator.pwm_d;
	while(1)
	{
		nnl_puts_P(PSTR("\rDC="));
		print_u(dc, 0);
		//putchar('%');
		nnl_puts_P(PSTR("%\n(+/-) 1% "));
		
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(dc > 1 )
			dc -= 1;
		}
		else if(b == BTN_UP)
		{
			if(dc < 99 )
			dc += 1;
		}
		else if(b == BTN_SET)
		{
			break;
		}

		if(is_run)
			pwm_start(generator.pwm_f, dc, 0);
	}
	generator.pwm_d = dc;	
}

void input_t(uint32_t* p, const char* title)
{
	uint8_t b;
	uint32_t multipler, t;
	
	t = *p;
	if(t > MAX_T)
		t = MIN_T;
	
	multipler = 1;
	while(1)
	{
		nnl_puts_P(title);
		print_u(t,0);
		nnl_puts_P(PSTR("us\n(+/-) "));
		print_u(multipler,0);
		nnl_puts_P(PSTR("us"));
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(t > MIN_T+multipler )
			t-= multipler;
		}
		else if(b == BTN_UP)
		{
			if(t < MAX_T-multipler )
			t+= multipler;
		}
		else if(b == BTN_MODE)
		{
			if(multipler < 1000000)
				multipler *= 10;
			else
				multipler = 1;
		}
		else if(b == BTN_SET)
		{
			break;
		}

	}
	*p = t;
}

inline void input_n()
{
	uint8_t b;
	uint16_t multipler, n;


	n = generator.pulse_n;
	multipler = 1;
	while(1)
	{
		nnl_puts_P(PSTR("\rN="));
		if(n != 0)
			print_u(n,0);
		else
			nnl_puts_P(PSTR("ND"));
		nnl_puts_P(PSTR("\n(+/-) "));
		print_u(multipler,0);

		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(n >= multipler )
				n-= multipler;
		}
		else if(b == BTN_UP)
		{
			if(n < 0xFFFF-multipler )
			n += multipler;
		}
		else if(b == BTN_MODE)
		{
			if(multipler < 10000)
			multipler *= 10;
			else
			multipler = 1;
		}
		else if(b == BTN_SET)
		{
			break;
		}

	}
	generator.pulse_n = n; 	
}


uint8_t select_val(const char* t, const char* v[], uint8_t n)
{
	uint8_t b, i;
	
	i = 0;
	
	while(1)
	{
		nnl_puts_P(t);
		nnl_puts_P(v[i]);	
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(i > 0)
				i--;
		}
		else if(b == BTN_UP)
		{
			if(i < n-1)
			i++;
		}
		else if(b == BTN_SET)
		{
			break;
		}

	}
	return i;
}

inline uint8_t input_trig()
{
	const char* values[] = {
		PSTR("NO"),
		PSTR("RISE "),
		PSTR("FALL ")
	};
	
	return select_val(PSTR("\rTrigger: "), values, 3);
}

inline uint8_t input_ext_sync()
{
	const char* values[] = {
		PSTR("NO"),
		PSTR("HIGH"),
		PSTR("LOW")
	};
	
	return select_val(PSTR("\rEXT SYNC: "), values, 3);
}

void input_hs()
{
	const char* values[] = {
		PSTR("1MHz"),
		PSTR("2MHz"),
		PSTR("4MHz"),
		PSTR("8MHz")
	};	

	uint8_t  f;
	
	f = 1<<select_val(PSTR("\rF="), values, 4);
	generator.hs_f = (uint32_t)(1000000 * f);

}