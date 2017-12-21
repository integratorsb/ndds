/*
 * dds_rout.h
 *
 * Created: 16.07.2017 2:45:40
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef DDS_ROUT_H_
#define DDS_ROUT_H_

#include <avr/io.h>
#include "config.h"

// старший байт адреса буфера в памяти 
// буфер должен быть выровнен на 0x100
#define BUF_BASE 0x01

#define NUM_CYCLES 9

#ifndef __ASSEMBLER__

typedef struct{
	uint32_t t_rise;
	uint32_t t_on;
	uint32_t t_fall;
	uint32_t t_off;
	uint16_t n;
}pulse_t;

void dds(uint32_t p);
void dds_sq(uint32_t p);
void pulse(pulse_t* p);
void dtmf(uint32_t p1, uint32_t p2 );
void sweep(uint32_t f_start, uint32_t f_end, uint32_t f_inc);
//void sweep(uint32_t p[]);

#endif



#endif /* DDS_ROUT_H_ */