/*
 * config.h
 *
 * Created: 21.07.2017 23:17:27
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#include <avr/io.h>
#include "menu.h"


//#define SIM 1
#define FW_VER "0.16"

/*����� ���������� � ���������������� ����*/
//#define USE_SW_UART 1

/*���. ��������� �������������� ����������*/
#define VGEN_ENABLE 1

/*������������ ������� ������ ������*/
#define USE_ENCODER 1

/*���. ����� ��� ���������� ������,
��� ��������� ����������� ������� �� ������ ����� 0,
��������� - 1
*/
#define USE_OMUX 1

//��������� �� ���������
#define DEF_SETTING {\
	M_SINW,\
	1000,\
	1000,\
	1000,\
	1000,\
	1000,\
	1000,\
	1000,\
	50,\
	0,\
	1000000,\
	100,\
	100,\
	100,\
	100,\
	0,\
	0,\
	TV_VBARS,\
	697,\
	1209,\
	20,\
	20000,\
	10,\
	0}\


#define R2RPORT PORTD
#define R2RDDR DDRD

/*������*/

#define START PORTC4
#define MODE PORTC3
#define SET PORTC2
#define UP PORTC1
#define DOWN PORTC0

#define  BTN_PIN PINC
#define  BTN_PORT PORTC
#define  BTN_DDR DDRC

#define BTN_MODE (1<<MODE)
#define BTN_SET (1<<SET)
#define BTN_START (1<<START)
#define BTN_UP (1<<UP)
#define BTN_DOWN (1<<DOWN)

#define BTN_MASK (BTN_SET | BTN_START | BTN_UP | BTN_DOWN | BTN_MODE)


#ifdef USE_SW_UART
	#define BR 135//19200bps 16MHz; see AVR305 appnote 
	#define  UART_PORT PORTB
	#define  UART_PIN PINB
	#define  UART_RX PORTB5
	#define  UART_TX PORTB4	
#endif

#ifdef USE_OMUX
	#define OMUX_PORT PORTB
	#define OMUX_PIN PORTB5
	#define	OMUX_DDR DDRB
#endif

#define omux_set() (OMUX_PORT |= 1<<OMUX_PIN)
#define omux_clr() (OMUX_PORT &= ~(1<<OMUX_PIN))


/*���� E*/
#define EXT_PIN 5
#define EXT_PORT PORTC
#define EXT_PINS PINC
#define EXT_PIN_MSK (1<<EXT_PIN)

/*����� D ��������� �������*/
#define HSDDR DDRB
#define HSPORT PORTB
#define HSPIN PORTB2

/*��������� �������������� ��������� ��� ������� ��*/
#ifdef VGEN_ENABLE
	#define VGEN_FREQ 20000
	#define VGEN_DDR DDRB
	#define VGEN_PORT PORTB	
	#define VGEN_PIN PORTB3
#endif

/*���������� �������� ������� ��� ��������� �������*/
#define MINFREQ 1
#define MAXFREQ_DDS 111111
#define MAXFREQ_PWM 100000
#define MAXFREQ_SQW 500000
#define MAXFREQ_DTMF 10000
#define MIN_T 2
#define MAX_T 1000000

/*��������� LCD ���������� */
#define USE_ADELAY_LIBRARY       0           // Set to 1 to use my ADELAY library, 0 to use internal delay functions
#define LCD_BITS                 4           // 4 for 4 Bit I/O Mode, 8 for 8 Bit I/O Mode
#define RW_LINE_IMPLEMENTED      0           // 0 for no RW line (RW on LCD tied to ground), 1 for RW line present
#define WAIT_MODE                0           // 0=Use Delay Method (Faster if running <10Mhz)
// 1=Use Check Busy Flag (Faster if running >10Mhz) ***Requires RW Line***
#define DELAY_RESET              15          // in mS

#if (LCD_BITS==8)                            // If using 8 bit mode, you must configure DB0-DB7
#define LCD_DB0_PORT           PORTC
#define LCD_DB0_PIN            0
#define LCD_DB1_PORT           PORTC
#define LCD_DB1_PIN            1
#define LCD_DB2_PORT           PORTC
#define LCD_DB2_PIN            2
#define LCD_DB3_PORT           PORTC
#define LCD_DB3_PIN            3
#endif
#define LCD_DB4_PORT             PORTC       // If using 4 bit omde, yo umust configure DB4-DB7
#define LCD_DB4_PIN              3
#define LCD_DB5_PORT             PORTC
#define LCD_DB5_PIN              2
#define LCD_DB6_PORT             PORTC
#define LCD_DB6_PIN              1
#define LCD_DB7_PORT             PORTC
#define LCD_DB7_PIN              0

#define LCD_RS_PORT              PORTB       // Port for RS line
#define LCD_RS_PIN               0           // Pin for RS line

#define LCD_RW_PORT              PORTB       // Port for RW line (ONLY used if RW_LINE_IMPLEMENTED=1)
#define LCD_RW_PIN               1           // Pin for RW line (ONLY used if RW_LINE_IMPLEMENTED=1)

#define LCD_DISPLAYS             1           // Up to 4 LCD displays can be used at one time
// All pins are shared between displays except for the E
// pin which each display will have its own

// Display 1 Settings - if you only have 1 display, YOU MUST SET THESE
#define LCD_DISPLAY_LINES        2           // Number of Lines, Only Used for Set I/O Mode Command
#define LCD_E_PORT               PORTB       // Port for E line
#define LCD_E_PIN                1           // Pin for E line



#endif /* CONFIG_H_ */