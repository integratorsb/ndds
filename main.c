
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
EEMEM uint8_t e_pattern[256] = DEF_DPATTERN;

uint8_t __attribute__ ((section (".buffer"))) buf[256]; 


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
/*

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
};*/
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
	//DDRB |= 1<<PORTB3;
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

void io_to_default()
{
	EXT_DDR &= ~(1<<EXT_PIN);
	EXT_PORT |= (1<<EXT_PIN);
	HSPORT &= ~(1<<HSPIN);
	HSDDR |= 1<<HSPIN;
	R2RPORT = 0;
	R2RDDR = 0xff;
}

inline void io_init()
{
	io_to_default();
	//DDRB |=  (1<<HSPORT);
	
	#ifdef USE_OMUX
	DDRB |= (1<<OMUX_PIN);
	#endif
	
	#ifdef VGEN_ENABLE
	DDRB |= (1<<VGEN_PIN);
	#endif
	
	#ifdef USE_SWUART
	DDRB |= (0<<UART_RX) | (1<<UART_TX);
	#endif
	
	//PORTB = 0<<HSPORT;
/*
	#ifdef VGEN_ENABLE
	PORTB &= ~(1<<VGEN_PIN);
	#endif*/
	
	#ifdef USE_SWUART
	PORTB |= 1<<UART_RX | 1<<UART_TX	
	#endif
	
	//DDRC =  0<<EXT_PIN;
	PORTC = BTN_MASK | 0<<EXT_PIN;
	
	//R2RPORT = 0;
	//R2RDDR = 0xff;
}


inline void noise_start()
{
	while(!btnCheck(BTN_START))
	{
		R2RPORT = rand();
	}
	io_to_default();
}

void pwm_stop()
{
	TCCR1A = 0;
	TCCR1B = 0;
	io_to_default();
}


void pwm_start(uint32_t freq, uint8_t d, uint8_t e )
{
	uint32_t period;
	uint8_t cntr;
	
	
	//fast PWM	
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM13)  | (1<<WGM12);

	period = (F_CPU/freq) - 1;	
	
	cntr = (TCCR1B & 0b11111000);

	if(period < 0xffff)
	{
		
		//cntr = (TCCR1B & 0b11111000) | (1<<CS10);//��� ��������
		cntr |= (1<<CS10);//��� ��������
	}
	else
	{
		period /= 256;
		//cntr = (TCCR1B & 0b11111000) | (1<<CS12);//�������� 256
		cntr |= (1<<CS12);//�������� 256
		
	}
	
	ICR1 = (uint16_t)period;
	OCR1B =  (uint16_t)(period * d / 100);	
	TCCR1B = cntr;
	
	TCCR1A |= (1<<COM1B1);

	//	if(e == EXT_NO)
	//		return;
	if(e == EXT_HIGH)
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
	//else return;
	//pwm_stop();
}

uint8_t pwm_is_run() 
{
	return (TCCR1B != 0);
}

uint32_t get_phase_f(uint32_t f)
{
	//return ((uint64_t)f << 16) / (F_CPU/NUM_CYCLES/0x100);
	//return (((uint64_t)f<<24 )/(F_CPU/NUM_CYCLES));	
	return (((uint64_t)f * (0x1000000*NUM_CYCLES) )/F_CPU);

}

inline void dds_start(uint32_t f)
{

	dds(get_phase_f(f));
	io_to_default();
}

inline void ddssq_start(uint32_t f)
{

	//HSPORT &= ~(1<<HSPIN);
	//HSDDR |= 1<<HSPIN;
	dds_sq(get_phase_f(f) * 2);
	//HSPORT &= ~(1<<HSPIN);
	//HSDDR &= ~(1<<HSPIN);
	io_to_default();

}


void get_phase_t( uint32_t *p, uint32_t *t)
{
/*
	 T0 = 1000000<<16 / (F_CPU / NUM_CYCLES)
	 P = 1<<24 / (T<<16 / T0)*/
	//*p =  0x1000000/(((uint64_t)*t<<16)/((uint64_t)(1000000<<16) / (F_CPU / NUM_CYCLES)));
	//*p =  0x1000000/(((uint64_t)*t<<16)/36864);	
	//*p = ((1000000 * (uint64_t)NUM_CYCLES * 0x1000000) / F_CPU) / *t;
	*p = ((1000000 * (uint64_t)NUM_CYCLES * 0x100000000) / F_CPU) / *t;
	
}

inline void pulse_start()
{
	pulse_t pulse_s;
	
	get_phase_t(&pulse_s.t_rise, &generator.pulse_t_rise);
	get_phase_t(&pulse_s.t_on,&generator.pulse_t_on);
	get_phase_t(&pulse_s.t_fall,&generator.pulse_t_fall);
	get_phase_t(&pulse_s.t_off,&generator.pulse_t_off);
	pulse_s.n = generator.pulse_n;
	pulse(&pulse_s);
	io_to_default();

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

void load_table(const uint8_t* src)
{
	memcpy_P(&buf, src, sizeof(buf));
}


inline void dtmf_start()
{
	load_table(sinewave);
	
	dtmf((uint64_t)(generator.dtmf_f1<<16) /(F_CPU/21/0x100), \
	(uint64_t)(generator.dtmf_f2<<16) /(F_CPU/21/0x100));
	io_to_default();
}



void dds_main(const char* t, uint32_t* f, uint32_t* f_save)
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
		dds_start(*f);
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_f(f);
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
	nnl_puts_P(PSTR("\rSQUARE WAVE"));
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
		input_f(&generator.f_sqw);
	}
	mode_select(btn);
}

inline void pwm_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	io_to_default();
	//st = pwm_is_run();
	
	nnl_puts_P(PSTR("\rPWM"));
	print_on_off(pwm_is_run());
	print_fq(generator.pwm_f);
	nnl_puts_P(PSTR(" D="));
	print_u8(generator.pwm_d);
	if(generator.pwm_sync == EXT_HIGH)
		nnl_puts_P(PSTR(" E=H"));
	else if(generator.pwm_sync == EXT_LOW)
		nnl_puts_P(PSTR(" E=L"));
					
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
						
		if( pwm_is_run())
		{
			pwm_stop();
		}
		else
		{

			print_on_off(1);
			pwm_start(generator.pwm_f, generator.pwm_d, generator.pwm_sync );
			save_mode();
			eeprom_update_block(&generator.pwm_f,&setting.pwm_f, 1+4+1);
		}
	}
	else if(btn == BTN_SET)
	{
		input_f(&generator.pwm_f);
		input_dc();
		if(!pwm_is_run())
			generator.pwm_sync = input_ext_sync();
	}
	if(!pwm_is_run())
		mode_select(btn);

}

inline void hs_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	nnl_puts_P(PSTR("\rHIGH SPEED"));
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
	if(generator.pulse_trig == TR_RISE)
		nnl_puts_P(PSTR("T:R"));
	else if(generator.pulse_trig == TR_FALL)
		nnl_puts_P(PSTR("T:F"));
	print_on_off(0);		
	print_t(generator.pulse_t_rise + generator.pulse_t_fall\
	+ generator.pulse_t_on + generator.pulse_t_off);

	
/*
	nnl_puts_P(PSTR("\rPULSE\nT="));
	print_t(generator.pulse_t_rise + generator.pulse_t_fall\
		 + generator.pulse_t_on + generator.pulse_t_off);
	if(generator.pulse_trig == TR_RISE)
		nnl_puts_P(PSTR(" T:R"));
	else if(generator.pulse_trig == TR_FALL)
		nnl_puts_P(PSTR(" T:F"));*/

	
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
		input_fd(PSTR("\rF1="), &generator.dtmf_f1);
		input_fd(PSTR("\rF2="), &generator.dtmf_f2);
	}
	mode_select(btn);
	
}

inline void sweep_main()
{
	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_clr();
	#endif
	//HSDDR |= 1<<HSPIN;
	nnl_puts_P(PSTR("\rSWEEP"));
	print_on_off(0);
	print_fq(generator.sweep_fs);
	n_putchar('>');
	print_fq(generator.sweep_fe);

	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		save_mode();
		load_table(sinewave);
		eeprom_update_block(&generator.sweep_fs, &setting.sweep_fs,
		3 * sizeof(generator.sweep_fs));
		print_on_off(1);		

		do
		{
			sweep(get_phase_f(generator.sweep_fs),\
				get_phase_f(generator.sweep_fe),\
				get_phase_f(generator.sweep_fi));
		} while (!btnCheck(BTN_START));
		io_to_default();
		print_on_off(0);
		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_fd(PSTR("\rFstart="),&generator.sweep_fs);
		input_fd(PSTR("\rFend="), &generator.sweep_fe);
		input_fd(PSTR("\rFinc="), &generator.sweep_fi);
	}
	mode_select(btn);
}


// ��������� ������� ��� ����
// �������� ��������� 1 - ������ ����, -1 ��������
void load_sawtooth_wf(int8_t a)
{

	uint8_t i = 0, b = 0;

	do 
	{
		buf[i] = b;
		b += a;
		i++;
	} while (i);
}

inline void load_triangle_wf()
{
	uint8_t i = 1,  b = 1;

	while(i)
	{
		buf[i] = b;
		if(i<128)
			b += 2;
		else
			b -= 2;	
			
		i++;	
	}
	buf[i] = 0;
}


inline void dpattern_d(uint32_t t0)
{
	uint8_t  i = 0;
	int8_t p;
	uint32_t t;
	

	
	TCCR1A = 0;
	//��������� ����� ������ ������� ��� t0	
	t = t0 * (F_CPU/1000000) - 1;

	if(t < 0xFFFF)
	{
		TCCR1B = 1<<WGM12 | 0<<CS12 | 0<<CS11 | 1<<CS10; // CTC ��� ��������	
	}
	else 
	{
		TCCR1B = 1<<WGM12 | 1<<CS12 | 0<<CS11 | 0<<CS10; // CTC �������� 256
		t /= 256;
	}	
	OCR1A = t;	

	while(1)
	{
		p = buf[i++];
		if(p == CMD_REP)
		{
			if(i == 0)//�����, ���� ������� ������� ���� ������
				p = CMD_BRK;
			i = 0;
			continue;
		}	
		else if (p == CMD_BRK)
			break;
		else if(p > 0)
		{
			HSDDR &= ~(1<<HSPIN);			
		}
		else
		{
			HSDDR |= (1<<HSPIN);
			p = -p;
		}
		
		//TIFR |= (1<<OCF1A);
		//_delay_us(5);	
			
		while(p != 0)
		{
			TIFR |= (1<<OCF1A);			
			while(!(TIFR & (1<<OCF1A)))
			{
				if(btnCheck(BTN_START))	goto dpattern_stop;
			}
			p--;
		}
		
	}
dpattern_stop:
	HSDDR &= ~(1<<HSPIN);
	TCCR1B = 0;
}

inline void dpattern_main()
{

	uint8_t btn;
	
	#ifdef USE_OMUX
	omux_set();
	#endif
	
	HSDDR &= ~(1<<HSPIN);
	HSPORT &= ~(1<<HSPIN);
	
	nnl_puts_P(PSTR("\rDPATTERN\nT0="));	
	print_t(generator.dpattern_t);
	print_on_off(0);	
	
	btn = btn_wait();
	if(btn == BTN_START)
	{
		btn_wait_up_start();
		
		eeprom_read_block(buf, e_pattern, sizeof(buf));
		save_mode();
		eeprom_update_dword(&setting.dpattern_t, generator.dpattern_t);
		
		print_on_off(1);
		
		dpattern_d(generator.dpattern_t);

		btn_wait_up_start();
	}
	else if(btn == BTN_SET)
	{
		input_t(&generator.dpattern_t,PSTR("\rT0="));
		//generator.dpattern_mode = input_io_mode();
		input_dpattern();
		
	}
	mode_select(btn);
	if(generator.m  != M_DPATTERN)
	{
		io_to_default();
	}
}



int main()
{

	
	io_init();	
	_delay_ms(20);





#ifndef USE_ENCODER
	// ����� �������� ��� ��������� � ������� ������� BTN_SET
	
	uint8_t  c = 50;
	while(btnCheck(BTN_SET))
	{

		_delay_ms(100);
		c--;
		if(c == 0)
		{
			//db_puts("reset to default");
			btn_wait_up(BTN_SET);
			memcpy_P(&generator, &def_setting, sizeof(generator));
			eeprom_update_block(&generator, &setting, sizeof(generator));
			break;
		}
/*
		_delay_ms(5000);
		if(btnCheck(BTN_SET))
		{
			//db_puts("reset to default");
			btn_wait_up(BTN_SET);
			memcpy_P(&generator, &def_setting, sizeof(generator));
			eeprom_update_block(&generator, &setting, sizeof(generator));
			break;
		}*/
	}
#else
	//����� �������� ��� ��������� � ������� ������� BTN_START
		
	uint8_t  c = 50;
	while(btnCheck(BTN_START))
	{
		c--;
		_delay_ms(100);
		if(c == 0)
		{
			//db_puts("reset to default");
			btn_wait_up(BTN_START);
			memcpy_P(&generator, &def_setting, sizeof(generator));
			eeprom_update_block(&generator, &setting, sizeof(generator));
			break;
		}
	}
#endif	

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
				load_table(sinewave);
				dds_main(PSTR("\rSINE "),&generator.f_sine, &setting.f_sine);
				break;
				
			case M_STW:
				//memcpy_P(&buf, sawtoothwave, sizeof(buf));
				load_sawtooth_wf(1);
				dds_main(PSTR("\rSAWTOOTH "),&generator.f_stw, &setting.f_stw);
				break;
				
			case M_RSTW:
				//memcpy_P(&buf, rsawtoothwave, sizeof(buf));
				load_sawtooth_wf(-1);
				dds_main(PSTR("\rRSAWTOOTH "),&generator.f_rstw, &setting.f_rstw);
				break;
				
			case M_TRW:
				//memcpy_P(&buf, trianglewave, sizeof(buf));
				load_triangle_wf();
				dds_main(PSTR("\rTRIANGLE "),&generator.f_trw, &setting.f_trw);
				break;
				
			case M_ECG:
				load_table(ECG);
				dds_main(PSTR("\rECG "),&generator.f_ecg, &setting.f_ecg);
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
				
			case M_SWEEP:
				sweep_main();
				break;

			case M_DPATTERN:
				dpattern_main();
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
	
	return 0;
}