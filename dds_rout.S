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


 dds:
	push r18
	push r19
;FOUT = M * (F_CPU/N_CLOCK)/(2 * 2^24) 
;��� F_CPU - �������� ������� ����������
; M - ���������� ����
;N_CLOCK - ����� �������� ������ �� ���� ������ ����� 

	
;� ������� Z ����������� ����� ������ � ���, ���������� ������� ������ ������� ������������� �������
;��� ���������� ������ ��������� ����� ������ ������ ���� �������� �� 0x100h, �.��. ���������� � 0xXX00h
;� ������ ������ ����� ���������� �� ������ 0x0100 
	ldi zh, 0x01
	ldi zl, 0
	
;��������  r18,  r19, zl ���������� 24������ ������� ������������
;������� 8 �������� ����� �������� ������� � ������� �������
	eor r18, r18 ;��������� ������������
	eor r19, r19 ;��������� ������������
	
;� ��������� r22, r23, r24 ���������� 24������ �������� ���������� ���� - M

	1:
	add r18, r22 ;�������� ����������� �� ��������� ���������� ����
	adc r19, r23 ;�������� ����������� �� ��������� ���������� ����
	adc zl, r24 ;�������� ����������� �� ��������� ���������� ����
	ld r0, z	;�������� �������� �� ������� �� ������ zl
	out _SFR_IO_ADDR(R2RPORT), r0 ;����� �������� � ���
	sbic _SFR_IO_ADDR(BTN_PIN), START ;������� ��� ���������� �����,���� ������ ������ �������
	rjmp 1b		;������ �� ������ �����
	
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

	;� ������� z ��������� ��������� �� pulse_t
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