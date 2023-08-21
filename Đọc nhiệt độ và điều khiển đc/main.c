/**********************************************
* Project: Mach cam bien nhiet do, dieu khien dong
*     co, hien thi nhiet do len LCD 16x2.
*     Support: chon dai thong so he thong bang tay
* Khai bao PORT Su dung:
* __PWM__
*   1 channel
*   PORT: P1.6
* __LCD-DISPLAY__
*   4 wire DATA, 2 wire CONTROLLER
*   PORT: P1.0(D7), P1.1(D6), P1.2(D5), P1.3(D4), P1.4(EN), P1.5(RS)
* __Sensor-DS18B20__
*   1 wire DATA
*   PORT: P2.3
* __BUTTON__
*   3 button
*   PORT: P2.0 (Option), P2.1(INCREASE), P2.2(DECREASE)
* __LED-Indicator__
*   2 PORT: P2.4(LV1), P2.5(LV2)
************************************************/
#include "msp430g2553.h"
#include "stdio.h"
#include "string.h"
#include "lcd.c"
#include "ds1820.c"

#define PWM_OUT BIT6 //P1.6 PWM
#define BUTTON_OPTION 	BIT0 //P2.0
#define BUTTON_INCREASE 	BIT1 //P2.1
#define BUTTON_DECREASE 	BIT2 //P2.2
#define BUTTON_P_DIR 	P2DIR
#define BUTTON_P_OUT 	P2OUT
#define BUTTON_P_REN 	P2REN
#define BUTTON_P_IES 	P2IES
#define BUTTON_P_IFG 	P2IFG
#define BUTTON_P_IE 	P2IE

float nhietdo=0;
int tempmin = 25, tempmax = 30, temp_dangerus = 70, motor_min = 4, motor_max = 9;
volatile int a, b, ta=0, func=0, enable_sys = 1;
char ketqua[];

//*****function special*****
void button_init()
{
  //setup for button port 
  BUTTON_P_DIR &= ~(BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
  BUTTON_P_OUT |= (BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
  BUTTON_P_REN |= (BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
  BUTTON_P_IES |= (BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
  BUTTON_P_IFG &= ~(BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
  BUTTON_P_IE |= (BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
}
 
void display_temp_min()
{
  lcd_gotoxy(0,0); lcd_puts("Set T LV1:      ");
  lcd_gotoxy(0,1); sprintf(ketqua," T= %5d oC    ",tempmin); lcd_puts(ketqua);
  __delay_cycles(200000);
}

void display_temp_max()
{
  lcd_gotoxy(0,0); lcd_puts("Set T LV2:      ");
  lcd_gotoxy(0,1); sprintf(ketqua," T= %5d oC    ",tempmax); lcd_puts(ketqua);
  __delay_cycles(200000);
} 

void display_temp_dangerus()
{
  lcd_gotoxy(0,0); lcd_puts("Set T Dangerus: ");
  lcd_gotoxy(0,1); sprintf(ketqua," T= %5d oC    ",temp_dangerus); lcd_puts(ketqua);
  __delay_cycles(200000);
} 

void display_motor_min()
{
  lcd_gotoxy(0,0); lcd_puts("Set Motor LV1:  ");
  lcd_gotoxy(0,1); sprintf(ketqua," P= %5d %%    ",motor_min*10); lcd_puts(ketqua);
  __delay_cycles(200000);
}

void display_motor_max()
{
  lcd_gotoxy(0,0); lcd_puts("Set Motor LV2:  ");
  lcd_gotoxy(0,1); sprintf(ketqua," P= %5d %%    ",motor_max*10); lcd_puts(ketqua);
  __delay_cycles(200000);
}
void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  BCSCTL1=CALBC1_1MHZ;
  DCOCTL=CALDCO_1MHZ;
  
  //setup for lcd port
  P1DIR |= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5; 
  P1SEL=0;  
  
  //setup for led motor port
  P2DIR |= (BIT4 + BIT5);
  P2OUT &= ~(BIT4 + BIT5);
  
  //setup port PWM
  P1DIR|= PWM_OUT;
  P1OUT&=~PWM_OUT;
  
  //setup for timer0 interrupt
  TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TA0CCR0 = 5000;                            //5ms with DCO = 1MHz
  TA0CCR1 = 0; 
  TA0CCTL1 = OUTMOD_7;
  P1SEL |= PWM_OUT;
  TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode

  button_init();
  lcd_init();
  __bis_SR_register(GIE); //=======Enable interrupt
  
  __delay_cycles(10);
  lcd_gotoxy(0,0);
  lcd_puts("  THANHNT.COM   ");
  lcd_gotoxy(0,1);
  lcd_puts("  DO NHIET DO   ");
  __delay_cycles(2000000);
  lcd_gotoxy(0,0);
  lcd_puts(" VI DIEU KHIEN  ");
  lcd_gotoxy(0,1);
  lcd_puts(" TI MSP430G2553 ");
  __delay_cycles(2000000);
  lcd_clear();
  __delay_cycles(2000);
  while(1)
  {
	  switch(func){
		case 0:
				if(nhietdo <= tempmin) {
				  TA0CCR1 = 0; // 0% PWM
				  enable_sys = 1;
				  lcd_gotoxy(0,0);
				  lcd_puts("Motor:OFF       ");}
				if(nhietdo > tempmin && enable_sys == 1) {
				  TA0CCR1 = motor_min*500; // min PWM
				  P2OUT |= BIT4;
				  lcd_gotoxy(0,0);
				  lcd_puts("Motor:ON-Level:1");}
				if(nhietdo >= tempmax && enable_sys == 1) {
				  TA0CCR1 = motor_max*500; // max PWM
				  P2OUT |= BIT5;
				  lcd_gotoxy(0,0);
				  lcd_puts("Motor:ON-Level:2");}
				if(nhietdo > temp_dangerus) { // temp so high, turn off motor
                  TA0CCR1 = 0;
				  enable_sys = 0;
                  P2OUT &= ~(BIT4+BIT5);
                  lcd_gotoxy(0,0);
                  lcd_puts(" Turn off Motor ");}
                if(!enable_sys) {lcd_gotoxy(0,0); lcd_puts(" Turn off Motor ");}
				
			lcd_gotoxy(0,1);
			sprintf(ketqua," T= %5.2f oC    ",nhietdo);
			lcd_puts(ketqua);
		    P2OUT &= ~(BIT4+BIT5);
			break;
		case 1:	//option tuy chinh moc nhiet do min
				display_temp_min(); break;
		case 2: //option tuy chinh moc nhiet do max
				display_temp_max(); break;
        case 3: //option tuy chinh nhiet do nguy hiem
                display_temp_dangerus(); break;
		case 4: //option tuy chinh moc dong co min
				display_motor_min(); break;
		case 5: //option tuy chinh moc dong co max
				display_motor_max(); break;
		default: break;
	  }
  }
}

//ISR timer interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
  ta++;
  if(ta==200) {nhietdo = ReadTemperature();ta=0;}
}

//ISR external interrupt over button
#pragma vector=PORT2_VECTOR
__interrupt void PressButton (void)
{
  if((BUTTON_P_IFG & BUTTON_OPTION)==BUTTON_OPTION) //Start option
  {
    func++;
    if(func>5) func=0;
    __delay_cycles(500000); //Debouncing - chong doi phim
    BUTTON_P_IFG &= ~BUTTON_OPTION;
  }
  if((BUTTON_P_IFG & BUTTON_INCREASE)==BUTTON_INCREASE) //increase value
  {
    if(func==1) {tempmin++; if(tempmin>=tempmax) tempmin = tempmax;}
    if(func==2) {tempmax++; if(tempmax>=temp_dangerus) tempmax = temp_dangerus;}
    if(func==3) temp_dangerus++;
    if(func==4) {motor_min++; if(motor_min>=motor_max) motor_min = motor_max;} // gioi han muc min ko dc vuot qua muc max
    if(func==5) {motor_max++; if(motor_max>10) motor_max=10;} //gioi han ko vuot qua 100%
    __delay_cycles(500000); //Debouncing - chong doi phim
    BUTTON_P_IFG &= ~BUTTON_INCREASE;
  }
  if((BUTTON_P_IFG & BUTTON_DECREASE)==BUTTON_DECREASE) //decrease value
  {
    if(func==1) tempmin--;
    if(func==2) {tempmax--; if(tempmax<=tempmin) tempmax = tempmin;}
    if(func==3) {temp_dangerus--; if(temp_dangerus<=tempmax) temp_dangerus = tempmax;}
    if(func==4) {motor_min--; if(motor_min<0) motor_min=0;} //gioi han ko dc thap hon 0%
    if(func==5) {motor_max--; if(motor_max<=motor_min) motor_max = motor_min;} //gioi han muc max ko dc thap hon muc min
    __delay_cycles(500000); //Debouncing - chong doi phim
    BUTTON_P_IFG &= ~BUTTON_DECREASE;
  }
  BUTTON_P_IFG &= ~(BUTTON_OPTION|BUTTON_INCREASE|BUTTON_DECREASE);
}