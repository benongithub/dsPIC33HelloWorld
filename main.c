/// Configuration Bits---------------------------

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC                 // Start with Internal RC Oscillator
#pragma config IESO = OFF                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS             // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPOST = PS1            // Watchdog Timer Postscaler (1:1)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = ON             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = OFF                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)


/// Include headers-------------------------------
#include "xc.h"
#include <stdio.h>              // Standard I/O - required for printf() function
#include <stdbool.h>
#include "IOconfig.h"
#include "myTimers.h"
#include "mypwm.h"
#include "myadc.h"
#include "myencoder.h"
#include "math.h"

/// Defines----------------------------
#define SEVEN_MEG_OSC 1//set to 1 if we use slow (7.3728 MHz) oscillator and not 16 MHz
#define M_PI 3.14159265358979323846

int main() {
#if (SEVEN_MEG_OSC == 0) 
    /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 16MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual
    
     * Result: FCY = 0.5 * (16MHz*20/(3*2)) = 26.666 MIPS, Tcycle=37.5nsec
   ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 18; //set PPL to M=20 (18+2)
    CLKDIVbits.PLLPRE = 1; //N1 = input/3
    CLKDIVbits.PLLPOST = 0; //N2 = output/2
#else //Below the 7.3728 Setup 

    /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 7.3728 MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual
    
     * Result: FCY = 0.5 * (7.3728 MHz*29/(2*2)) = 26.73 MIPS, which is 
     * not exactl Tcycle=37.5nsec, but close enough for our purposes
    ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 27; //set PPL to M=29 (27+2)
    CLKDIVbits.PLLPRE = 0; //N1 = input/2
    CLKDIVbits.PLLPOST = 0; //N2 = output/2
#endif //SEVEN_MEG_OSC == 0

    /* Clock switch to incorporate PLL*/
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary

    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(OSCCON || 0x01); // Start clock switching

    while (OSCCONbits.COSC != 0b011);

    // In reality, give some time to the PLL to lock
    while (OSCCONbits.LOCK != 1); //Wait for PPL to lock

    
    setupIO(); //configures inputs and outputs
    
    LED4 = LEDOFF;
    LED5 = LEDOFF;
    LED6 = LEDOFF;
    LED7 = LEDOFF;

    setupPWM();
    initTimer1InMS(1);
    startTimer1();   
    
    initQEI1(0);
    initQEI2(0);
    
    // P1DC1 = 15000L;
    
    float t = 0;
    
    
    unsigned long lastUpdate = millis();
    unsigned long delayUpdate = 1;
    
    while (1) {
        
        // t += 0.001;
        // float sineCurve = (26666L * sin(t*250)) + 26666L;
        // P1DC1 = (long) sineCurve;
        unsigned long current_millis = millis();
        if((unsigned long)(current_millis - lastUpdate)>=delayUpdate) {
            t += 0.001;
            float sineCurve = (10000L * sin(2 * M_PI * 50 * t)) + 10000L;
            P1DC1 = (long) sineCurve;
            //  printf("%f\r\n", sineCurve);
            // LED5 = ~LED5;
            /*t++;
            float sineCurve = (26666L * sin(t*2000)) + 26666L;
            printf("%f\r\n", sineCurve);
            P1DC1 = (long) sineCurve;*/
            lastUpdate = current_millis;
        }
    };
    return 0;
}

/*
    // initTimer1(4166); //creates a 10ms timer interrupt
    // P1DC1 = 26666L;
    // P1DC2 = 26666L;
    // P1DC3 = 26666L; 
    printf("Millis at Start: %lu \r\n", millis());
    unsigned long test = millis();
    
    // printf("Start %d", myCounter);
    printf("Please insert Duty Cycle: \r\n");
    char buffer [32];
    int cnt = 0;
    char ready = 0;
    
    bool switchState = false;
        unsigned long current_millis = millis();
        if((unsigned long)(current_millis - test)>=1000) {
            printf("Current %lu - Last %lu = %lu\r\n", current_millis, test, (unsigned long)(current_millis - test));
            test = current_millis;
        }
        
        if (SW1 && !switchState)
        {
            printf("Button pressed\r\n");
            switchState = true;
        } else if (!SW1 && switchState)
        {
            printf("Button released\r\n");
            switchState = false;
        }
        // printf("%d \r\n", millisCounter);
        
        if(ready == 1) {
            double f = 0;
            sscanf(buffer, "%lf", &f);
            printf("Your duty cyle: %f \r\n", f);
            unsigned long dutyCycle = (2*26666L) * f;
            printf("Your duty cyle: %lu \r\n", dutyCycle);
            P1DC1 = dutyCycle;
            buffer[0] = '\0';
            ready = 0;
        } else {
            char newChar;
            while (_U1RXIF==0);			// Wait and Receive One Character
            newChar = U1RXREG;
            _U1RXIF=0;
            buffer[cnt++] = newChar;
            if((newChar == '\r') || (newChar == '\n') || (cnt == sizeof(buffer)-1)) {
                buffer[cnt] = '\0';
                cnt = 0;
                ready = 1;
            }
        }
		int a;
		
		while (_U1RXIF==0);			// Wait and Receive One Character
		a = U1RXREG;
		
		while(!U1STAbits.TRMT);
		U1TXREG = '"';
	
		while(!U1STAbits.TRMT);		// Echo Back Received Character with quotes
		U1TXREG = a;
	
		while(!U1STAbits.TRMT);
		U1TXREG = '"';
	
		while(!U1STAbits.TRMT);
		U1TXREG = ' '; 	
	
		_U1RXIF=0;					// Clear UART RX Interrupt Flag
    while (1) {
        unsigned long current_millis = millis();
        if((unsigned long)(current_millis - lastPosPrint)>=100) {
            // printf("%ld \r\n", getPositionInCounts_2());
            // printf("%ud\t%ud \r\n", POSCNT, POS2CNT);
            // printf("%u\t%ld \r\n", POSCNT, getPositionInCounts_1());
            printf("%u\t%ld \r\n", POS2CNT, getPositionInCounts_2());
            lastPosPrint = current_millis;
        }
    };
 * 
 * 
 */
