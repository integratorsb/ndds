/*
 * menu.h
 *
 * Created: 18.07.2017 2:55:44
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef MENU_H_
#define MENU_H_
#ifndef __ASSEMBLER__
#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <inttypes.h>
#include "dds_rout.h"

 //состояния устройства
 enum{
	 //M_IDLE,
	 M_SINW,	//генератор синуса
	 M_STW,		//генератор пилы
	 M_RSTW,	//генератор обратной пилы
	 M_TRW,		//генератор треугольника
	 M_ECG,		//генератор сигнала ЭКГ
	 M_NOISE,	//генератор шума
	 M_SQW,		//генератор прямоугольника(меандр)
	 M_SQPWM,	//генератор прямоугольника с регулируемой скважностью и частотой
	 M_HSSQW,	//генератор прямоугольника(меандр) 1-8 МГц
	 M_PULSE,	//генератор импульсов заданной формы
	 M_TV,		//генератор ТВ сигнала	 
	 M_DTMF,	//DTMF
	 M_SWEEP,	//ГКЧ
	 M_DPATTERN,//имп. последовательность
	 M_VER		//версия прошивки	 
 };
 
  enum{
	  TR_NO,
	  TR_RISE,
	  TR_FALL
  };

  enum{
	  EXT_NO,
	  EXT_HIGH,
	  EXT_LOW
  };

#define CMD_BRK (int8_t)0
#define CMD_REP (int8_t)-128


 
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
	uint32_t dtmf_f1;
	uint32_t dtmf_f2;
	uint32_t sweep_fs;
	uint32_t sweep_fe;
	uint32_t sweep_fi;
	uint32_t dpattern_t;
	uint8_t dpattern_mode;
 }gen_t;
 
 
 
//структура с настройками
extern gen_t generator;
//буфер 
extern uint8_t buf[256];

extern EEMEM uint8_t e_pattern[256];

extern void pwm_start(uint32_t freq, uint8_t d, uint8_t e);

extern uint8_t pwm_is_run();

void n_putchar(const char c);

/*
выводит текстовую строку из флэш без перевода строки
символ \r выполняет команду clr_lcd
символ \n выполняет переход на след. строку*/
void nnl_puts_P(const char* s);

// макрос для отладки
#define db_puts(s) nnl_puts_P(PSTR(s))

//возвращает состояние кнопки
#define btnCheck(btn) (!(BTN_PIN & btn))

//возвращает маску нажатой кнопки или 0 если ни одна не нажата
uint8_t btnCheckAll();

//ожидает нажатие кнопки и возвращает её маску
uint8_t btn_wait();

//ожидает отпускание кнопки
void btn_wait_up(uint8_t b);

void btn_wait_up_start();

//запускает режим ввода периода
void input_t(uint32_t* p, const char* t);

//запускает режим ввода коэф. заполнения
void input_dc();

//запускает режим ввода высокой частоты 
//void input_hs(uint32_t* p);
void input_hs();

//запускает режим ввода частоты 
void input_fd(const char* t, uint32_t* p);

//запускает режим ввода частоты
void input_f(uint32_t* p);

//запускает режим ввода числа импульсов
void input_n();

//запускает режим ввода типа триггера
uint8_t input_trig();

//запускает режим ввода типа внешней синхр.
uint8_t input_ext_sync();

//uint8_t input_io_mode();

//выводит десятичное число 0...255
void print_u8(uint8_t a);

//выводит десятичное число 
void print_u(uint32_t a);

//выводит значение периода
uint8_t print_t(uint32_t t);

//выводит значение частоты
uint32_t print_fq(uint32_t f);



//brief Выводит строку вкл.выкл на дисплей
//st: 0 - выкл, 1 - вкл.
void print_on_off( uint8_t st);

/**
 * \brief Запускает режим выбора значений из списка
 * 
 * \param t заголовок
 * \param v указатель на массив с указателями на строки значений
 * \param n число элементов в v
 * 
 * \return uint8_t номер выбранного элемента v
 */
uint8_t select_val(const char* t, const char* v[], uint8_t n);

//запускает режим ввода цифр. последовательности
void input_dpattern(void);


#endif 

#endif /* MENU_H_ */