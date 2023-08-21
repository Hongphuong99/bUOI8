#include "delay.h"
//****************************************************************************************
//*DEFINE VALUE nRF24L01 COMMAND
//***************************************************************************************/
//*********************************************NRF24L01***********************************
#define TX_ADR_WIDTH    5     // 5 uints TX address width
#define RX_ADR_WIDTH    5     // 5 uints RX address width
#define TX_PLOAD_WIDTH  32    // 32 uints TX payload
#define RX_PLOAD_WIDTH  32    // 32 uints TX payload

//***************************************NRF24L01*****************************************
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define WR_ACK_PAYLOAD	0xA8
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register
//*************************************SPI(nRF24L01)**************************************
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

//**********************************nRF24L01 interrupt flag's*****************************
#define IDLE            0x00  // Idle, no interrupt pending
#define MAX_RT          0x10  // Max #of TX retrans interrupt
#define TX_DS           0x20  // TX data sent interrupt
#define RX_DR           0x40  // RX data received

//****************************************************************************************
extern unsigned char TxBuf[TX_PLOAD_WIDTH];
extern unsigned char RxBuf[RX_PLOAD_WIDTH];

unsigned char RxBuf[32];
unsigned char TxBuf[32]=
{
	0x01,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
	0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
	0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,
	0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,
};
void IO_RF24L01_initial(void);
void init_NRF24L01(void);
unsigned char nRF24L01_SPI_RW(unsigned char Buff);
unsigned char nRF24L01_SPI_Read(unsigned char reg);
unsigned char nRF24L01_SPI_RW_Reg(unsigned char reg, unsigned char value);
unsigned char nRF24L01_SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars);
unsigned char nRF24L01_SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned uchars);
void nRF24L01_SetRX_Mode(void);
void nRF24L01_SetTX_Mode(void);
void nRF24L01_SetChannel(unsigned int channel);
void nRF24L01_ClearSend(void);
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char* tx_buf);
//****************************************************************************************
//*Config ADDRESS RX-TX for RF Device
//***************************************************************************************/
unsigned char TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//
unsigned char RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//

//****************************************************************************************
//*Config PORT
//***************************************************************************************/

#define BIT(x)	(1 << (x))
#define CE        0
#define CSN       3
#define SCK       1
#define MOSI      4
#define MISO      2
#define IRQ       5
#define PORT_RF_OUT     P2OUT
#define PORT_RF_DIR     P2DIR
#define PORT_RF_IN      P2IN
#define PORT_RF_REN     P2REN

//****************************************************************************************
//*Config COMMAND MSP430
//***************************************************************************************/
#define set |= //set state to 1
#define tgl ^= //toggle state 1->0; 0->1
#define clr &=~ //clear state to 0

//****************************************************************************************
//*NRF24L01
//***************************************************************************************/
void IO_RF24L01_initial(void)
{
	PORT_RF_DIR set (BIT(CSN) + BIT(CE) + BIT(SCK) + BIT(MOSI));
	PORT_RF_DIR clr BIT(MISO);

	PORT_RF_REN set (BIT(IRQ) + BIT(MISO));
	PORT_RF_OUT set BIT(IRQ);
          
	PORT_RF_OUT clr BIT(CE); //CHIP DISABLE
	PORT_RF_OUT set BIT(CSN); //CSN IS PULL HIGH.DISABLE THE OPERATION
	PORT_RF_OUT clr BIT(SCK);//CLK IS LOW

}
void init_NRF24L01(void)
{
	delay_us(100);
	PORT_RF_OUT clr BIT(CE);    // chip enable
	PORT_RF_OUT set BIT(CSN);   // Spi disable
	PORT_RF_OUT clr BIT(SCK);   // Spi clock line init high
	nRF24L01_SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Turn on auto acknowledge
	nRF24L01_SPI_RW_Reg(WRITE_REG + SETUP_AW, 0x03);      // Address widht 5 bytes
	nRF24L01_SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //Enable data P0
	nRF24L01_SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a);  //500us + 86us, 10 retrans...
	nRF24L01_SPI_RW_Reg(WRITE_REG + RF_CH, 0);        // Chanel 0 ; RF = 2400 + RF_CH* (1or 2 M)
	nRF24L01_SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x26);   		// 1M, 0dbm (0x26 => 250kpbs, 0x07 => 1Mbps, 0x0F => 2Mbps)
	nRF24L01_SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); // Do rong data truyen 32 byte
	nRF24L01_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    //
	nRF24L01_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //
	nRF24L01_SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // Enable CRC, 2 byte CRC, Send
	delay_ms(16);
}
//****************************************************************************************************/
//unsigned char SPI_RW(unsigned char Buff) SPI Write
//NRF24L01
//****************************************************************************************************/
unsigned char nRF24L01_SPI_RW(unsigned char Buff)
{
	unsigned char bit_ctr;
	PORT_RF_OUT clr BIT(SCK);
	delay_us(1);
	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
	{
		if (Buff & 0x80)
		PORT_RF_OUT set BIT(MOSI);
		else PORT_RF_OUT clr BIT(MOSI);			// output 'byte', MSB to MOSI
		
		PORT_RF_OUT set BIT(SCK);       // Set SCK high..   
		delay_us(2);
		Buff = (Buff << 1);           // shift next bit into MSB..
		//delay_us(5);		  
        if (PORT_RF_IN & (BIT(MISO))) // capture current MISO bit
			Buff |= 0x01;
		else
		{
			Buff &= ~0x01;
		}
		PORT_RF_OUT clr BIT(SCK);            		  // ..then set SCK low again
		delay_us(2);
   	}
    //PORT_RF_OUT clr BIT(MOSI);
    return(Buff);           		  // return read uchar
}
//****************************************************************************************************/
//unsigned char SPI_Read(unsigned char reg READ REG
//NRF24L01
//****************************************************************************************************/
unsigned char nRF24L01_SPI_Read(unsigned char reg)
{
	unsigned char reg_val;

	PORT_RF_OUT clr BIT(CSN);                // CSN low, initialize SPI communication...
	nRF24L01_SPI_RW(reg);            // Select register to read from..
	reg_val = nRF24L01_SPI_RW(0);    // ..then read registervalue
	PORT_RF_OUT set BIT(CSN);                // CSN high, terminate SPI communication

	return(reg_val);        // return register value
}
//****************************************************************************************************/
//unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
//****************************************************************************************************/
unsigned char nRF24L01_SPI_RW_Reg(unsigned char reg, unsigned char value)
{
	unsigned char status;

	PORT_RF_OUT clr BIT(CSN);                   // CSN low, init SPI transaction
	status = nRF24L01_SPI_RW(reg);      // select register
	nRF24L01_SPI_RW(value);             // ..and write value to it..
	PORT_RF_OUT set BIT(CSN);                   // CSN high again

	return(status);            // return nRF24L01 status uchar
}
//****************************************************************************************************/
//unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
//
//****************************************************************************************************/
unsigned char nRF24L01_SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
{
	unsigned char status,uchar_ctr;

	PORT_RF_OUT clr BIT(CSN);                   		// Set CSN low, init SPI tranaction
	status = nRF24L01_SPI_RW(reg);       		// Select register to write to and read status uchar

	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = nRF24L01_SPI_RW(0);    //

	PORT_RF_OUT set BIT(CSN);

	return(status);                    // return nRF24L01 status uchar
}
//****************************************************************************************************/
//uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
//****************************************************************************************************/
unsigned char nRF24L01_SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned uchars)
{
	unsigned char status,uchar_ctr;
	PORT_RF_OUT clr BIT(CSN);            //SPI
	status = nRF24L01_SPI_RW(reg);
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
		nRF24L01_SPI_RW(*pBuf++);
	PORT_RF_OUT set BIT(CSN);           //SPI
	return(status);    //
}
//****************************************************************************************************/
//void SetRX_Mode(void)
//activate_receiver sets the nRF24L01 to act as a receiver. It uses write_register to set all necessary register values. The
//function activates the RF-module as receiver, sets up CRC config, address width, frequency, bandwidth and payload size. It
//also turns on PWR_UP and flushes RX_FIFO and TX_FIFO. CE is set to high when the unit acts as a receiver.
//****************************************************************************************************/
void nRF24L01_SetRX_Mode(void)
{
	PORT_RF_OUT clr BIT(CE);
	nRF24L01_SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// Enable CRC, 2 byte CRC, Recive
/*
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_LENGTH);	// Use the same address on the RX device as the TX device
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);								// Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);							// Enable Pipe0
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);									// Select RF channel 40

	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);					// Select same RX payload width as TX Payload width
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f);								// TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR

	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);								// Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
*/
	PORT_RF_OUT set BIT(CE);
	delay_ms(16);
}
//****************************************************************************************************/
//void SetTX_Mode(void)
//activate transmitter sets the nRF24L01 to act as a transmitter. Auto acknowledge, frequency, bandwidth, address width,
//CRC must be identical to the receiver. The PRIM_RX bit in the CONFIG register is set to 0 so that the unit acts as
//transmitter. CE is set to 0 since Chip Enable should only be high during transmission or really to start transmission.
//****************************************************************************************************/
void nRF24L01_SetTX_Mode(void)
{
	PORT_RF_OUT clr BIT(CE);
	nRF24L01_SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		// Enable CRC, 2 byte CRC, Send
	nRF24L01_SPI_RW_Reg(WRITE_REG + SETUP_RETR,0x2F); //Setup resend
/*
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_LENGTH);		// Writes TX_Address to nRF24L01
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_LENGTH);	// RX_Addr0 same as TX_Adr for Auto.Ack
	SPI_Write_Buf(WR_TX_PLOAD, TX_PAYLOAD, TX_PLOAD_WIDTH);				// Writes data to TX payload
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);								// Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);							// Enable Pipe0
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a);							// 500µs + 86µs, 10 retrans...

	SPI_RW_Reg(WRITE_REG + RF_CH, 40);									// Select RF channel 40
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f);								// TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);								// Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
*/
	PORT_RF_OUT set BIT(CE);
	delay_ms(16);
}
	
//****************************************************************************************************/
//unsigned char nRF24L01_SetChannel(unsigned char* rx_buf)
//****************************************************************************************************/
void nRF24L01_SetChannel(unsigned int channel)
{
	if(channel > 127) channel = 127;
	nRF24L01_SPI_RW_Reg(WRITE_REG + RF_CH, channel);
}

//****************************************************************************************************/
//unsigned char nRF24L01_SetChannel(unsigned char* rx_buf)
//****************************************************************************************************/
void nRF24L01_ClearSend(void)
{
	nRF24L01_SPI_RW_Reg(WRITE_REG+STATUS,0XFF);
}

//****************************************************************************************************/
//unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//****************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
    unsigned char revale=0;
    unsigned char sta;
    sta=nRF24L01_SPI_Read(STATUS);	// Read Status 
    if(sta&0x40)		// Data in RX FIFO
	{
		PORT_RF_OUT clr BIT(CE); 			//SPI
		nRF24L01_SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;	
	}
	nRF24L01_SPI_RW_Reg(WRITE_REG+STATUS,sta);   
	return revale;
}
//****************************************************************************************************/
//void nRF24L01_TxPacket(unsigned char * tx_buf)
//
//****************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	PORT_RF_OUT clr BIT(CE);	//Standby	
	nRF24L01_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Send Address
	nRF24L01_SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 //send data
	//SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // Send Out
	PORT_RF_OUT set BIT(CE);
	delay_us(40);
}

/*
CSN (Chip Select/Slave Select) is set low to activate the RF-module. Then sends a spi-command consisting of the register
read command (0x00) OR-ed with the register to be read. Sends a spi-command with NOP (No Operation, 0xFF) and stores
the response in the value variable. Last it sets CSN high and returns value.
*/
// --------------------END OF FILE------------------------
// -------------------------------------------------------