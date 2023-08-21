#include "msp430.h"
#include "Basic_Config/Basic_Config.h"
#include "I2C/I2C.h"
#include <stdint.h>
#include <stdio.h>
#define DS1307ADD 0x68
 uint8_t Write_DS1307[]={0x00,0x00,0x12,0x01,0x02,0x12,0x18}; //array to config data for Ds1307
//                       sec, min, hour, day, date, mon, year
 uint8_t Read_DS1307[8];

 uint8_t sec, min, hour, day , date , month, year;
void Data_Transfer(void);
void main (void)
{
    Config_stop_WDT();
    Config_Clocks();


    i2c_init(DS1307ADD);
    i2c_wirte(0x00, 7, Write_DS1307);

    while(1)
    {
        i2c_read(0x00, 7, Read_DS1307);
        Data_Transfer();
        delay_ms(500);
    }
}

void Data_Transfer(void)
{
    sec    =((Read_DS1307[0]&0xf0)>>4)*10 + (Read_DS1307[0]&0x0f);
    min    =((Read_DS1307[1]&0xf0)>>4)*10 + (Read_DS1307[1]&0x0f);
    hour   =((Read_DS1307[2]&0xf0)>>4)*10 + (Read_DS1307[2]&0x0f);
    day    =((Read_DS1307[3]&0xf0)>>4)*10 + (Read_DS1307[3]&0x0f);
    date   =((Read_DS1307[4]&0xf0)>>4)*10 + (Read_DS1307[4]&0x0f);
    month  =((Read_DS1307[5]&0xf0)>>4)*10 + (Read_DS1307[5]&0x0f);
    year   =((Read_DS1307[6]&0xf0)>>4)*10 + (Read_DS1307[6]&0x0f);
}


