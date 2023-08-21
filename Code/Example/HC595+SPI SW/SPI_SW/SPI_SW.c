#include "SPI_SW.h"
#include "msp430G2553.h"
#include <stdint.h>


#define SPISW_PORT P1OUT
#define CLK BIT4  //11
#define SDI BIT3 //14
#define STR BIT5 //12

void SPISW_Init(void)
{
    P1SEL  &= ~(CLK + SDI + STR);
    P1SEL2 &= ~(CLK + SDI + STR);
    P1DIR |= STR + SDI + CLK;
    P1OUT &= ~(STR + SDI + CLK);
}
void SPISW_Write(unsigned char byte)
{
    uint8_t i,K;
    K=byte;
        for(i=0;i<8;i++)
            {
                P1Write( SDI, K&0x80);
                P1Write( CLK, 0);
                P1Write( CLK, 1);
                K<<=1;
            } // 1 byte transmission
        P1Write( STR, 0);
        P1Write( STR, 1);
}
void P1Write( unsigned char bit, unsigned char val )
{
  if (val){
      SPISW_PORT |= bit;
  } else {
      SPISW_PORT &= ~bit;
  }
}
