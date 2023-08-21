#include "msp430.h"
#include <stdint.h>
#include <stdio.h>
#include "Basic_Config/Basic_Config.h"
#include  "SPI_SW/SPI_SW.h"
// define 7segs
#define SEG1_BIT BIT5
#define SEG2_BIT BIT6
#define SEG3_BIT BIT7
#define SEG4_BIT BIT0
#define SEG1_PORT P2OUT
#define SEG2_PORT P2OUT
#define SEG3_PORT P2OUT
#define SEG4_PORT P1OUT
void LED7Seg_config(void);
uint8_t  LED7Seg_code[]= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
void main (void)
{
    Config_stop_WDT();
    Config_Clocks();
    LED7Seg_config();
    SPISW_Init();
    while(1)
    {
        SEG1_PORT &= ~SEG1_BIT;
        SPISW_Write(LED7Seg_code[1]);
        delay_ms(100);
    }
}
void LED7Seg_config(void)
{
        P1DIR |= (SEG4_BIT);
        P2DIR |= (SEG1_BIT+SEG2_BIT+SEG3_BIT);
        P1DIR |=SEG4_BIT;
        P2DIR |=(SEG1_BIT+SEG2_BIT+SEG3_BIT);
        P2SEL &= ~(BIT6+BIT7);
        P2OUT |=(SEG1_BIT+SEG2_BIT+SEG3_BIT);
        P1OUT |=SEG4_BIT;
}

