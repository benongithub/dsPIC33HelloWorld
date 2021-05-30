#include "myTimers.h"
#include "IOconfig.h"


void initTimer1(unsigned int period) 
{
    //unsigned TimerControlValue;
    
    T1CON = 0;              // ensure Timer 1 is in reset state
    //TimerControlValue=T1CON;
 
    T1CONbits.TCKPS = 0b10; // FCY divide by 64: tick = 2.4us (Tcycle=37.5ns base value)
    T1CONbits.TCS = 0;      // select internal FCY clock source
    T1CONbits.TGATE = 0;    // gated time accumulation disabled
    TMR1 = 0;
    PR1 = period;           // set Timer 1 period register ()
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
    
    comparator = 0;
    myCounter = 1;
    millisCounter = 0;
}

void initTimer1InMS(unsigned int timeInMS)
{
    /*
     * Input is a multiple of 1ms, which means we will have a loss in precision
     * unless the input is a multiple of Tcycle.
     * => going for maximum precision to minimize error => Taking raw Tcycle.
     * => set timer up to 26666 ticks (1.000012 ms) and set up a variable that
     * the interrupt can decrement to the desired time.
     * 
     * If we use prescalers, we lose precision, e.g. with prescaler 64 we are
     * 800ns off for every ms, compared to the 12 ns described above.
     */
    T1CON = 0;              // ensure Timer 1 is in reset state
    
    T1CONbits.TCKPS = 0b00; // Raw FCY value for maximum precision
    T1CONbits.TCS = 0;
    T1CONbits.TGATE = 0;
    
    TMR1 = 0;               // reset timer
    PR1 = 26666;            // set Timer 1 period register ()
    
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
    
    comparator = timeInMS;
    myCounter = 1;
    millisCounter = 0;
}

void startTimer1(void) 
{
    T1CONbits.TON = 1; //
 
}

unsigned long millis(void)
{
    return millisCounter;
}

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
    ++millisCounter;
    
    if(myCounter != comparator)
    {
        myCounter++;
    }
    else
    {
        myCounter = 1;
        LED6=~LED6;
    }
    
}//
