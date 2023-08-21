/*************************************************************************
      DESCRIPTION - CLIENT
Project su dung Rf24L01 de truyen phat
Author: Trung Thành
Website: http://thanhnt.com
*************************************************************************/

#include <msp430g2553.h>
#include "RF24.c"
#include "ds1820.c"
#include "delay.h"
#include "stdio.h"

float nhietdo=0, voltage=0;
volatile int ta0 = 0, ta1 = 0;
long x[8];
long volt_measure = 0;

unsigned short AdcConvert(void);

void main()
{
  WDTCTL = WDTPW + WDTHOLD;     //Stop WatchDog Timer
  BCSCTL1 = CALBC1_1MHZ;            // Set DCO to 1 MHz
  DCOCTL = CALDCO_1MHZ;
  
  //Led Notify
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  IO_RF24L01_initial();
  init_NRF24L01();
  
  //=============Timer Initial=================
  //setup for timer0 interrupt
  TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
  TA0CCR0 = 5000; 
  //TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
  
  TA0CCTL1 = CCIE;
  TA0CCR1 = 5000;
  TA0CTL = TASSEL_2 + MC_1;
  //===========================================
  
  //=============ADC Initial===================
  ADC10CTL1 = INCH_7 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + REF2_5V;
  //__delay_cycles(1000);
  //ADC10CTL0 |= ADC10ON + ENC;
  ADC10AE0 |= BIT7;
  //===========================================
  __bis_SR_register(GIE);           //Enable interrupt
  nRF24L01_SetRX_Mode();
  while(1)
  {

  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
  ta0++;
  char MessSend[32];
  if(ta0==200) {
    //Get temperature
    nhietdo = DS18B20_ReadTemperature();
    nRF24L01_SetTX_Mode();
    sprintf(MessSend,"%5.2fC-%5.2fV \r\n",nhietdo, voltage);
    unsigned char * TxBuf = (unsigned char *)MessSend;
    //Send
    P1OUT |= BIT0;
    nRF24L01_TxPacket(TxBuf); // Transmit Tx buffer data
    nRF24L01_ClearSend(); // Clear Bit Send
    nRF24L01_SetRX_Mode();
    P1OUT &= ~BIT0;
    
    ta0=0;
  }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1(void)
{
  ta1++;
  if(ta1==200) {
  //Get voltage 8 times
  for(int i =0; i<8; i++)
      {
        x[i] = AdcConvert();
        volt_measure += x[i];
      }
      //arverage voltage value
      volt_measure>>=3;
      voltage = (volt_measure*30.0)/1023; //Max input ADC: 30V
      ta1=0;
  }
}

 unsigned short AdcConvert(void)
 {
 	// Bat dau chuyen doi
 	ADC10CTL0 |= ADC10SC + ENC;
 	// Cho chuyen doi xong (cho bit ADC10BUSY xuong 0)
 	while (ADC10CTL1 & 1);
 	// Doc va tra ve ket qua chuyen doi
 	return ADC10MEM;
 }