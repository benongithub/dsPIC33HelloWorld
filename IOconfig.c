#include "xc.h"
#include "IOconfig.h"

#define FCY             26730000                //Instruction Cycle Frequency
#define BAUDRATE        9600		      
#define BRGVAL          ((FCY/BAUDRATE)/16)-1 

void setupIO()
{

    int i;
    AD1PCFGL=0xFFFF; //all pins are now digital, by default they are analogue
    
    // set LEDs as output
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
    
    // UART1 TX
    TRISBbits.TRISB8=0;

    //PIN MAPPING
    //before we map, we need to unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    // PERIPHERAL receives data from which INPUT  
    // RPINR18bits.U1RXR = 9; //mapped to RP9 is U1 RX, CHANGE THIS
    //PERIPHERAL QEA Encoder 1, receives data from RP10
    RPINR14bits.QEA1R = 10; 
    //PERIPHERAL QEB Encoder 1, receives data from RP11
    RPINR14bits.QEB1R = 11;
    //PERIPHERAL QEA Encoder 2, receives data from RP7
    RPINR16bits.QEA2R = 6; 
    //PERIPHERAL QEB Encoder 2, receives data from RP8
    RPINR16bits.QEB2R = 7;
    //OUTPUT PIN receives data from which PERIPHERAL, 
    //see table 11-2 in datasheet to check peripheral codes 
    // RPOR4bits.RP8R = 0b00011; //output bin RP2 gets data from peripheral U1 TX 
    
    RPINR18 = 9;			// Make Pin RP9 U1RX
	RPOR4bits.RP8R = 3;		// Make Pin RP8 U1TX
    U1BRG  = BRGVAL;
	U1MODE = 0x8000; /* Reset UART to 8-n-1, alt pins, and enable */
	U1STA  = 0x0440; /* Reset status register and enable TX & RX*/
    _U1RXIF=0; // Clear UART RX Interrupt Flag
   
    //after mapping we lock again
     __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)
     
    for (i = 0; i < 30000; i++); // short dirty delay for changes to take effect,
}

void UART_PutChar(char c)
{
  while (!U1STAbits.TRMT);        // Wait for space in UART1 Tx buffer
  U1TXREG = c;                    // Write character to UART1
}

int write(int handle, void *buffer, unsigned int len)
{
  int i;
  switch (handle)
  {
      case 0:        // handle 0 corresponds to stdout
      case 1:        // handle 1 corresponds to stdin
      case 2:        // handle 2 corresponds to stderr
      default:
          for (i=0; i<len; i++)
              UART_PutChar(*(char*)buffer++);
  }
  return(len);
}