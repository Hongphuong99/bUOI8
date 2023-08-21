#ifndef SPI_H_
#define SPI_H_

#include <msp430g2553.h>

void Config_SPI_A(void);
void Config_SPI_B(void);
void Send_byte_A(int data, int time_ms);
void Send_byte_B(int data, int time_ms);



#endif /* SPI_H_ */
