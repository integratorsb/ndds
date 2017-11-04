/*
 * dds_rout.S
 *
 * Created: 16.07.2017 2:45:40
 *  Author: integrator.sb@gmail.com
 */ 
 #include "dds_rout.h"

 .global dds
 .global dds_sq
 .global pulse
 .global dtmf


 dds:
	push r18
	push r19
;FOUT = M * (F_CPU/N_CLOCK)/(2^24) 
;где F_CPU - тактовая частота процессора
; M - приращение фазы
;N_CLOCK - число машинных тактов за один проход цикла 

	
;в регистр Z загружается адрес буфера в ОЗУ, содержащий таблицу одного периода генерируемого сигнала
;для правильной работы алгоритма адрес буфера должен быть выровнен на 0x100h, т. е. начинаться с 0xXX00h
;в данном случае буфер находиться по адресу 0x0100 
	ldi zh, 0x01
	ldi zl, 0
	
;регистры  r18,  r19, zl составляют 24битный фазовый аккумулятора
;старшие 8 разрядов также являются адресом в таблице сигнала
	eor r18, r18 ;обнуление аккумулятора
	eor r19, r19 ;обнуление аккумулятора
	
;в регистрах r22, r23, r24 находиться 24битное значение приращения фазы - M

	1:
	add r18, r22 ;сложение аккумулятор со значением приращения фазы
	adc r19, r23 ;сложение аккумулятор со значением приращения фазы
	adc zl, r24 ;сложение аккумулятор со значением приращения фазы
	ld r0, z	;загрузка значения из таблицы по адресу zl
	out _SFR_IO_ADDR(R2RPORT), r0 ;вывод значения в ЦАП
	sbic _SFR_IO_ADDR(BTN_PIN), START ;условие для перрывания цикла,если кнопка нажата выходим
	rjmp 1b		;прыжок на начало цикла
	
	pop r19
	pop r19
	ret


dds_sq:
	push r18
	push r19

	ldi zh, 1<<HSPIN
	ldi zl, 0

	eor r18, r18
	eor r19, r19
	in r0, _SFR_IO_ADDR(HSPORT)
	1:
	add r18, r22
	adc r19, r23
	adc zl, r24
	BRCC 2f
	eor r0, zh
	2:
	out _SFR_IO_ADDR(HSPORT), r0 
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp 1b
	pop r19
	pop r19
	ret

pulse:
	push xl
	push xh
	push r23
	push r22
	push r21
	push r20
	push r19
	push r18

	;в регистр z загружаем указатель на pulse_t
	mov zh, r25
	mov zl, r24
	;n
	ldd xl, z+16
	ldd xh, z+17
	clt
	tst xh
	brne next
	tst xl
	brne next
	set

	next:
	ldd r21, z+0
	ldd r22, z+1
	ldd r23, z+2
	clr r18
	clr r19
	clr r20
	t1:
	add r18, r21
	adc r19, r22
	adc r20, r23
	BRCS t2i
	nop
	out _SFR_IO_ADDR(R2RPORT), r20 
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t1
	rjmp exit

	t2i:
	ldi r18, 0xff
	out _SFR_IO_ADDR(R2RPORT), r18 
	ldd r21, z+4
	ldd r22, z+5
	ldd r23, z+6
	clr r18
	clr r19
	clr r20

	;mov r0,r1
	;neg r0 
	;out _SFR_IO_ADDR(R2RPORT), r0 
	t2:
	add r18, r21
	adc r19, r22
	adc r20, r23
	BRCS t3i
	nop
	nop
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t2
	rjmp exit

	t3i:
	ldd r21, z+8
	ldd r22, z+9
	ldd r23, z+10
	ser r18
	ser r19
	ser r20

	t3:
	sub r18, r21
	sbc r19, r22
	sbc r20, r23
	tst r20
	breq t4i
	nop
		out _SFR_IO_ADDR(R2RPORT), r20 
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t3
	rjmp exit

	t4i:
	ldd r21, z+12
	ldd r22, z+13
	ldd r23, z+14
	clr r18
	clr r19
	clr r20
	t4:
	add r18, r21
	adc r19, r22
	adc r20, r23
	BRCS t5
	nop
	nop
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t4
	rjmp exit

	t5:
	BRTS 1f
	sbiw X,1
	breq exit
	1:
	rjmp next


	exit:
	out _SFR_IO_ADDR(R2RPORT), r1
	pop r18
	pop r19
	pop r20
	pop r21
	pop r22
	pop r23
	pop xh
	pop xl
	ret

;===================

#define x_a r25
#define x_b r21

dtmf:
	push xl
	push xh

	;push r23
	;push r22

	;push r21
	;push r20
	;push r19
	;push r18

	push r17
	push r16
	push r15
	push r14
	;push r8
	;push r7

	out _SFR_IO_ADDR(R2RPORT),r1
	;загрузка указателей для s1,s2
	ldi zh, 0x01
	ldi zl, 0
	mov xh, zh
	mov xl, zl

	mov r17,r1
	mov r16,r1
	mov r15,r1
	mov r14,r1



	1:
	add r16, r22 ;сложение аккумулятор со значением приращения фазы
	adc r17, r23 ;сложение аккумулятор со значением приращения фазы
	adc zl, r24 ;сложение аккумулятор со значением приращения фазы
	ld r0, z	;загрузка значения из таблицы по адресу zl
	;в регистр z загружаем sw2
	lsr r0
	mov x_a, r0
	lsr r0
	lsr r0
	sub x_a, r0


	;в регистр z загружаем sw1
	add r14, r18 ;сложение аккумулятор со значением приращения фазы
	adc r15, r19 ;сложение аккумулятор со значением приращения фазы
	adc xl, r20 ;сложение аккумулятор со значением приращения фазы
	ld x_b, x	;загрузка значения из таблицы по адресу zl
	lsr x_b
	add x_b, x_a

	out _SFR_IO_ADDR(R2RPORT),x_b
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp 1b
	out _SFR_IO_ADDR(R2RPORT),r1

	;pop r7
	;pop r8
	pop r14
	pop r15
	pop r16
	pop r17
	;pop r18
	;pop r19
	;pop r20
	;pop r21
	;pop r22
	;pop r23
	pop xh
	pop xl
	ret