/**
  **********************************************************
  * @file    system.c
  * @author  Ageyev D.V.
  * @version V1.0
  * @date    25-jul-2013
  * @brief   This file contains all definitions for FSM
  *          supervisor core.
  **********************************************************
  * @attention
  *
  **********************************************************
  */

/* Includes ----------------------------------------------*/
#include "system.h" 
#include "hal.h"
#include "stdio.h"

/* Private types -----------------------------------------*/

/* Exported constants ------------------------------------*/
/* Varibles ----------------------------------------------*/



//кольцевой буфер

volatile struct 
{
	tMsg CycleBuf[EVENTBUF_SIZE];
	unsigned char tailBuf;
	unsigned char headBuf;
	unsigned char countBuf;
} EventBuf;


volatile  tVReg SPI_Slow_Out[1];
volatile tVReg SPI_Fast_IO[SPI_SHREG_SIZE];

//массив указателей на таймеры
volatile tSoftTimer SoftTimers[MAX_TIMERS];
// Системный таймер
volatile unsigned long SysTimer=0; 

/* Exported macro ----------------------------------------*/
/* Exported functions ----------------------------------- */

//---------------------------------------------------------

void InitTimers()
{ 
	uint8_t i;
	SysTimer=0;
	for (i=0; i<MAX_TIMERS; i++)
	{
		SoftTimers[i].time=0;
		SoftTimers[i].l_time=0;
		SoftTimers[i].periodical=FALSE;
    SoftTimers[i].state=IDLE;
    SoftTimers[i].pFunc=0;
	  SoftTimers[i].Message=mNONE;
	}
}
//------------------------------------------
void TimerInitF(tTimerNames TimerName, 
							 unsigned int time,
							 bool periodical,
							 tTimerState state,        
							 void (*pFunc)(void)
						 )
{
		SoftTimers[TimerName].l_time=time;
		SoftTimers[TimerName].time=time;
		SoftTimers[TimerName].periodical=periodical;
    SoftTimers[TimerName].pFunc=pFunc;
	  SoftTimers[TimerName].Message=mNONE;
		SoftTimers[TimerName].state=state;
}
//-----------------------------------------						 
void TimerInitM(tTimerNames TimerName, 
							 unsigned int time,
							 bool periodical,
							 tTimerState state,
							 tMessages Message
						 )
{
		TimerInitF(TimerName,time,periodical,state,0);
	  SoftTimers[TimerName].Message=Message;
}
//---------------------------------------
void TimerReset(tTimerNames TimerName)
{
	if (SoftTimers[TimerName].state!=IDLE)
  {
		SoftTimers[TimerName].l_time = 
		                             SoftTimers[TimerName].time;
		if (SoftTimers[TimerName].state==DONE)
				SoftTimers[TimerName].state=ACTIVE;
	}
}
//---------------------------------------
unsigned int GetTimer(tTimerNames TimerName)
{
	return SoftTimers[TimerName].time - 
	                             SoftTimers[TimerName].l_time;
}
//---------------------------------------

void TimerTrigered(tTimerNames TimerName)
{
		if(SoftTimers[TimerName].state==ACTIVE)
		{
			if(SoftTimers[TimerName].pFunc)
				SoftTimers[TimerName].pFunc();
			if(SoftTimers[TimerName].Message!= mNONE)
				PostMessage(SoftTimers[TimerName].Message);
			if(SoftTimers[TimerName].periodical)
			    SoftTimers[TimerName].l_time=
						SoftTimers[TimerName].time;
				else
					SoftTimers[TimerName].state=DONE;
		}			
		
}

void PostMessage(tMessages Message)
{
	EQPutEvent(hALL, Message,0);
}

void 	EQInit()
{
	uint8_t i;
	for (i=0;i<EVENTBUF_SIZE;i++) 
			EventBuf.CycleBuf[i].Message=mNONE;
			EventBuf.CycleBuf[i].Handl=hEMPTY;
	EventBuf.tailBuf = 0;
  EventBuf.headBuf = 0;
  EventBuf.countBuf = 0;
}

tMsg EQGetEvent()
{
	tMsg _msg;
	if (EventBuf.countBuf > 0)
	{
		_msg=EventBuf.CycleBuf[EventBuf.headBuf];
		EventBuf.CycleBuf[EventBuf.headBuf].Message=mNONE;
		EventBuf.CycleBuf[EventBuf.headBuf].Handl=hEMPTY;
		EventBuf.countBuf--;                         //уменьшить счетчик 
    EventBuf.headBuf++;                          //инкрементировать индекс головы буфера  
    if (EventBuf.headBuf == EVENTBUF_SIZE) 
				EventBuf.headBuf = 0;
	}
	else
	{
		_msg.Handl=hEMPTY;
		_msg.Message=mNONE;
		_msg.wParam=0;
	}
	return _msg;
}

void 	EQPutEvent(tHandler Handl,tMessages	Message,
									 int16_t wParam)
{
	if (EventBuf.countBuf < EVENTBUF_SIZE){                    //если в буфере еще есть место                     
      EventBuf.CycleBuf[EventBuf.tailBuf].Handl = Handl;               //кинуть событие в буфер
      EventBuf.CycleBuf[EventBuf.tailBuf].Message = Message;
			EventBuf.CycleBuf[EventBuf.tailBuf].wParam = wParam;
			EventBuf.tailBuf++;                               //увеличить индекс хвоста буфера 
      if (EventBuf.tailBuf == EVENTBUF_SIZE) 
					EventBuf.tailBuf = 0;  
      EventBuf.countBuf++;                              //увеличить счетчик 
  }
}

void EQProcNext()
{
	tMsg _msg;
	uint8_t i;
	
	_msg=EQGetEvent();
	if (_msg.Handl)
	{
		if ((_msg.Handl>hEMPTY) && (_msg.Handl<MAX_FSM) )
		{
			if (HandlerList[_msg.Handl])
				HandlerList[_msg.Handl](&_msg);
		}
		else
		{
			tMsgHndlResult fl;
			for (i=1;i<MAX_FSM;i++)
			{
				if (HandlerList[i]) fl=HandlerList[i](&_msg);
				if (!fl && (_msg.Handl==hANY)) break;
			}
		}
	}
}

// --------- Ctrl LED ----------------------//

enum{
			CtrlLED_IDLE,
			CtrlLED_STARTING,
			CtrlLED_RUNING
		}  CtrlLED_State;
		
char CtrlLED_Count;

void CtrlLED_FSM_Init()
{
	CtrlLED_Count=8;
	CtrlLED_State=CtrlLED_STARTING;
	TimerInitM(CtrlLED,200,TRUE,ACTIVE,CLC1_Trig);
	printf("Starting\n\r");
	GPIOE->ODR&= 0xDF;
	
              
                BEEP_Init(BEEP_FREQUENCY_1KHZ);
                BEEP_Cmd(ENABLE);
}

tMsgHndlResult CtrlLED_FSM_Handl(tMsg* Msg)
{
	uint8_t _adr;
	switch (CtrlLED_State)
	{
		case CtrlLED_STARTING:
			MSG_HND_BEGIN(CLC1_Trig)
			
			BEEP_Cmd(DISABLE);
				REVERSE_PIN(CtrlLED_PIN);
				_adr=GPIOC->ODR & 0xf1;
				GPIOC->ODR=_adr|((CtrlLED_Count & 0x07)<<1);
				if (!(CtrlLED_Count--))
				{
					CtrlLED_State=CtrlLED_RUNING;
					TimerInitM(CtrlLED,500,TRUE,ACTIVE,CLC1_Trig);
//					printf("Started\n\r");
					GPIOE->ODR|= 0x20;
				}
			MSG_HND_END	
			break;
		case	CtrlLED_RUNING:
			MSG_HND_BEGIN(CLC1_Trig)
				REVERSE_PIN(CtrlLED_PIN);
//				printf("Step at %lu - %u \n\r",SysTimer, EventBuf.countBuf);
			MSG_HND_END
			break;
	default: {};
	return MSG_PASSED;
	}
}
// --------- SPI Scaner ----------------------//
void DebounceInit(tBounceBuf* buf, 
										 uint8_t* field,
										 uint8_t mask)
{
	uint8_t i;
	buf->Mask=mask;
	buf->Input=field;
	buf->Index=0;
	for (i=1;i<BOUNCE_DEPTH;i++) buf->Scan[i]=0;
	buf->Scan[0]=*field;
}

uint8_t Debounce(tBounceBuf* buf, uint8_t value)
{
	uint8_t _t0=0xFF;
	uint8_t _t1=0;
	uint8_t _mask;
	uint8_t i;
	(buf->Index)++;
	if (buf->Index==BOUNCE_DEPTH) buf->Index=0;
	buf->Scan[buf->Index]=value;
	for (i=0;i<BOUNCE_DEPTH;i++)
	{
		_t0 &= buf->Scan[i];		// ищем "0"
		_t1 |= buf->Scan[i];		// ищем "1"
	}
	_mask = ~(_t0 ^ _t1); 		// стабильные биты = "1" 
	_mask |= ~(buf->Mask);		// учитываем маску и биты которые
														// можно записывать ="1"
	_t0=*(buf->Input);				// запоминаем старое значение
	*(buf->Input)&=~_mask;			// очишаем биты кот. нужно изменить
	*(buf->Input)|=_mask & value;// записываем новое значение
	return _mask & (_t0 ^ value);	
}

void MessageSender(uint8_t ch_mask, 
									 uint8_t value, 
									 tSPI_ShRegHand SR_Handl)
{
	uint8_t i,
					_mask=1;
					
	if (ch_mask)
	{
		for (i=0;i<8;i++)
		{
			if (_mask & ch_mask)
			{
				if (SR_Handl[i].onBitChange.Handl 
						&& SR_Handl[i].onBitChange.Message) 
							EQPutEvent(SR_Handl[i].onBitChange.Handl,
												 SR_Handl[i].onBitChange.Message,
												 _mask & value);
				if (_mask & value)
				{
					if (SR_Handl[i].onBitSet.Handl 
						&& SR_Handl[i].onBitSet.Message) 
							EQPutEvent(SR_Handl[i].onBitSet.Handl,
												 SR_Handl[i].onBitSet.Message,0);
				}
				else
				{
					if (SR_Handl[i].onBitClr.Handl 
						&& SR_Handl[i].onBitClr.Message) 
							EQPutEvent(SR_Handl[i].onBitClr.Handl,
												 SR_Handl[i].onBitClr.Message,0);
				}
			}
			_mask=_mask << 1;
		}	// for
	}		// if (ch_mask
}

void F_SPI_FSM_Init()
{
	uint8_t i;
	uint8_t _buf[SPI_SHREG_SIZE];
	
	for (i=0;i<SPI_SHREG_SIZE;i++){
		SPI_Fast_IO[i].Output=0x83; //test string
		_buf[i]=SPI_Fast_IO[i].Output; // инит регистр
	}
	SPI_RW(_buf, SPI_SHREG_ADR,SPI_SHREG_SIZE);
	
	for (i=0;i<SPI_SHREG_SIZE;i++)
		SPI_Fast_IO[i].Input=_buf[i];
		
	for (i=0;i<SPI_SHREG_SIZE;i++)
		DebounceInit(&(SPI_Fast_IO[i].BBuf),
								 &(SPI_Fast_IO[i].Input),
								 ShRegMask[i]);
								 
	TimerInitM(F_SPI,25,TRUE,ACTIVE,F_SPI_Trig);
}

tMsgHndlResult F_SPI_FSM_Handl(tMsg* Msg)
{
	uint8_t _buf[SPI_SHREG_SIZE],
					i,
					_ch_mask;
	tShRegParam _param;
	MSG_HND_BEGIN(F_SPI_Trig)
		for (i=0;i<SPI_SHREG_SIZE;i++)
			_buf[i]=SPI_Fast_IO[i].Output; // инит регистр
		SPI_RW(_buf, SPI_SHREG_ADR, SPI_SHREG_SIZE);
		for (i=0;i<SPI_SHREG_SIZE;i++)
		{
			_ch_mask=Debounce(&(SPI_Fast_IO[i].BBuf),_buf[i]);
			_param.dbParam.Index=i;
			_param.dbParam.Mask=_ch_mask;
			if (_ch_mask)
					EQPutEvent(hALL,mSH_REG_CHANGE,_param.wParam);
			MessageSender(_ch_mask,
										_buf[i],
										ShRegHandlList[i]);
		}
	MSG_HND_END
	return MSG_PASSED;
}

void Sl_SPI_FSM_Init()
{
	uint8_t _buf[1];
	
		_buf[0]=0; // инит регистр	
		SPI_Slow_Out[0].Output=_buf[0]; //test string

	SPI_RW(_buf, Sl_SHREG_ADR,1);
	SPI_Slow_Out[0].Input=_buf[0];
									 
//	TimerInitM(F_SPI,25,TRUE,ACTIVE,F_SPI_Trig);
}

tMsgHndlResult Sl_SPI_FSM_Handl(tMsg* Msg)
{
	uint8_t _buf[1];
	
	MSG_HND_BEGIN(F_SPI_Trig)
	
		_buf[0]=SPI_Slow_Out[0].Output; // инит регистр
		SPI_RW(_buf, Sl_SHREG_ADR, 1);
		SPI_Slow_Out[0].Input=_buf[0];
	MSG_HND_END
	return MSG_PASSED;
}

/***(C)COPYRIGHT 2013 Агеев Д.В. ***END OF FILE **/ 