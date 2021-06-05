#include "xc.h"
#include "myencoder.h"
#include "myTimers.h"
#include "IOconfig.h"

long rotationCount1;
long rotationCount2;

unsigned long lastRotationCount1;

void initQEI1(unsigned int startPos) {
    QEI1CONbits.QEISIDL = 1;
    QEI1CONbits.QEIM = 0b101;
    QEI1CONbits.SWPAB = 0;
    QEI1CONbits.PCDOUT = 0;
    QEI1CONbits.TQGATE = 0;
    QEI1CONbits.POSRES = 0;
    QEI1CONbits.TQCS = 0;
    QEI1CONbits.UPDN_SRC = 0;
    
    DFLT1CONbits.QEOUT = 1;
    DFLT1CONbits.QECK = 0b111;
    
    MAXCNT = 0xffff;
    
    POSCNT = startPos;
    rotationCount1=0;
    // lastRotationCount1=millis();
    
    IFS3bits.QEI1IF = 0; //clear
    IPC14bits.QEI1IP = 5; // priority
    IEC3bits.QEI1IE = 1; // enable
}

void initQEI2(unsigned int startPos) {
    QEI2CONbits.QEISIDL = 1;
    QEI2CONbits.QEIM = 0b111;
    QEI2CONbits.SWPAB = 0;
    QEI2CONbits.PCDOUT = 0;
    QEI2CONbits.TQGATE = 0;
    QEI2CONbits.POSRES = 0;
    QEI2CONbits.TQCS = 0;
    QEI2CONbits.UPDN_SRC = 0;
    
    DFLT2CONbits.QEOUT = 1;
    DFLT2CONbits.QECK = 0b111;
    
    MAX2CNT = 0xffff;
    
    POS2CNT = startPos;
    rotationCount2=0;
    
    IFS4bits.QEI2IF = 0; //clear
    IPC18bits.QEI2IP = 5; // priority
    IEC4bits.QEI2IE = 1; // enable
}

void __attribute__((__interrupt__, auto_psv)) _QEI1Interrupt(void)
{
    IFS3bits.QEI1IF = 0; //clear
    // LED6 = ~LED6;
    if (POSCNT < 32768)
        {
            rotationCount1 = rotationCount1 + (long) 0x10000; // pos roll over
            // rotationCount1++;
        } else {
            rotationCount1 = rotationCount1 - (long) 0x10000; // neg roll over
            // rotationCount1--;
     }
    /*unsigned long current_millis = millis();
    if ((current_millis - lastRotationCount1) > 5) {
        LED6 = ~LED6;
        // rotationCount1++;
        if (POSCNT < 32768)
        {
            // rotationCount1 = rotationCount1 + (long) 0x10000; // pos roll over
            rotationCount1++;
        } else {
            // rotationCount1 = rotationCount1 - (long) 0x10000; // neg roll over
            rotationCount1--;
        }
        lastRotationCount1 = current_millis;
    }*/
    
    /*if (POSCNT < 32768)
    {
        // rotationCount1 = rotationCount1 + (long) 0x10000; // pos roll over
        rotationCount1 = rotationCount1 + 65535;
    } else {
        // rotationCount1 = rotationCount1 - (long) 0x10000; // neg roll over
        rotationCount1 = rotationCount1 - 65535;
    }*/
}

void __attribute__((__interrupt__, auto_psv)) _QEI2Interrupt(void)
{
    IFS4bits.QEI2IF = 0; //clear
    if (POS2CNT < 32768)
    {
        rotationCount2 = rotationCount2+ (long) 0x10000; // pos roll over
    } else {
        rotationCount2 = rotationCount2- (long) 0x10000; // neg roll over
    }
}

long getPositionInCounts_1(void)
{
    long currentEncoderPosition = 0;
    currentEncoderPosition = rotationCount1 + POSCNT;
    return currentEncoderPosition;
    // return rotationCount1;
}

long getPositionInCounts_2(void)
{
    long currentEncoderPosition = 0;
    currentEncoderPosition = rotationCount2 + POS2CNT;
    return currentEncoderPosition;
    // return rotationCount2;
}

