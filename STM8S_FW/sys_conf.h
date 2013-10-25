/**
  **********************************************************
  * @file    system.h
  * @author  Ageyev D/V/
  * @version V1.0
  * @date    25-jul-2013
  * @brief   This file contains all definitions for FSM
  *          supervisor core.
  **********************************************************
  * @attention
  *
  **********************************************************
  */

/* Define to prevent recursive inclusion -----------------*/
#ifndef __SYS_CONF_H
#define __SYS_CONF_H

#define EVENTBUF_SIZE 8			// размер буфера событий

#define MAX_FSM 			8			// количество FSM + 1
typedef enum {
							hALL=-2,
							hANY=-1,
							hEMPTY=0,
							FSM_CtrlLED,
							FAST_SPI,
              hS_SPI,
							hHLAMP,
              hPLAMP,
              hTLAMP
//							hTest_SPI

														// List of FSM`s
						 }
						 tHandler;
 		  
#define MAX_TIMERS 		4 		// максимальное число таймеров
typedef enum {
							CtrlLED,
							F_SPI,
							TurnLampTmr,
							T2
													// List of timers
						 }
						 tTimerNames;

typedef enum {
							mNONE = 0, 
							CLC1_Trig,
              mTL_Trig,
							F_SPI_Trig,
							mSH_REG_CHANGE,
							mOFF_ALL,
							mHLAMP_PRESSED,
							mHLAMP_SWITCHED,
							mREVLAMP_SWITCHED,
              mLFT_TLAMP_KEY_PRESSED,
              mRTH_TLAMP_KEY_PRESSED,
              mFAULT_KEY_PRESSED,
              mTLAMP_HC_TRIGERED,
              mTLAMP_KEY_RELEASED,
              mFAULT_KEY_RELEASED,
              
													// List of messages 
						 } 
						 tMessages;
						 
#define F_SPI_MASK_1 0xFF
#define F_SPI_MASK_2 0xFF
#define LOCAL_MSG_ID_START 192
#define SPI_SHREG_SIZE 2
#define SPI_SHREG_ADR 5
#define Sl_SHREG_ADR 6


#endif /* __SYS_CONF_H */

/***(C)COPYRIGHT 2013 Агеев Д.В. ***END OF FILE **/ 