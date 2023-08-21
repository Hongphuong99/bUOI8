/*************************************************************************
      DESCRIPTION - STATION
Project su dung Rf24L01 de truyen phat
Author: Trung Thành
Website: http://thanhnt.com
*************************************************************************/

#include <msp430g2553.h>
#include "RF24.c"
#include "delay.h"
#include "stdio.h"

#define RXD        BIT1 //Check your launchpad rev to make sure this is the case. Set jumpers to hardware uart.
#define TXD        BIT2 // TXD with respect to what your sending to the computer. Sent data will appear on this line

void UART_TX(char * tx_data);            // Function Prototype for TX

unsigned char RxBuf[32];
char UART_Mess[32];

void main()
{
  WDTCTL = WDTPW + WDTHOLD;     //Stop WatchDog Timer
  BCSCTL1 = CALBC1_1MHZ;            // Set DCO to 1 MHz
  DCOCTL = CALDCO_1MHZ;
  
  P1DIR |= BIT6;
  P1OUT &= ~BIT6;
  
  IO_RF24L01_initial();
  init_NRF24L01();
  
  //===========UART Initial====================
  P1SEL |= RXD + TXD ;                // Select TX and RX functionality for P1.1 & P1.2
  P1SEL2 |= RXD + TXD ;              //
  
  UCA0CTL1 |= UCSSEL_2;             // Have USCI use System Master Clock: AKA core clk 1MHz
  
  UCA0BR0 = 104;                    // 1MHz 9600, see user manual
  UCA0BR1 = 0;                      //

  UCA0MCTL = UCBRS0;                // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;             // Start USCI state machine
  //===========================================

  __bis_SR_register(GIE);           //Enable interrupt
  nRF24L01_SetRX_Mode();
  while(1)
  {
    //Something code
    
    if(nRF24L01_RxPacket(RxBuf)==1)
    {
      P1OUT |= BIT6;
      delay_ms(10);
      sprintf(UART_Mess, "%s", RxBuf);
      nRF24L01_ClearSend(); // Clear Bit Send
      UART_TX(UART_Mess);
      P1OUT &= ~BIT6;
      nRF24L01_SetRX_Mode();
    } 
  }
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
        while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
        UCA0TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
}