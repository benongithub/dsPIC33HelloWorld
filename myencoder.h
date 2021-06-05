#ifndef XC_MYENCODER_H
#define	XC_MYENCODER_H

#include <xc.h>

void initQEI1(int startPos);
long getPositionInCounts_1(void);

void initQEI2(unsigned int startPos);
long getPositionInCounts_2(void);

#endif	/* XC_MYENCODER_H */

