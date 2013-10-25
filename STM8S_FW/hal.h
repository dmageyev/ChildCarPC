#ifndef __HAL_H
#define __HAL_H

#define BEEP_ON(freq)  BEEP_Init(BEEP_FREQUENCY_1KHZ);

void SPI_RW(uint8_t* BUF, uint8_t address, uint8_t size);
#endif