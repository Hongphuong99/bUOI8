#define DS1820_DATA BIT1
#define DS1820_P_DIR P1DIR
#define DS1820_P_OUT P1OUT
#define DS1820_P_IN P1IN
#define DS1820_P_REN P1REN

//Define Port State
#define DS18B20_DQOUT_HI (DS1820_P_OUT |= DS1820_DATA)
#define DS18B20_DQOUT_LO (DS1820_P_OUT &= ~DS1820_DATA)
#define DS18B20_DQIN  (DS1820_P_IN & DS1820_DATA)

//Define COMMAND
#define DS18B20_CONVERT_T 			0x44
#define DS18B20_READ_SCRATCHPAD		0xBE
#define DS18B20_WRITE_SCRATCHPAD	0x4E
#define DS18B20_COPY_SCRATCHPAD		0x48
#define DS18B20_SKIP_ROM 			0xCC
#define DS18B20_READ_ROM			0x33		
#define DS18B20_MATCH_ROM			0x55
#define DS18B20_SEARCH_ROM			0xF0
#define DS18B20_ALARM_SEARCH		0xEC

//Define Resolutin for sensor
#define DS18B20_RESOLUTION 		12

//*****************************************
//=CHUONG TRINH DELAY 
void DS18B20_Delay(unsigned int i)
{
    while(i--);
}
//========================
//=KHOI TAO CAM BIEN(RESET)
void DS18B20_Initial() 
{
     DS1820_P_DIR |= DS1820_DATA;
     DS18B20_DQOUT_HI;
     DS18B20_Delay(8); 
     DS18B20_DQOUT_LO;
     DS18B20_Delay(80);
     DS18B20_DQOUT_HI;
     DS18B20_Delay(14);
     DS1820_P_DIR &= ~DS1820_DATA;
     DS18B20_Delay(20);
}
//=DOC MOT BYTE  
unsigned char DS18B20_Read_byte(void)
{
    unsigned char i=0;
    unsigned char dat = 0;
    for (i=8;i>0;i--)
     {
         DS1820_P_DIR |= DS1820_DATA;
         DS18B20_DQOUT_LO;
         dat>>=1;
         DS18B20_DQOUT_HI;
         DS1820_P_DIR &= ~DS1820_DATA;
         if(DS1820_P_IN & DS1820_DATA)
          dat|=0x80;
         DS18B20_Delay(4);
     }
     return(dat);
}
//========================
//=CHUONG TRINH GHI 1BYTE 
void DS18B20_Write_byte(unsigned char dat)
{
   DS1820_P_DIR |= DS1820_DATA;
 unsigned char i=0;
 for (i=8; i>0; i--)
 {
   DS18B20_DQOUT_LO;
   if(dat&0x01) DS18B20_DQOUT_HI;
   else DS18B20_DQOUT_LO;
   DS18B20_Delay(5);
   DS18B20_DQOUT_HI;
   dat>>=1;
 }
}

//========================
//=HIEU CHINH DO PHAN GIAI CUA CAM BIEN
void DS18B20_SetResolution(unsigned int resolution)
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
	DS18B20_Initial();
	DS18B20_Write_byte(DS18B20_SKIP_ROM);            // Skip ROM
	DS18B20_Write_byte(DS18B20_WRITE_SCRATCHPAD);    // WRITE_SCRATCHPAD
	DS18B20_Write_byte(0x7f);                // Alarm TH (có thể thay bằng 0xFF nếu bị lỗi)
	DS18B20_Write_byte(0x80);                // Alarm TL (có thể thay bằng 0xFF nếu bị lỗi)
	DS18B20_Write_byte(resolution_bin);      // 0xx11111 xx=resolution (9-12 bits)
}

//========================
//=CHUONG TRINH DOC NHIET DO TU CAM BIEN
float DS18B20_ReadTemperature(void)
{
	float t;
	char a, b;
	DS18B20_SetResolution(DS18B20_RESOLUTION);
    DS18B20_Initial();
    DS18B20_Write_byte(DS18B20_SKIP_ROM);
    DS18B20_Write_byte(DS18B20_CONVERT_T);
    DS18B20_Delay(100);
    DS18B20_Initial();
    DS18B20_Write_byte(DS18B20_SKIP_ROM);
    DS18B20_Write_byte(DS18B20_READ_SCRATCHPAD);
    a=DS18B20_Read_byte();
    b=DS18B20_Read_byte();
    t = (b*256+a)*0.0625;
    return t;
}