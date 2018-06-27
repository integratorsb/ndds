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

void n_putchar(const char c)
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

#ifndef USE_ENCODER
uint8_t btn_wait()
{
	static uint8_t msk=0;
	uint8_t d, p, n, r;
	
	p = BTN_PORT;
	d = BTN_DDR;
	
	BTN_DDR &= ~(BTN_MASK);
	//_delay_ms(2);

	#ifdef SIM
	BTN_PORT &= ~(BTN_MASK);
	#else
	BTN_PORT |= BTN_MASK;
	#endif

	while(1)
	{
		if(msk != 0)
		{
			r = msk;
			if(btnCheck(msk) == 0)
				msk = 0;
			else
				_delay_ms(200);
			break;
		}
		else
		{
			if((msk = btnCheckAll()) != 0)
			{
				for(n = 5; n > 0; n--)
				{
					_delay_ms(100);
					if(btnCheck(msk) == 0)
						break;
				}
				r = msk;	
				if(n == 0)
					break;// долгое нажатие 
				msk = 0;
				if(n < 5)
					break;// обычное нажатие
		
			}
		}
	}
	BTN_PORT = p;
	BTN_DDR = d;
	return r;
}

	
		

/*
uint8_t btn_wait()
{
	
	static uint8_t st = 0, msk=0, c = 0;;
	uint8_t d, p;
	
	p = BTN_PORT;
	d = BTN_DDR;
	
	BTN_DDR &= ~(BTN_MASK);	
	//_delay_ms(2);
	
	#ifdef SIM
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
					c = 5;
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
}*/
#else
//uint8_t enc_wait()
uint8_t btn_wait()
{
	uint8_t i;
	uint8_t d, p, r;
	
	p = BTN_PORT;
	d = BTN_DDR;
	
	BTN_DDR &= ~(BTN_MASK);
	//_delay_ms(2);
	
	#ifdef SYM
	BTN_PORT &= ~(BTN_MASK);
	#else
	BTN_PORT |= BTN_MASK;
	#endif
	
	while(btnCheck(BTN_UP) == 0)
	{
		if(btnCheck(BTN_START))
			break;
	}
	
	_delay_ms(5);	
	
	while(btnCheck(BTN_UP) != 0)
	{
		if(btnCheck(BTN_START))
			break;
	}
	

	if(btnCheck(BTN_START))
	{
		r = BTN_SET;// долгое нажатие	
		
		for(i = 5; i > 0; i--)
		{	
			_delay_ms(100);
			if(btnCheck(BTN_START) == 0)
			{
				r = BTN_START;//обычное нажатие
				break;
			}
		}
		btn_wait_up_start();

	}
	else
	{
		if(btnCheck(BTN_DOWN))
			r = BTN_DOWN;
		else
			r = BTN_UP;
	}
	BTN_PORT = p;
	BTN_DDR = d;
	return r;
}
#endif

void btn_wait_up(uint8_t b)
{
	while(btnCheck(b))
	{
		_delay_ms(20);
	}
}

void btn_wait_up_start()
{
	btn_wait_up(BTN_START);
}

void print_u_ex(unsigned long a, char p, char n)
{
	char buf[12];
	unsigned char i;


	i = 0;

	do
	{
		i++;
		buf[i] = '0' + a % 10;
		a /= 10;
	}while(a != 0);

	while(i > n)
	{

		n_putchar(buf[i]);
		if(i == p)
			n_putchar('.');
		i--;

	}

}

void print_u(uint32_t a)
{
	print_u_ex(a, 0, 0);
}


void print_u8(uint8_t a)
{
	print_u(a);
}


inline uint32_t pow_10u(uint8_t p)
{
	uint32_t r = 1;

	while(p > 0)
	{
		r *= 10;
		p--;
	}
	return r;

}



uint32_t print_fq(uint32_t f)
{
	uint8_t p, d;
	char pr = '\0';

	
	d = 0;// сколько младших разрядов пропустить
	//pr = 0;// префикс
	p = 0;// после какого разряда стоит дес. точка


	if(f < 10000)
	{
		// xxxxHz
		if(generator.m == M_SQPWM && f > 1000)
		{
			// x.xKHz
			d = 2;
			p = 4;
			pr ='K';

		}

	}
	else if (f < 100000)
	{
		pr ='K';
		if(generator.m == M_SQPWM)
		{
			// xxKHz
			d = 3;

		}
		else
		{
			// xx.xxKHz
			d = 1;
			p = 4;

		}
	}
	else if (f < 1000000)
	{
		// xxx.xKHz
		pr ='K';
		d = 2;
		p = 4;

	}
	else
	{
		// x.xxMHz
		d = 4;
		p = 7;
		pr ='M';

	}
	print_u_ex(f, p, d);
	if(pr)
		n_putchar(pr);
	nnl_puts_P(PSTR("Hz"));
	return pow_10u(d);
}


/*

uint8_t get_range_u32(uint32_t a)
{
	uint8_t r = 0;
	
	while(a)
	{
		a /= 10;
		r++;
	}
	return r;
}
*/

/*
uint16_t print_fq(uint32_t f)
{
	char p, pr;
	uint16_t d;
	
	
	d = 1;
	pr = 0;
	p = 0;// после какого разряда стоит дес. точка

	if(f < 10000)
	{
		if(generator.m == M_SQPWM && f > 1000)
		{

				d = 100;
				p = 2;
				pr ='K';

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
}*/


/*

void print_t(uint32_t t)
{
/ *
	if(t > 10000)
		t /= 1000* /
	print_u(t);
	nnl_puts_P(PSTR("us"));
}*/

uint8_t print_t(uint32_t t)
{
	uint8_t p, d, r;
	char pr;

	p = 4;
	d = 0;
	pr = 'm';
	if(t < 1000)
	{
		p = 0;
		pr = 'u';
		r = 1;
	}
	else if(t < 10000)
	{
		d = 1;
		r = 10;
	}
	else
	{
		d = 2;
		r = 100;
	}
	
	print_u_ex(t, p, d);
	n_putchar(pr);
	n_putchar('s');
	//nnl_puts_P(PSTR("us"));
	return r;
}

void print_on_off( uint8_t st)
{

	lcd_goto(13);
	#ifdef USE_SW_UART
		SerialPutChar('\r');
	#endif
	if(st)
		nnl_puts_P(PSTR("ON \n"));
	else
		nnl_puts_P(PSTR("OFF\n"));
}

void input_fd(const char* t, uint32_t* p)
{
	uint8_t b;
	//uint16_t d;
	uint32_t multipler;
	uint32_t max, f, d;
	
	f = *p;



	multipler = 0;
	while(1)
	{
		
		switch(generator.m)
		{
			case M_SQW:
				max = MAXFREQ_SQW;
				break;
			case M_SQPWM:
				max = MAXFREQ_PWM;
				break;			
			case M_DTMF:
				max = MAXFREQ_DTMF;
				break;
			default:
				max = MAXFREQ_DDS;
		}		
		


		nnl_puts_P(t);
		d = print_fq(f);
		f -= f % d;

		if(multipler < d)
			multipler = d;
			
		max -= multipler;		

		nnl_puts_P(PSTR("\n(+/-) "));
		print_fq(multipler);

		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(f > multipler )
				f -= multipler;
		}
		else if(b == BTN_UP)
		{
			if(f < max )
				f += multipler;
/*
			f += multipler;
			if(f >= max )
				f -= multipler;*/
		}
		#ifndef USE_ENCODER
		else if(b == BTN_MODE)
		#else
		else if(b == BTN_START)
		#endif
		{
			multipler *= 10;
			if(multipler > max)
				multipler = 0;
		}
		else if(b == BTN_SET)
		{
			break;
		}

		if(generator.m == M_SQPWM && pwm_is_run() )
			pwm_start(f, generator.pwm_d, 0);
	}
	
	*p = f;
}

void input_f(uint32_t* p)
{
	input_fd(PSTR("\rF="), p);
}


inline void input_dc()
{
	uint8_t b, dc;
	dc = generator.pwm_d;
	while(1)
	{
		nnl_puts_P(PSTR("\rDC="));
		//print_u(dc, 0);
		print_u8(dc);
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
		
		if(pwm_is_run())
			pwm_start(generator.pwm_f, dc, 0);
	}
	generator.pwm_d = dc;	
}


void input_t(uint32_t* p, const char* title)
{
	uint8_t b, d;
	uint32_t multipler, t;
	
	t = *p;
/*
	if(t > MAX_T)
		t = MIN_T;
	*/
	multipler = 1;
	while(1)
	{
		nnl_puts_P(title);
		d = print_t(t);
		t -= t % d;

		if(multipler < d)
			multipler = d;
		nnl_puts_P(PSTR("\n(+/-) "));
		print_t(multipler);	
		
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(t >= MIN_T+multipler )
			t-= multipler;
		}
		else if(b == BTN_UP)
		{
			t += multipler;
			if(t > MAX_T )
				t = MAX_T;
		}
		#ifndef USE_ENCODER
		else if(b == BTN_MODE)
		#else
		else if(b == BTN_START)
		#endif
		{
			multipler *= 10;
			if(multipler > MAX_T/10)
				multipler = 1;
/*
			if(multipler < 1000000)
				multipler *= 10;
			else
				multipler = 1;*/
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
			print_u(n);
		else
			nnl_puts_P(PSTR("ND"));
		nnl_puts_P(PSTR("\n(+/-) "));
		print_u(multipler);

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
		#ifndef USE_ENCODER
		else if(b == BTN_MODE)
		#else
		else if(b == BTN_START)
		#endif
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

/*
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

}*/

void input_hs()
{
	
	uint32_t f = generator.hs_f;
	uint8_t b;
	
	while(1)
	{
		//nnl_puts_P(t);
		nnl_puts_P(PSTR("\rF="));
		print_fq(f);
		nnl_puts_P(PSTR("\n(+/-) x2"));
		b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(f > 125000)
				f /= 2;
		}
		else if(b == BTN_UP)
		{

			if(f < 8000000)
				f *= 2;
		}
		else if(b == BTN_SET)
		{
			break;
		}
	}
	generator.hs_f = f;
}

/*
inline uint8_t input_io_mode()
{
	const char* values[] = {
		PSTR("PULL-UP"),
		PSTR("PUSH-PULL"),
	};
	
	return select_val(PSTR("\rOUT: "), values, 2) - 1;
}*/



void print_pattern(int8_t d)
{
	if(d == CMD_BRK)
		nnl_puts_P(PSTR("BRK"));
	else if(d == CMD_REP)
		nnl_puts_P(PSTR("REP"));
	else if(d < 0)
	{
		print_u8(-d);
		n_putchar('L');
	}
	else
	{
		print_u8(d);
		n_putchar('H');
	}

}



uint8_t abs_u8(int8_t d)
{
	if(d == CMD_REP)
		d = 0;
	return (d >= 0 ? d : -d);
}




/*
inline void print_dpattern_t(uint8_t i)
{
	nnl_puts_P(PSTR("T="));
	print_t(generator.dpattern_t * (abs_u8(buf[i]) + abs_u8(buf[i+1])));
}
*/

/*
inline void input_dpattern_t(uint8_t i)
{
	uint8_t  k;

	
	k = i;
	while(1)
	{
		
		print_dpattern_t(i);
		
		n_putchar('\n');
		print_pattern(buf[i]);
		if(k == i)
			n_putchar('<');
		else
			n_putchar('>');
		print_pattern(buf[i+1]);	
		
		uint8_t b = btn_wait();
		if(b == BTN_DOWN)
		{
			buf[k]--;
		}
		else if(b == BTN_UP)
		{
			buf[k]++;
		}
		#ifndef USE_ENCODER
		else if(b == BTN_MODE)
		#else
		else if(b == BTN_START)
		#endif
		{
			if(k == i)
				k++;
			else
				k = i;
		}
		else if(b == BTN_SET)
		{
			break;
		}
	}
}*/

inline void input_dpattern()
{
	uint8_t i = 0, s = 2;
	
	eeprom_read_block(buf, e_pattern, sizeof(buf));
	while(1)
	{

		//print_dpattern_t(i);

		//lcd_goto(12);		
		//n_putchar(':');
		//print_u8(i/2);
		//n_putchar('\n');
		
		n_putchar('\r');
		print_pattern(buf[i]);
		if(s == 0)
			n_putchar('<');
		else if(s == 1)
			n_putchar('>');
		print_pattern(buf[i+1]);
		n_putchar(',');	
		print_pattern(buf[i+2]);
		print_pattern(buf[i+3]);	
		nnl_puts_P(PSTR("...\n:"));
		print_u8(i/2);
		nnl_puts_P(PSTR(" T="));
		print_t(generator.dpattern_t * (abs_u8(buf[i]) + abs_u8(buf[i+1])));
		//print_dpattern_t(i);
/*
		uint8_t v, k;
		k = i + 2;
		for(v = 3; v > 0; v--)
		{

			print_pattern(buf[k]);
			print_pattern(buf[k+1]);	
			n_putchar(',');		
			k += 2;	
		}*/

		uint8_t b = btn_wait();
		if(b == BTN_DOWN)
		{
			if(s == 2)
				i -= 2;
			else
				buf[i + s]--;
		}
		else if(b == BTN_UP)
		{
			if(s == 2)
				i+=2;
			else
				buf[i + s]++;
		}
		#ifndef USE_ENCODER
		else if(b == BTN_MODE)
		#else
		else if(b == BTN_START)
		#endif
		{
			if(s > 0)
				s--;
			else
				s = 2;
		}
		else if(b == BTN_SET)
		{
			eeprom_update_block(buf, e_pattern, sizeof(e_pattern));
			break;
		}
	}

}
