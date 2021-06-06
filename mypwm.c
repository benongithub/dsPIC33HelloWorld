/*
 * File:   mypwm.c
 * Author: ga63wic
 *
 * Created on 17. Mai 2021, 15:13
 */

#include "mypwm.h"
#include "xc.h"

void setupPWM()
{
    P1TCONbits.PTEN = 0;
    P1TCONbits.PTCKPS = 0b00;
    P1TPER = 10000;
    PWM1CON1bits.PMOD1 = 1;
    
    PWM1CON1bits.PEN1H = 1;
    PWM1CON1bits.PEN2H = 0;
    PWM1CON1bits.PEN3H = 0;
    PWM1CON1bits.PEN1L = 0;
    PWM1CON1bits.PEN2L = 0;
    PWM1CON1bits.PEN3L = 0;
    
    P1TCONbits.PTEN = 1;
    P1DC1 = 0;
    P1DC2 = 0;
    P1DC3 = 0;    
}
