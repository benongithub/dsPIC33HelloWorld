/* 
 * File:   IOconfig.h
 * Author: adamp
 *
 * Created on 13 December 2019, 09:38
 */

#ifndef IOCONFIG_H
#define	IOCONFIG_H


#define LED4 LATBbits.LATB15
#define LED5 LATBbits.LATB14
#define LED6 LATBbits.LATB13
#define LED7 LATBbits.LATB12

#define SW1 !PORTBbits.RB5 


#define LEDON 0
#define LEDOFF 1

void setupIO();
void UART_PutChar(char c);
int write(int handle, void *buffer, unsigned int len);


#endif	/* IOCONFIG_H */

