#include "xc.h"
#include "myencoder.h"

long rotationCount1;
volatile long rotationCount2;

void initQEI1(int startPos) {
    QEI1CONbits.QEISIDL = 1;
    QEI1CONbits.QEIM = 0b111;
    QEI1CONbits.SWPAB = 0;
    QEI1CONbits.PCDOUT = 0;
    QEI1CONbits.TQGATE = 0;
    QEI1CONbits.POSRES = 0;
    QEI1CONbits.TQCS = 0;
    QEI1CONbits.UPDN_SRC = 0;
    
    MAXCNT = 0xffff;
    
    POSCNT = startPos;
    rotationCount1=0;
    
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
    if (POSCNT < 32768)
    {
        rotationCount1 = rotationCount1+ (long) 0x10000; // pos roll over
    } else {
        rotationCount1 = rotationCount1- (long) 0x10000; // neg roll over
    }
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

