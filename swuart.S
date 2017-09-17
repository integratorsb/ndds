//UART Functions per AVR Application Note #305
#include <avr/io.h>
#include "suart.h"
            
UARTDelay:
        ldi  r17, BR
UARTDelay1:
        dec  r17
        brne UARTDelay1
        ret
 
//
//asynchronous 8n1 serial transmit byte
//
//#define TX_PIN 6                //TX pin is PA6
.global SerialPutChar
SerialPutChar:
        push r16
        push r17
        ldi  r16, 10        ;1 start + 8 data + 1 stop bits (bit count)
        com  r24            ;Invert it (r24 = xmit byte)
        sec             ;Start bit
putchar0:
        brcc putchar1       ;If carry set
        cbi  _SFR_IO_ADDR(UART_PORT), UART_TX ;send a '0'
        rjmp putchar2       ;else
putchar1:
        sbi  _SFR_IO_ADDR(UART_PORT), UART_TX ;send a '1'
        nop
putchar2:
        rcall UARTDelay     ;1/2 bit delay +
        rcall UARTDelay     ;1/2 bit delay = 1bit delay
        lsr   r24           ;Get next bit
        dec   r16           ;If not all bits sent
        brne  putchar0      ;send next
                        ;else
        pop   r17
        pop   r16
        ret             ;return
 

.global SerialGetChar
SerialGetChar:
        push  r16
        push  r17
        ldi   r16, 9        ;8 data bits + 1 stop bit
getchar1:
        sbic  _SFR_IO_ADDR(UART_PIN), UART_RX ;Wait for start bit
        rjmp  getchar1
        rcall UARTDelay     ;1.5bit delay after start bit edge
getchar2:
        rcall UARTDelay     ;1 bit delay from here
        rcall UARTDelay
        clc             ;clear carry
        sbic  _SFR_IO_ADDR(UART_PIN), UART_RX ;if RX pin high
        sec             ;
        dec   r16           ;If bit is stop bit
        breq  getchar3      ;   return
                        ;else
        ror   r24           ;   shift bit into r24 (rec'd byte)
        rjmp  getchar2      ;   go get next
getchar3:
        ldi   r25, 0x00     ;zero extend r24:r25 for return
        pop   r17
        pop   r16
        ret