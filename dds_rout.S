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
 .global sweep
 

 dds:
	push r18
	push r19
;FOUT = M * (F_CPU/N_CLOCK)/(2^24) 
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
	;push r18
	;push r19

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
	;pop r19
	;pop r19
	ret

pulse:
	push xl
	push xh
	push r17
	push r16
	;push r23
	;push r22
	;push r21
	;push r20
	;push r19
	;push r18

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
	ldd r20, z+0
	ldd r21, z+1
	ldd r22, z+2
	ldd r23, z+3
	clr r16
	clr r17
	clr r18
	clr r19
	t1:
	add r16, r20
	adc r17, r21
	adc r18, r22
	adc r19, r23

	BRCS t2i
	;nop
	out _SFR_IO_ADDR(R2RPORT), r19 
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t1
	rjmp exit

	t2i:
	ldi r18, 0xff
	out _SFR_IO_ADDR(R2RPORT), r18 
	ldd r20, z+4
	ldd r21, z+5
	ldd r22, z+6
	ldd r23, z+7
	clr r16
	clr r17
	clr r18
	clr r19

	;mov r0,r1
	;neg r0 
	;out _SFR_IO_ADDR(R2RPORT), r0 
	t2:
	add r16, r20
	adc r17, r21
	adc r18, r22
	adc r19, r23
	BRCS t3i
	;nop
	nop
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t2
	rjmp exit

	t3i:
	ldd r20, z+8
	ldd r21, z+9
	ldd r22, z+10
	ldd r23, z+11
	ser r16
	ser r17
	ser r18
	ser r19

	t3:
	sub r16, r20
	sbc r17, r21
	sbc r18, r22
	sbc r19, r23
	;tst r20
	brcs t4i
	;nop
		out _SFR_IO_ADDR(R2RPORT), r19 
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp t3
	rjmp exit

	t4i:
	ldd r20, z+12
	ldd r21, z+13
	ldd r22, z+14
	ldd r23, z+15
	clr r16
	clr r17
	clr r18
	clr r19
	t4:
	add r16, r20
	adc r17, r21
	adc r18, r22
	adc r19, r23
	BRCS t5
	out _SFR_IO_ADDR(R2RPORT), r1
	;nop
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
	;out _SFR_IO_ADDR(R2RPORT), r1
	;pop r18
	;pop r19
	;pop r20
	;pop r21
	;pop r22
	;pop r23
	pop r16
	pop r17
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
	;�������� ���������� ��� s1,s2
	ldi zh, 0x01
	ldi zl, 0
	mov xh, zh
	mov xl, zl

	mov r17,r1
	mov r16,r1
	mov r15,r1
	mov r14,r1

	1:
	add r16, r22 
	adc r17, r23 
	adc zl, r24 
	ld r0, z	
	lsr r0
	mov x_a, r0
	lsr r0
	lsr r0
	sub x_a, r0

	add r14, r18 
	adc r15, r19 
	adc xl, r20 
	ld x_b, x	
	lsr x_b
	add x_b, x_a

	out _SFR_IO_ADDR(R2RPORT),x_b
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp 1b
	;out _SFR_IO_ADDR(R2RPORT),r1

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


;====================================================
;�� ������ ���� https://github.com/dev26th/avr_dds_20
sweep:	

	ldi zl, 0
	ldi zh, BUF_BASE

	;clr a
	mov r25, r1
	mov r21,r1
	;D=1
	sbi  _SFR_IO_ADDR(HSPORT), HSPIN	
	2:
	add r25, r22
	adc r21, r23 
	adc zl, r24
	brcs 3f 
	ld r0, z	
	out _SFR_IO_ADDR(R2RPORT), r0 
	rjmp 2b
	3:
	ld r0, z	
	out _SFR_IO_ADDR(R2RPORT), r0 

	;a + incr
	add r22, r14
	adc r23, r15
	adc r24, r16

	; end reached? any continue branch takes 10 cycles
	cp r20, r24	
	;brlo 10f			; a2 > e2 : exit
	brlo 10f
	brne 4f			;  a2 == e2 
	cp r19, r23		
	;brlo 10f			;  a1 > e1 : exit
	brlo 10f
	brne 5f			; a1 == e1 : check e0/a0
	cp r18, r22		
	;brlo 10f			; // a0 > e0 : exit
	brlo 10f
	rjmp 6f	
	4:            
	nop 
	nop
	nop
	5: 
	nop  
	nop  
	nop 

	add r25, r22
	adc r21, r23 
	adc zl, r24
	add r25, r22
	adc r21, r23 
	adc zl, r24
	add r25, r22
	adc r21, r23 
	adc zl, r24
	add r25, r22
	adc r21, r23 
	adc zl, r24
	add r25, r22
	adc r21, r23 
	adc zl, r24
	nop
	6:

	brne 7f			
	add r25, r22
	adc r21, r23 
	adc zl, r24
	nop  
	nop
	nop
	rjmp 6b			
	7: 
	; output the new value
	ld r0, Z	
	out _SFR_IO_ADDR(R2RPORT), r0 ; output

	; check exit condition
	sbic _SFR_IO_ADDR(BTN_PIN), START
	rjmp 2b		
	10:
	;D=0
	cbi  _SFR_IO_ADDR(HSPORT), HSPIN	

	ret

