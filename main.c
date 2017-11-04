
/*
 * main.c
 *
 * Created: 10.07.2017 1:17:10
 *  Author: integrator.sb@gmail.com
 */ 

#include "config.h"
//#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include "hd44780.h"
#include "menu.h"
#include "suart.h"
#include "dds_rout.h"
#include "TV_video_generator.h"


const gen_t PROGMEM def_setting = DEF_SETTING;
gen_t EEMEM setting = DEF_SETTING;

uint8_t __attribute__ ((section (".buffer"))) buf[256]; 
uint8_t is_run = 0;

const uint8_t  sinewave[] PROGMEM= 
{
	0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
	0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
	0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf5,
	0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
	0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
	0xda,0xd8,0xd5,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb3,
	0xb0,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
	0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,
	0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
	0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,
	0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
	0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,
	0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};

const uint8_t sawtoothwave[] PROGMEM= 
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
	0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};
const uint8_t rsawtoothwave[] PROGMEM= 
{
	0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf3,0xf2,0xf1,0xf0,
	0xef,0xee,0xed,0xec,0xeb,0xea,0xe9,0xe8,0xe7,0xe6,0xe5,0xe4,0xe3,0xe2,0xe1,0xe0,
	0xdf,0xde,0xdd,0xdc,0xdb,0xda,0xd9,0xd8,0xd7,0xd6,0xd5,0xd4,0xd3,0xd2,0xd1,0xd0,
	0xcf,0xce,0xcd,0xcc,0xcb,0xca,0xc9,0xc8,0xc7,0xc6,0xc5,0xc4,0xc3,0xc2,0xc1,0xc0,
	0xbf,0xbe,0xbd,0xbc,0xbb,0xba,0xb9,0xb8,0xb7,0xb6,0xb5,0xb4,0xb3,0xb2,0xb1,0xb0,
	0xaf,0xae,0xad,0xac,0xab,0xaa,0xa9,0xa8,0xa7,0xa6,0xa5,0xa4,0xa3,0xa2,0xa1,0xa0,
	0x9f,0x9e,0x9d,0x9c,0x9b,0x9a,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,
	0x8f,0x8e,0x8d,0x8c,0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80,
	0x7f,0x7e,0x7d,0x7c,0x7b,0x7a,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,
	0x6f,0x6e,0x6d,0x6c,0x6b,0x6a,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,
	0x5f,0x5e,0x5d,0x5c,0x5b,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,
	0x4f,0x4e,0x4d,0x4c,0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,
	0x3f,0x3e,0x3d,0x3c,0x3b,0x3a,0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,
	0x2f,0x2e,0x2d,0x2c,0x2b,0x2a,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,
	0x1f,0x1e,0x1d,0x1c,0x1b,0x1a,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,
	0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
};

const uint8_t trianglewave[] PROGMEM= 
{
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
	0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
	0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
	0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
	0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
	0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
	0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
	0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
	0xff,0xfd,0xfb,0xf9,0xf7,0xf5,0xf3,0xf1,0xef,0xef,0xeb,0xe9,0xe7,0xe5,0xe3,0xe1,
	0xdf,0xdd,0xdb,0xd9,0xd7,0xd5,0xd3,0xd1,0xcf,0xcf,0xcb,0xc9,0xc7,0xc5,0xc3,0xc1,
	0xbf,0xbd,0xbb,0xb9,0xb7,0xb5,0xb3,0xb1,0xaf,0xaf,0xab,0xa9,0xa7,0xa5,0xa3,0xa1,
	0x9f,0x9d,0x9b,0x99,0x97,0x95,0x93,0x91,0x8f,0x8f,0x8b,0x89,0x87,0x85,0x83,0x81,
	0x7f,0x7d,0x7b,0x79,0x77,0x75,0x73,0x71,0x6f,0x6f,0x6b,0x69,0x67,0x65,0x63,0x61,
	0x5f,0x5d,0x5b,0x59,0x57,0x55,0x53,0x51,0x4f,0x4f,0x4b,0x49,0x47,0x45,0x43,0x41,
	0x3f,0x3d,0x3b,0x39,0x37,0x35,0x33,0x31,0x2f,0x2f,0x2b,0x29,0x27,0x25,0x23,0x21,
	0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x11,0x0f,0x0f,0x0b,0x09,0x07,0x05,0x03,0x01
};
const uint8_t ECG[] PROGMEM= 
{
	73,74,75,75,74,73,73,73,73,72,71,69,68,67,67,67,
	68,68,67,65,62,61,59,57,56,55,55,54,54,54,55,55,
	55,55,55,55,54,53,51,50,49,49,52,61,77,101,132,
	169,207,238,255,254,234,198,154,109,68,37,17,5,
	0,1,6,13,20,28,36,45,52,57,61,64,65,66,67,68,68,
	69,70,71,71,71,71,71,71,71,71,72,72,72,73,73,74,
	75,75,76,77,78,79,80,81,82,83,84,86,88,91,93,96,
	98,100,102,104,107,109,112,115,118,121,123,125,
	126,127,127,127,127,127,126,125,124,121,119,116,
	113,109,105,102,98,95,92,89,87,84,81,79,77,76,75,
	74,73,72,70,69,68,67,67,67,68,68,68,69,69,69,69,
	69,69,69,70,71,72,73,73,74,74,75,75,75,75,75,75,
	74,74,73,73,73,73,72,72,72,71,71,71,71,71,71,71,
	70,70,70,69,69,69,69,69,70,70,70,69,68,68,67,67,
	67,67,66,66,66,65,65,65,65,65,65,65,65,64,64,63,
	63,64,64,65,65,65,65,65,65,65,64,64,64,64,64,64,
	64,64,65,65,65,66,67,68,69,71,72,73
};
/*
const uint8_t PROGMEM fexp[] = {
	0, 4, 9, 14, 19, 23, 28, 32, 37, 41, 45, 49, 53, 57, 61, 65,
	68, 72, 75, 79, 82, 86, 89, 92, 95, 98, 101, 104, 107, 110, 113, 116,
	118, 121, 124, 126, 129, 131, 134, 136, 138, 141, 143, 145, 147, 149, 151, 153,
	155, 157, 159, 161, 163, 165, 166, 168, 170, 171, 173, 175, 176, 178, 179, 181,
	182, 184, 185, 186, 188, 189, 190, 192, 193, 194, 195, 196, 197, 199, 200, 201,
	202, 203, 204, 205, 206, 207, 208, 209, 210, 210, 211, 212, 213, 214, 215, 215,
	216, 217, 218, 218, 219, 220, 221, 221, 222, 223, 223, 224, 224, 225, 226, 226,
	227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234,
	234, 235, 235, 236, 236, 236, 237, 237, 238, 238, 238, 239, 239, 239, 240, 240,
	240, 240, 241, 241, 241, 242, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244,
	244, 244, 245, 245, 245, 245, 245, 246, 246, 246, 246, 246, 247, 247, 247, 247,
	247, 247, 248, 248, 248, 248, 248, 248, 248, 249, 249, 249, 249, 249, 249, 249,
	249, 250, 250, 250, 250, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 251,
	251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
	252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253,
	253, 253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254
};

const uint8_t PROGMEM fexp2[] = {
	9, 19, 28, 37, 45, 53, 61, 68, 75, 82, 89, 95, 101, 107, 113, 118,
	124, 129, 134, 138, 143, 147, 151, 155, 159, 163, 166, 170, 173, 176, 179, 182,
	185, 188, 190, 193, 195, 197, 200, 202, 204, 206, 208, 210, 211, 213, 215, 216,
	218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234,
	235, 236, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244,
	245, 245, 245, 246, 246, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249, 249,
	250, 250, 250, 250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
	252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254,
	247, 237, 228, 219, 211, 203, 195, 188, 181, 174, 167, 161, 155, 149, 143, 138,
	132, 127, 122, 118, 113, 109, 105, 101, 97, 93, 90, 86, 83, 80, 77, 74,
	71, 68, 66, 63, 61, 59, 56, 54, 52, 50, 48, 46, 45, 43, 41, 40,
	38, 37, 35, 34, 33, 32, 30, 29, 28, 27, 26, 25, 24, 23, 22, 22,
	21, 20, 19, 18, 18, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12,
	11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7,
	6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4,
	4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2};

*/

#ifdef VGEN_ENABLE

inline void vgen_on()
{
	DDRB |= 1<<PORTB3;
	OCR2 = F_CPU/8/2/VGEN_FREQ;
	TCCR2 =  (0<<WGM20)|(1<<WGM21)| (1<<COM20)|  (0<<CS22)|(1<<CS21)|(0<<CS20);
}

#define vgen_off() (TCCR2 = 0)
#endif 
	
/*
inline void adc_init()
{
	//x32 prescaler
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);
	ADMUX = 1<<REFS0| 0<<REFS1|0<<ADLAR | EXT_PIN;	
}
*/

inline void io_init()
{
	DDRB |=  (1<<HSPORT);
	
	#ifdef USE_OMUX
	DDRB |= (1<<OMUX_PIN);
	#endif
	
	#ifdef VGEN_ENABLE
	DDRB |= (1<<VGEN_PIN);
	#endif
	
	#ifdef USE_SWUART
	DDRB |= (0<<UART_RX) | (1<<UART_TX);
	#endif
	
	PORTB = 0<<HSPORT;
/*
	#ifdef VGEN_ENABLE
	PORTB &= ~(1<<VGEN_PIN);
	#endif*/
	
	#ifdef USE_SWUART
	PORTB |= 1<<UART_RX | 1<<UART_TX	
	#endif
	
	DDRC =  0<<EXT_PIN;
	PORTC = BTN_MASK | 0<<EXT_PIN;
	R2RPORT = 0;
	R2RDDR = 0xff;
}


inline void noise_start()
{
	while(!btnCheck(BTN_START))
	{
		R2RPORT = rand();
	}
}

inline void pwm_stop()
{
	TCCR1A = 0;
	TCCR1B = 0;
	HSPORT &= ~(1<<HSPIN);
	HSDDR &= ~(1<<HSPIN);
}


void pwm_start(uint32_t freq, uint8_t d, uint8_t e )
{
	uint32_t period;
	
	HSPORT &= ~(1<<HSPIN);
	HSDDR |= 1<<HSPIN;
	
	period = (F_CPU/freq) - 1;
	
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM13)  | (1<<WGM12);
	//fast PWM
	
	if(freq > 300)
	{
				
		ICR1 = (uint16_t)period;	

		if(generator.m == M_HSSQW)
			OCR1B =  (uint16_t)(period/2);
		else
			OCR1B =  (uint16_t)(period * d / 100);
		
		TCCR1B = (TCCR1B & 0b11111000) | (1<<CS10);
		//TCCR1B |= (1<<CS10);//без делителя

		
		
	}
	else
	{
		period /= 256;
		ICR1 = (uint16_t)period;	
		OCR1B =  (uint16_t)(period * d / 100);	
		TCCR1B = (TCCR1B & 0b11111000) | (1<<CS12);
		//TCCR1B |=  (1<<CS12);// делитель 256

	}	
	
	TCCR1A |= (1<<COM1B1);
	

//	if(e == EXT_NO)
//		return;
	if(e == EXT_HIGHT)
	{
		while(!btnCheck(BTN_START))
		{
			if((EXT_PINS & EXT_PIN_MSK) != 0)
				TCCR1A |= 1<<COM1B1;
			else
				TCCR1A &= ~(1<<COM1B1);
		}
	}
	else if(e == EXT_LOW)
	{
		while(!btnCheck(BTN_START))
		{
			if((EXT_PINS & EXT_PIN_MSK) == 0)
				TCCR1A |= 1<<COM1B1;
			else
				TCCR1A &= ~(1<<COM1B1);
		}
	}
	else return;
	pwm_stop();	
}


inline void dds_start(uint32_t f)
{
	uint32_t phase;
	
	//#define RES  ((16000000/9/0x1000000)*0x10000)
	#define RES (F_CPU/NUM_CYCLES/0x100)
	
	phase = ((uint64_t)f << 16) / RES;
	

	R2RPORT = 0;
	//R2RDDR = 0xFF;
	dds(phase);
	R2RPORT = 0;
	//R2RDDR = 0xFF;

}

inline void ddssq_start(uint32_t f)
{
	uint32_t phase;
	
	#define RES1 (F_CPU/NUM_CYCLES/2/0x100)
	
	phase = ((uint64_t)f << 16) / RES1;
	
	HSPORT &= ~(1<<HSPIN);
	HSDDR |= 1<<HSPIN;
	dds_sq(phase);
	HSPORT &= ~(1<<HSPIN);
	HSDDR &= ~(1<<HSPIN);

}

uint32_t get_phase(uint32_t T)
{
	return 0x1000000/(((uint64_t)T<<16)/36864);
}

inline void pulse_start()
{
	pulse_t pulse_s;	
	
	pulse_s.t_rise = get_phase(generator.pulse_t_rise);
	pulse_s.t_on = get_phase(generator.pulse_t_on);
	pulse_s.t_fall = get_phase(generator.pulse_t_fall);
	pulse_s.t_off = get_phase(generator.pulse_t_off);
	pulse_s.n = generator.pulse_n;
	pulse(&pulse_s);
}

inline uint8_t trig_wait_rise()
{
	while((EXT_PINS & EXT_PIN_MSK) != 0)
	{
		if(btnCheck(BTN_START)) return 0;
	}
	while((EXT_PINS & EXT_PIN_MSK) == 0)
	{
		if(btnCheck(BTN_START)) return 0;
	}
	return 1;
}

inline uint8_t  trig_wait_fall() 
{
	while((EXT_PINS & EXT_PIN_MSK) == 0)
	{
		if(btnCheck(BTN_START)) return 0;
	}
	while((EXT_PINS & EXT_PIN_MSK) != 0)
	{
		if(btnCheck(BTN_START)) return 0;
	}
	return 1;
}

void save_mode()
{
	eeprom_update_byte(&setting.m, generator.m);
}

void mode_select(uint8_t btn)
{
	if(btn == BTN_MODE || btn == BTN_UP)
		generator.m++;
	else if(btn == BTN_DOWN)
	{
		if(generator.m == 0)
			generator.m = M_VER;
		else
			generator.m--;
	}
}

inline void dtmf_start()
{
	uint32_t s1, s2;

	memcpy_P(&buf, sinewave, sizeof(buf));

	s1 = (uint32_t)(generator.dtmf_f1<<16) /(F_CPU/21/0x100);
	s2 = (uint32_t)(generator.dtmf_f2<<16) /(F_CPU/21/0x100);

	dtmf(s1, s2);
}



void dds_main(const char* t, const uint8_t* raw, uint32_t* f, uint32_t* f_save)
{
	uint8_t btn;
	
	#ifdef USE_OMUX
		omux_clr();
	#endif
	nnl_puts_P(t);
	print_on_off(0);			
	print_fq(*f);
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		eeprom_update_dword(f_save, *f);
		save_mode();
		print_on_off(1);
		memcpy_P(&buf, raw, sizeof(buf));
		dds_start(*f);
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_fd(f, 0);
	}
	mode_select(btn);
}

inline void noise_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
		omux_clr();
	#endif
	nnl_puts_P(PSTR("\rNOISE "));
	print_on_off(0);
	
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		print_on_off(1);
		noise_start();
		btn_wait_up_start();
	}
	mode_select(btn);
}

inline void sqw_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	nnl_puts_P(PSTR("\rSQUARE WAVE "));
	print_on_off(0);
	print_fq(generator.f_sqw);
						
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		eeprom_update_dword(&setting.f_sqw, generator.f_sqw);

		print_on_off(1);
		ddssq_start(generator.f_sqw);
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_fd(&generator.f_sqw, 0);
	}
	mode_select(btn);
}

inline void pwm_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	nnl_puts_P(PSTR("\rPWM "));
	print_on_off(is_run);
	print_fq(generator.pwm_f);
	nnl_puts_P(PSTR(" D="));
	print_u(generator.pwm_d,0);
	if(generator.pwm_sync == EXT_HIGHT)
	nnl_puts_P(PSTR(" E=H"));
	else if(generator.pwm_sync == EXT_LOW)
	nnl_puts_P(PSTR(" E=L"));
					
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
						
		if(is_run)
		{
			pwm_stop();
			is_run = 0;
		}
		else
		{
			is_run = 1;
			print_on_off(is_run);
			pwm_start(generator.pwm_f, generator.pwm_d, generator.pwm_sync );
			save_mode();
			eeprom_update_block(&generator.pwm_f,&setting.pwm_f, 1+4+1);
		}
	}
	else if(btn == BTN_SET)
	{
		input_fd(&generator.pwm_f, is_run);
		input_dc(is_run);
		if(!is_run)
		generator.pwm_sync = input_ext_sync();
		//btn_wait_up(BTN_SET);
		//generator.pwm_sync = input_ext_sync();
	}
	if(!is_run)
	mode_select(btn);

}

inline void hs_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	nnl_puts_P(PSTR("\rHIGH SPEED "));
	print_on_off(0);
	print_fq(generator.hs_f);
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		eeprom_update_dword(&setting.hs_f, generator.hs_f);
		print_on_off(1);
		pwm_start(generator.hs_f, 50, 0);
		while(!btnCheck(BTN_START)){};
		pwm_stop();
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		//input_hs(&generator.hs_f);
		input_hs();
	}
	mode_select(btn);
}

inline void pulse_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_clr();
	#endif
	nnl_puts_P(PSTR("\rPULSE "));
	print_on_off(0);
	nnl_puts_P(PSTR("T="));
	print_u((generator.pulse_t_rise + generator.pulse_t_fall + generator.pulse_t_on + generator.pulse_t_off), 0);
	nnl_puts_P(PSTR("us "));
	if(generator.pulse_trig == TR_RISE)
		nnl_puts_P(PSTR("T:R"));
	else if(generator.pulse_trig == TR_FALL)
		nnl_puts_P(PSTR("T:F"));
	//else
	//	nnl_puts_P(PSTR("NO"));


	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		eeprom_update_block(&generator.pulse_t_rise, &setting.pulse_t_rise,
		4 * sizeof(generator.pulse_t_rise) + sizeof(generator.pulse_n) + sizeof(generator.pulse_trig));
	

		lcd_goto(13);
		if(generator.pulse_trig == TR_RISE)
		{
			nnl_puts_P(PSTR("W:R"));
			if(trig_wait_rise() == 0)
			goto p_cancel;
			//break;
		}
		else if(generator.pulse_trig == TR_FALL)
		{
			nnl_puts_P(PSTR("W:F"));
			if(trig_wait_fall() == 0)
			goto p_cancel;
			//break;
		}
		print_on_off(1);
		pulse_start();
		p_cancel:
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_t(&generator.pulse_t_rise,PSTR("\rTrise="));
		input_t(&generator.pulse_t_on,PSTR("\rTon="));
		input_t(&generator.pulse_t_fall,PSTR("\rTfall="));
		input_t(&generator.pulse_t_off,PSTR("\rToff="));
		input_n();
		generator.pulse_trig = input_trig();
	}
	mode_select(btn);	
}

inline void tv_main()
{
	const char* tv_type[] = {
		PSTR("VERTICAL BARS"),
		PSTR("LATTICE"),
		PSTR("CHESSBOARD")
	};
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_clr();
	#endif
	if(generator.tv_type > TV_CHESSBOARD)
		generator.tv_type = 0;
	
	nnl_puts_P(PSTR("\rTV VIDEO\n"));
	nnl_puts_P(tv_type[generator.tv_type]);
	print_on_off(0);
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		eeprom_update_byte(&setting.tv_type, generator.tv_type);
		
		print_on_off(1);
		tv_gen(generator.tv_type);

		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		generator.tv_type++;
	}
	mode_select(btn);
}

inline void dtmf_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_clr();
	#endif
	nnl_puts_P(PSTR("\rDTMF"));
	print_on_off(0);
	print_fq(generator.dtmf_f1);
	n_putchar(' ');
	print_fq(generator.dtmf_f2);
	
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		
		eeprom_update_block(&generator.dtmf_f1, &setting.dtmf_f1, sizeof(generator.dtmf_f1)*2);
		
		print_on_off(1);
		dtmf_start();

		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_fd(&generator.dtmf_f1, 0);
		input_fd(&generator.dtmf_f2, 0);
	}
	mode_select(btn);
	
}



void main()
{
	//uint8_t btn;
	//uint8_t is_run = 0; 	
	
	io_init();	

	_delay_ms(20);

	if(btnCheck(BTN_SET))// сброс настроек при включении с зажатой кнопкой BTN_SET
	{
		//db_puts("reset to default");
		_delay_ms(5000);
		if(btnCheck(BTN_SET))
		{
			btn_wait_up(BTN_SET);
			memcpy_P(&generator, &def_setting, sizeof(generator));
			eeprom_update_block(&generator, &setting, sizeof(generator));
		}
	}

	#ifdef VGEN_ENABLE
		vgen_on();
		_delay_ms(100);
	#endif
	lcd_init();
	lcd_clrscr();
	eeprom_read_block(&generator, &setting, sizeof(generator));

	while(1)
	{
		switch(generator.m)
		{
			case M_SINW:
				dds_main(PSTR("\rSINE "),sinewave,&generator.f_sine, &setting.f_sine);
				break;
				
			case M_STW:
				dds_main(PSTR("\rSAWTOTH "),sawtoothwave,&generator.f_stw, &setting.f_stw);
				break;
				
			case M_RSTW:
				dds_main(PSTR("\rRSAWTOTH "),rsawtoothwave,&generator.f_rstw, &setting.f_rstw);
				break;
				
			case M_TRW:
				dds_main(PSTR("\rTRIANGLE "),trianglewave,&generator.f_trw, &setting.f_trw);
				break;
				
			case M_ECG:
				dds_main(PSTR("\rECG "),ECG,&generator.f_ecg, &setting.f_ecg);
				break;
				
			case M_NOISE:
				noise_main();
				break;
				
			case M_SQW:	
				sqw_main();
				break;
				
			case M_SQPWM:
				pwm_main();
				break;
				
			case M_HSSQW:
				hs_main();
				break;
				
			case M_PULSE:
				pulse_main();
				break;
				
			case M_TV:
				tv_main();
				break;
				
			case M_DTMF:
				dtmf_main();
				break;
				
			case M_VER:
				nnl_puts_P(PSTR("\rNDDS VER"FW_VER"\nSIGNAL GENERATOR"));
				mode_select(btn_wait());
				break;
				
			default:
				generator.m = M_SINW;
				break;
		}
	}

}