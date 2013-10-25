/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "system.h"
#include "stm8s_gpio.h"
#include "fsm.h"
#include "stm8s_beep.h"


/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

code pHandl HandlerList[MAX_FSM]={
	0,	CtrlLED_FSM_Handl,	F_SPI_FSM_Handl, Sl_SPI_FSM_Handl,	
	HLamp_FSM_Handl, 0, TurnLamp_FSM_Handl, 	0
};
code tSPI_ShRegHand ShRegHandlList[SPI_SHREG_SIZE]={
	{
//  	onBitChange				onBitSet				onBitClr		
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit0 кн.зажигания
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit1 кн.клаксона
		{{hEMPTY,mNONE,0},{hALL,mTLAMP_HC_TRIGERED,0},{hEMPTY,mNONE,0}}, // Bit2 герк. пр.поворот
		{{hEMPTY,mNONE,0},{hALL,mTLAMP_HC_TRIGERED,0},{hEMPTY,mNONE,0}},	// Bit3 герк. лв.поворот
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit4 герк. задн.ход
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit5
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit6
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}}	// Bit7
	},
	{
		{{hALL,mHLAMP_SWITCHED,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit0 перкл. бл/дал
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hALL,mHLAMP_PRESSED,0}},	// Bit1 кн.фары
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}}, // Bit2 габариты
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}},	// Bit3 пр.туман
		{{hEMPTY,mNONE,0},{hALL,mTLAMP_KEY_RELEASED,0},{hALL,mLFT_TLAMP_KEY_PRESSED,0}},	// Bit4 кн.лев.поворот
		{{hEMPTY,mNONE,0},{hALL,mTLAMP_KEY_RELEASED,0},{hALL,mRTH_TLAMP_KEY_PRESSED,0}},	// Bit5 кн.прв.поворот
		{{hEMPTY,mNONE,0},{hALL,mFAULT_KEY_RELEASED,0},{hALL,mFAULT_KEY_PRESSED,0}},	// Bit6 кн.авар.сигнал
		{{hEMPTY,mNONE,0},{hEMPTY,mNONE,0},{hEMPTY,mNONE,0}}	// Bit7
	}
};
code uint8_t ShRegMask[SPI_SHREG_SIZE]={
																				F_SPI_MASK_1,
																				F_SPI_MASK_2
																			 };

void main(void)
{
  
  
	CLK->ECKR|=CLK_ECKR_HSEEN; //Разрешаем работу генератора с внешним кварцем (HSEEN)
  CLK->SWCR|=CLK_SWCR_SWEN;  //Разрешаем автопереключение источника Clock при неисправности генератора
  CLK->SWR=0xB4;             //Включаем clock от кварцевого генератора (HSE)
  CLK->CKDIVR=0;             //Делители частоты внутреннего и внешнего генератора на 1 - частота ядра максимальная
  while(CLK->CMSR!=0xB4);    //Ждем стабилизации частоты
  CLK->CSSR|=CLK_CSSR_CSSEN; //Разрешаем автопереключение источника Clock на внутренний при неисправности генератора

// Инициализация SPI
	SPI->ICR=0;
	SPI->SR=0;
	SPI->CR2=3;
	SPI->CR1=0x7C;//0x04;
	
// Инициализация портов	
	GPIO_Init(CtrlLED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(Address_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(SSelect_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_WriteHigh(SSelect_PIN);
	
// Инициализация  UART
   UART2->BRR1|=0x68;
   UART2->BRR2|=0x03; // Baudrate 9600
   UART2->CR2|=UART2_CR2_TEN;
   UART2->CR2|=UART2_CR2_REN;
   UART2->CR3|=0x20;  // 2 stop bits

// Инициализация системных драйверов
	InitTimers();
	EQInit(); 
	CtrlLED_FSM_Init();
	F_SPI_FSM_Init();
	Sl_SPI_FSM_Init();
  TurnLamp_FSM_Init();
	

// Инициализация системного таймера
	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_64, 250);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	
	
	enableInterrupts();
	while (1)
  {
		
//		SPI_RW(15);
EQProcNext();
		
  }
  
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
