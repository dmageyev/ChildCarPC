#include "stm8s.h"

void SPI_RW(uint8_t* BUF, uint8_t address, uint8_t size)
{
	uint8_t _adr;
	uint8_t i;
	uint8_t *pb;
	if (BUF && size)
	{
		
		 //Формируем адрес на шине
		_adr=GPIOC->ODR & 0xf1;
		_adr|=(address & 0x07)<<1;
		GPIOC->ODR=_adr;
//	SPI->CR1|=SPI_CR1_SPE;
		GPIOC->ODR &= ~(1 << 4);
	while((SPI->SR & (uint8_t)SPI_FLAG_TXE)==RESET) { }
		for (i=0;i<size;i++)
		{
			SPI->DR=BUF[i];//*pb;
			_adr=SPI->SR; // for debug
			while((SPI->SR & (u8)SPI_FLAG_RXNE)==RESET) { }
			BUF[i]=SPI->DR;
		}
	GPIOC->ODR|=1 << 4;
	}
}