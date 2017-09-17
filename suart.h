/*
 * suart.h
 *
 * Created: 13.07.2017 0:52:59
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef SUART_H_
#define SUART_H_
#include "config.h"

#ifndef CONFIG_H_
	#define BR 135                //19200 bps@16MHz 

	#define  UART_PORT PORTB
	#define  UART_PIN PINB
	#define  UART_RX PORTB5
	#define  UART_TX PORTB4
#endif

#ifdef __ASSEMBLER__

//Определения только для ассемблера

#endif

#ifndef __ASSEMBLER__

void SerialPutChar(char c);
char SerialGetChar();

#endif

#endif /* SUART_H_ */