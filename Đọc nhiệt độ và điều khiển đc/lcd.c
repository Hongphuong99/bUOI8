void delay(unsigned long t)
{
  unsigned long i=0;
  for(i=0;i<t;i++);
}
void enable()
{
  EN=1;
  delay(10);
  EN=0;
  delay(20);
}
//ham gui 4 bit thap
void set_lsb(unsigned char data)
{
  unsigned char nibble=0;
  nibble=data&0x01;
  if(nibble==0x01) D4=1;
  else D4=0;
  nibble=data&0x02;
  if(nibble==0x02) D5=1;
  else D5=0;
  nibble=data&0x04;
  if(nibble==0x04) D6=1;
  else D6=0;
  nibble=data&0x08;
  if(nibble==0x08) D7=1;
  else D7=0;
}
//ham gui 4 bit cao
void set_msb(unsigned char data)
{
  unsigned char nibble=0;
  nibble=data&0x10;
  if(nibble==0x10) D4=1;
  else D4=0;
  nibble=data&0x20;
  if(nibble==0x20) D5=1;
  else D5=0;
  nibble=data&0x40;
  if(nibble==0x40) D6=1;
  else D6=0;
  nibble=data&0x80;
  if(nibble==0x80) D7=1;
  else D7=0;
}
// ham gui lenh len lcd
void put_cmd(unsigned char cmd)
{
  RS=0; // chon thanh ghi lenh
  set_msb(cmd);
  enable();
  set_lsb(cmd);
  enable();
}
//ham gui 1 ki tu len lcd
void lcd_putc(unsigned char data)
{
  RS=1; // chon thanh ghi du lieu
  set_msb(data);
  enable();
  set_lsb(data);
  enable();
}
//ham gui xau ki tu len lcd
void lcd_puts(unsigned char *s)
{
  while(*s)
  {
    lcd_putc(*s);
    s++;
  };
}
// khoi tao lcd
void lcd_init()
{
  delay(100);
  RS=0; // gui lenh len lcd
  set_lsb(0x03);
  enable();
  enable();
  enable();
  set_lsb(0x02);
  enable();
  put_cmd(0x28); // che do 4 bit,lcd 2 dong, co chu 5x7
  put_cmd(0x0c); // bat hien thi,tat con tro
  put_cmd(0x06);
  }
// xoa lcd
void lcd_clear()
{
  put_cmd(0x01);
}
void lcd_put_byte(unsigned char rs,unsigned char b)
{
	BYTE_VAL temp;
	RS=0;
	if(rs) RS=1;
	delay(20);
	EN=0;
	temp.Val=b;
	// send the high nibble
	D4 = temp.bits.b4;
	D5 = temp.bits.b5;
	D6 = temp.bits.b6;
	D7 = temp.bits.b7;
	delay(20);
	EN =  1;
	delay(20);
	EN = 0;
	// send the low nibble
	D4 = temp.bits.b0;
	D5 = temp.bits.b1;
	D6 = temp.bits.b2;
	D7 = temp.bits.b3;
	delay(20);
	EN =  1;
	delay(20);
	EN = 0;
}
void lcd_gotoxy(unsigned char col, unsigned char row)
{
	unsigned char address;
	
	if(row!=0)
		address=0x40;
	else
		address=0;
	
	address += col;
	lcd_put_byte(0,0x80|address);
}