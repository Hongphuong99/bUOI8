// Cau truc dieu khien tung bit cua msp430f2131
union reg 
{
	struct bit
	{
		unsigned char b0:1;
		unsigned char b1:1;
		unsigned char b2:1;
		unsigned char b3:1;
		unsigned char b4:1;
		unsigned char b5:1;
		unsigned char b6:1;
		unsigned char b7:1;
	}_BIT;
	unsigned char _BYTE;
};
union reg* _P1_DIRECT=(union reg*)0x22; // truy xuat port 1
union reg* _P1_OUT=(union reg*)0x21;    // ngo ra
union reg* _P1_IN=(union reg*)0x20;		// ngo vao
union reg* _P1_SEL=(union reg*)0x26;
union reg* _P2_DIRECT=(union reg*)0x2A;
union reg* _P2_OUT=(union reg*)0x29;
union reg* _P2_IN=(union reg*)0x28;
union reg* _P2_SEL=(union reg*)0x2e;
// dinh nghia chan LCD========================================
#define RS _P1_OUT->_BIT.b5
#define EN _P1_OUT->_BIT.b4

#define D4 _P1_OUT->_BIT.b3
#define D5 _P1_OUT->_BIT.b2
#define D6 _P1_OUT->_BIT.b1
#define D7 _P1_OUT->_BIT.b0

#define line_1 0x80     
#define line_2 0xc0
#define clear  0x01
typedef union _BYTE_VAL
{
    unsigned char Val;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
    } bits;
} BYTE_VAL;


/************************************************************************/
/*============ KHOI TAO LCD===============================================*/
void lcd_init();
/*============= TAO XUNG==================================================*/
void enable();
/*================= HAM GUI 4 BIT THAP======================================*/
void set_lsb(unsigned char data);
/*==================HAM GUI 4 BIT CAO======================================*/
void set_msb(unsigned char data);
/*================= GUI LENH LEN LCD======================================*/
void put_cmd(unsigned char cmd);
/*================== GUI 1 KI TU ====================================*/
void lcd_putc(unsigned char data);
/*================== GUI 1 XAU KI TU==================================*/
void lcd_puts(unsigned char *s);
/***********************************************************************/
void delay(unsigned long int t);
void lcd_gotoxy(unsigned char col,unsigned char row);
void lcd_put_byte(unsigned char rs,unsigned char b);