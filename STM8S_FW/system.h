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
#ifndef __SYSTEM_H
#define __SYSTEM_H

/* Includes ----------------------------------------------*/
# include "stm8s.h"
# include "sys_conf.h"

/* Exported types ----------------------------------------*/


typedef enum {
							IDLE,					// неуспользуетс€
							ACTIVE,				// активный
							DONE					// отработал
						 }
						 tTimerState;

typedef enum {
							 MSG_PROCESSED,
							 MSG_PASSED
						 }
						 tMsgHndlResult;
						
						
						 
//структура программного таймера
typedef struct 
{
  unsigned int 	l_time;     // через какое врем€ запустить
	unsigned int 	time;       // через какое врем€ запустить
	bool 					periodical;	// запускать периодически
  tTimerState 	state;      // текущее состо€ние
  void (*pFunc)(void);     	// указатель на CallBack функцию
	tMessages 		Message;		// посылаемое сообщение или NONE
} tSoftTimer;

typedef struct 
{
	uint8_t Index;
	uint8_t Mask;
} tDB;

typedef union
{
	int wParam;
	tDB dbParam;
} tShRegParam;

typedef struct
{
	tHandler 	Handl;					
	tMessages	Message;
	int		wParam;
} tMsg;
// буфер подавител€ дребезга
#define BOUNCE_DEPTH 4				// √лубина отслеживани€
typedef struct
{
	uint8_t *Input;						// указатель где хранитс€ текущее значение
	uint8_t Mask;								// маска подавител€ дребезга	
	uint8_t Index;							// активнивна€ €чейка буфера
	uint8_t Scan[BOUNCE_DEPTH];	// истори€ изменени€ бит
} tBounceBuf;

typedef struct
{
	uint8_t Output;
	uint8_t Input;
	tBounceBuf BBuf;
} tVReg;

typedef tMsgHndlResult (*pHandl)(tMsg*);

typedef struct
{
	struct
	{
		tHandler Handl;
		tMessages Message;
		pHandl pHandler;
	}  onBitChange, onBitSet, onBitClr;
} tBitHandl;

typedef tBitHandl tSPI_ShRegHand[8];


/* Exported constants ------------------------------------*/
extern unsigned long SysTimer;
extern tSoftTimer SoftTimers[MAX_TIMERS];
extern code pHandl HandlerList[MAX_FSM];
extern tVReg  SPI_Slow_Out[1];
extern tVReg SPI_Fast_IO[SPI_SHREG_SIZE];
extern code uint8_t ShRegMask[SPI_SHREG_SIZE];
extern code tSPI_ShRegHand ShRegHandlList[SPI_SHREG_SIZE];

/* Exported macro ----------------------------------------*/
#define SEC 			1000
#define MIN 			(60 * SEC)
#define HOUR 			(60 * MIN)

#define MSG_HND_BEGIN(msg)  if (Msg->Message==(msg)){
#define MSG_HND_END					return MSG_PROCESSED;	}
#define MSG_HND_NEXT(msg)		MSG_HND_END MSG_HND_BEGIN(msg)

#define CtrlLED_PIN  (GPIOE), (GPIO_PIN_0)
#define Address_PIN  (GPIOC), ((GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3))
#define SSelect_PIN  (GPIOC), (GPIO_PIN_4)
#define REVERSE_PIN(PPIN) GPIO_WriteReverse(PPIN)

// Format <array name>, <byte num>, <pin num>, <H | L>
#define _vp_mask(ar_name, num, pin, lev)		(1 << (pin))
#define _vp_wr(ar_name, num, pin, lev, val)	if(val)ar_name[(num)].Output|=(1 << (pin))\
																						else ar_name[(num)].Output&=~(1 << (pin))
#define _vp_rd(ar_name, num, pin, lev)		((ar_name[(num)].Output)&(1 << (pin)))

#define _vp_onH(ar_name, num, pin)				ar_name[(num)].Output|=1 << (pin)
#define _vp_onL(ar_name, num, pin)				ar_name[(num)].Output&=~(1 << (pin))

#define _vp_offH(ar_name, num, pin)				ar_name[(num)].Output&=~(1 << (pin))
#define _vp_offL(ar_name, num, pin)				ar_name[(num)].Output|=1 << (pin)

#define _vp_isonH(ar_name, num, pin)			(!!(_vp_rd(ar_name, num, pin, none)))
#define _vp_isonL(ar_name, num, pin)			(!(_vp_rd(ar_name, num, pin, none)))

#define _vp_set(ar_name, num, pin, lev)			_vp_onH(ar_name, num, pin)
#define _vp_clr(ar_name, num, pin, lev)			_vp_offH(ar_name, num, pin)
#define _vp_on(ar_name, num, pin, lev)			_vp_on##lev(ar_name, num, pin)
#define _vp_off(ar_name, num, pin, lev)			_vp_off##lev(ar_name, num, pin)
#define _vp_ison(ar_name, num, pin, lev)		_vp_ison##lev(ar_name, num, pin)
#define _vp_inv(ar_name, num, pin, lev)			ar_name[(num)].Output^=(1 << (pin))
#define _vp_isset(ar_name, num, pin, lev)		_vp_isonH(ar_name, num, pin)	

#define _vi_tst(ar_name, num, pin, lev)			((ar_name[(num)].Input)&(1 << (pin)))

#define _vi_actH(ar_name, num, pin)					(!!_vi_tst(ar_name, num, pin, none))			
#define _vi_actL(ar_name, num, pin)					(!_vi_tst(ar_name, num, pin, none))				
#define _vi_pasH(ar_name, num, pin)					_vi_actL(ar_name, num, pin)
#define _vi_pasL(ar_name, num, pin)					_vi_actH(ar_name, num, pin)	
#define _vi_act(ar_name, num, pin, lev)			_vi_act##lev((ar_name), (num), (pin))	
#define _vi_pas(ar_name, num, pin, lev)			_vi_pas##lev((ar_name), (num), (pin))	

#define __vp_mask(x) 		_vp_mask(x)
#define __vp_set(x) 		_vp_set(x)
#define __vp_clr(x) 		_vp_clr(x)
#define __vp_on(x)			_vp_on(x)
#define __vp_off(x)			_vp_off(x)
#define __vp_inv(x)			_vp_inv(x)
#define __vp_isset(x)		_vp_isset(x)
#define __vp_ison(x)		_vp_ison(x)
#define __vp_rd(x)			_vp_rd(x)
#define __vp_wr(x)			_vp_wr(x)

#define __vi_tst(x)			_vi_tst(x)
#define __vi_act(x)			_vi_act(x)
#define __vi_pas(x)			_vi_pas(x)


#define VPIN_MASK(Pin_Name)					__vp_mask(Pin_Name)
#define VPIN_SET(Pin_Name)					__vp_set(Pin_Name)
#define VPIN_CLR(Pin_Name)					__vp_clr(Pin_Name)
#define VPIN_ON(...)								__vp_on(__VA_ARGS__)
#define VPIN_OFF(Pin_Name)					__vp_off(Pin_Name)
#define VPIN_INV(Pin_Name)					__vp_inv(Pin_Name)
#define VPIN_IS_SET(Pin_Name)				__vp_isset(Pin_Name)
#define VPIN_IS_ON(Pin_Name)				__vp_ison(Pin_Name)
#define VPIN_READ(Pin_Name)					__vp_rd(Pin_Name)
#define VPIN_WRITE(Pin_Name,Value) 	__vp_wr(Pin_Name,Value)

#define VINP_TEST(Pin_Name) 				__vi_tst(Pin_Name)
#define VINP_ACTIVE(Pin_Name)			 	__vi_act(Pin_Name)
#define VINP_PASSIVE(Pin_Name) 			__vi_pas(Pin_Name)

/* Exported functions ----------------------------------- */

// Event Queue

void 	EQInit();     		              // инициализаци€
tMsg EQGetEvent();        			   		// вз€ть событие  
   
void EQProcNext();										// обработка следующего

void 	EQPutEvent(											// разместить событие
									tHandler 	Handl,		// идентификатор получател€
									tMessages	Message,	// тип сообщени€
									int16_t		wParam		// параметры
									);
void PostMessage(tMessages Message);	// добавить событие

// ---- Timers -----------------------//
void InitTimers();
void TimerInitF(tTimerNames TimerName, 
							 unsigned int time,
							 bool periodical,
							 tTimerState state,        
							 void (*pFunc)(void)
							 );
void TimerInitM(tTimerNames TimerName, 
							 unsigned int time,
							 bool periodical,
							 tTimerState state,
							 tMessages Message
							 );
void TimerReset(tTimerNames TimerName);
unsigned int GetTimer(tTimerNames TimerName);
void TimerTrigered(tTimerNames TimerName);

// --------- Ctrl LED ----------------------//

void CtrlLED_FSM_Init();
tMsgHndlResult CtrlLED_FSM_Handl(tMsg* Msg);

// --------- SPI Scaner ----------------------//
void DebounceInit (tBounceBuf    *buf, 
									 uint8_t       *field,
									 uint8_t        mask);
uint8_t Debounce(tBounceBuf *buf, uint8_t value);
void MessageSender(uint8_t        ch_mask, 
									 uint8_t        value, 
									 tSPI_ShRegHand SR_Handl);
									 
void F_SPI_FSM_Init ();
tMsgHndlResult F_SPI_FSM_Handl (tMsg* Msg);

void Sl_SPI_FSM_Init ();
tMsgHndlResult Sl_SPI_FSM_Handl (tMsg* Msg);

#endif /* __SYSTEM_H */

/***(C)COPYRIGHT 2013 јгеев ƒ.¬. ***END OF FILE **/ 