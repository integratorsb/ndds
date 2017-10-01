/*
 * menu.h
 *
 * Created: 18.07.2017 2:55:44
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef MENU_H_
#define MENU_H_
#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include "dds_rout.h"

 //��������� ����������
 enum{
	 //M_IDLE,
	 M_SINW,	//��������� ������
	 M_STW,		//��������� ����
	 M_RSTW,	//��������� �������� ����
	 M_TRW,		//��������� ������������
	 M_ECG,		//��������� ������� ���
	 M_NOISE,	//��������� ����
	 M_TV,		//��������� �� �������
	 M_SQW,		//��������� ��������������(������)
	 M_SQPWM,	//��������� �������������� � ������������ ����������� � ��������
	 M_HSSQW,	//��������� ��������������(������) 1-8 ���
	 M_PULSE,	//��������� ��������� �������� �����
	 M_VER		//������ ��������	 
 };
 
  enum{
	  TR_NO,
	  TR_RISE,
	  TR_FALL
  };

  enum{
	  EXT_NO,
	  EXT_HIGHT,
	  EXT_LOW
  };


 
typedef struct  {
	uint8_t m;
	uint32_t f_sine;
	uint32_t f_stw;
	uint32_t f_rstw;
	uint32_t f_trw;
	uint32_t f_ecg;
	uint32_t f_sqw;
	uint32_t pwm_f;
	uint8_t pwm_d;
	uint8_t pwm_sync;
	uint32_t hs_f;
	uint32_t pulse_t_rise;
	uint32_t pulse_t_on;
	uint32_t pulse_t_fall;
	uint32_t pulse_t_off;
	uint16_t pulse_n;
	uint8_t pulse_trig;
	uint8_t tv_type;
 }gen_t;
 
//��������� � �����������
extern gen_t generator;
//����� 
extern uint8_t buf[256];

extern void pwm_start(uint32_t freq, uint8_t d, uint8_t e);

void n_putchar(char c);
/*
������� ��������� ������ �� ���� ��� �������� ������
������ \r ��������� ������� clr_lcd
������ \n ��������� ������� �� ����. ������*/
void nnl_puts_P(const char* s);

// ������ ��� �������
#define db_puts(s) nnl_puts_P(PSTR(s))

//���������� ��������� ������
#define btnCheck(btn) (!(BTN_PIN & btn))

//���������� ����� ������� ������ ��� 0 ���� �� ���� �� ������
uint8_t btnCheckAll();

//������� ������� ������ � ���������� � �����
uint8_t btn_wait();

//������� ���������� ������
void btn_wait_up(uint8_t b);

//��������� ����� ����� �������
void input_t(uint32_t* p, const char* t);

//��������� ����� ����� ����. ����������
void input_dc(uint8_t is_run);

//��������� ����� ����� ������� ������� 
//void input_hs(uint32_t* p);
void input_hs();

//��������� ����� ����� ������� 
//void input_fd(uint32_t* p);
void input_fd(uint32_t* p, uint8_t is_run);

//��������� ����� ����� ����� ���������
void input_n();

//��������� ����� ����� ���� ��������
uint8_t input_trig();

//��������� ����� ����� ���� ������� �����.
uint8_t input_ext_sync();

//������� ���������� �����
/**
 * \brief ������� ���������� �����
 * 
 * \param a �����
 * \param p ����� �������, ����� �������� ����� �������� ���. �����, 0 - ���
 * 
 * \return void
 */
void print_u(uint32_t a, uint8_t p);


/**
 * \brief ������� �������� �������
 * 
 * \param f �������
 * 
 * \return uint16_t ��������� f 
 */
uint16_t print_fq(uint32_t f);


/**
 * \brief ������� ������ ���.���� �� �������
 * 
 * \param st 0 - ����, 1 - ���.
 * 
 * \return void
 */
void print_on_off( uint8_t st);

/**
 * \brief ��������� ����� ������ �������� �� ������
 * 
 * \param t ���������
 * \param v ��������� �� ������ � ����������� �� ������ ��������
 * \param n ����� ��������� � v
 * 
 * \return uint8_t ����� ���������� �������� v
 */
uint8_t select_val(const char* t, const char* v[], uint8_t n);



#endif /* MENU_H_ */