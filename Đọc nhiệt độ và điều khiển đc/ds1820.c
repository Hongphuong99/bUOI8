#define DS1820_DATA BIT3
#define DS1820_P_DIR P2DIR
#define DS1820_P_OUT P2OUT
#define DS1820_P_IN P2IN
#define DS1820_P_REN P2REN

#define DQOUT_HI (DS1820_P_OUT |= DS1820_DATA)
#define DQOUT_LO (DS1820_P_OUT &= ~DS1820_DATA)
#define DQIN  (DS1820_P_IN & DS1820_DATA)

//Define Resolutin for sensor
#define DS18B20_RESOLUTION 		12

//*****************************************
//=CHUONG TRINH DELAY 
void delay_18B20(unsigned int i)
{
    while(i--);
}
//========================
//=KHOI TAO CAM BIEN(RESET)
void Init_DS18B20() 
{
     DS1820_P_DIR |= DS1820_DATA;
     DQOUT_HI;
     delay_18B20(8); 
     DQOUT_LO;
     delay_18B20(80);
     DQOUT_HI;
     delay_18B20(14);
     DS1820_P_DIR &= ~DS1820_DATA;
     delay_18B20(20);
}
//=DOC MOT BYTE  
unsigned char Read_byte(void)
{
    unsigned char i=0;
    unsigned char dat = 0;
    for (i=8;i>0;i--)
     {
         DS1820_P_DIR |= DS1820_DATA;
         DQOUT_LO;
         dat>>=1;
         DQOUT_HI;
         DS1820_P_DIR &= ~DS1820_DATA;
         if(DS1820_P_IN & DS1820_DATA)
          dat|=0x80;
         delay_18B20(4);
     }
     return(dat);
}
//========================
//=CHUONG TRINH GHI 1BYTE 
void Write_byte(unsigned char dat)
{
   DS1820_P_DIR |= DS1820_DATA;
 unsigned char i=0;
 for (i=8; i>0; i--)
 {
   DQOUT_LO;
   if(dat&0x01) DQOUT_HI;
   else DQOUT_LO;
   delay_18B20(5);
   DQOUT_HI;
   dat>>=1;
 }
}

//========================
//=HIEU CHINH DO PHAN GIAI CUA CAM BIEN
void SetResolution(unsigned int resolution)
{
	unsigned char resolution_bin;
	switch(resolution)
	{
	case 9: resolution_bin = 0x1F; break; //09bit = xx ~ R1R0 ~ 00
	case 10: resolution_bin = 0x3F; break; //10bit = xx ~ R1R0 ~ 01
	case 11: resolution_bin = 0x5F; break; //11bit = xx ~ R1R0 ~ 10
	case 12: resolution_bin = 0x7F; break; //12bit = xx ~ R1R0 ~ 11
	default: resolution_bin = 0x7F; //==> default 12bits
	}
	Init_DS18B20();
	Write_byte(0xCC);                // Skip ROM
	Write_byte(0x4E);    			 // WRITE_SCRATCHPAD
	Write_byte(0x7f);                // Alarm TH (có thể thay bằng 0xFF nếu bị lỗi)
	Write_byte(0x80);                // Alarm TL (có thể thay bằng 0xFF nếu bị lỗi)
	Write_byte(resolution_bin);      // 0xx11111 xx=resolution (9-12 bits)
}

//========================
//=CHUONG TRINH DOC NHIET DO TU CAM BIEN
float ReadTemperature(void)
{
	float t;
	char a, b;
	SetResolution(DS18B20_RESOLUTION);
    Init_DS18B20();
    Write_byte(0xCC);
    Write_byte(0x44);
    delay_18B20(100);
    Init_DS18B20();
    Write_byte(0xCC);
    Write_byte(0xBE);
    a=Read_byte();
    b=Read_byte();
    t = (b*256+a)*0.0625; //Đoạn này không cần chỉnh sửa, chỉ cần set Resolution rồi là xong, kết quả sẽ như ý
    return t;
}