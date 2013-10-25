/**
  **********************************************************
  * @file    fsm.с
  * @author  Ageyev D.V.
  * @version V1.0
  * @date    7-aug-2013
  * @brief   This file contains all definitions for FSM.
  **********************************************************
  * @attention
  *
  **********************************************************
  */

/* Includes ----------------------------------------------*/
#include "fsm.h"

tHeadlampFSMState HeadlampFSMState = sHLAMP_OFF;       /* Начальное состояние фары выключены    */

#ifndef PARKLAMP_NO_FSM                                /* Если выбрана реализации обработчика  
                                                        * без FSM, то экономим память           */
tParklampFSMState ParklampFSMState = sPLAMP_OFF;       /* Начальное состояние фары выключены    */
#endif

tRLampFSMState    RLampFSMState    = sRLamp_OFF;       /* Начальное состояние повороты выключены */
volatile uint8_t TurnLampCnt;                /* Счетчик тактов поворотов               */

static  void HLampSwitch ();
static  void TurnLampSwitch ();

/* ----------------------------------------------------------------------------------------------  
 *  Function:       HLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    Обработчие событий для конечного автомата фар
 * 
 *  parameters:     tMsg *Msg      – обрабатывоемое сообщение 
 * 
 *  on return:      tMsgHndlResult -  результат обработки (обработано, пропушено)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
  
tMsgHndlResult HLamp_FSM_Handl(tMsg* Msg)
{
	
	switch (HeadlampFSMState)
	{
	case sHLAMP_OFF:
		   MSG_HND_BEGIN(mHLAMP_PRESSED) // if (Msg->Message==(mHLAMP_PRESSED){
			   HLampSwitch();
		   MSG_HND_END
		break;
	case sHLAMP_FAR_ON:
		MSG_HND_BEGIN(mHLAMP_PRESSED)
			VPIN_OFF(FAR_HLAMP_PIN);
			HeadlampFSMState=sHLAMP_OFF;
		MSG_HND_NEXT(mHLAMP_SWITCHED)
			HLampSwitch();
		MSG_HND_END
	  break;
	case sHLAMP_NEAR_ON:
		MSG_HND_BEGIN(mHLAMP_PRESSED)
			VPIN_OFF(NEAR_HLAMP_PIN);
			HeadlampFSMState=sHLAMP_OFF;
		MSG_HND_NEXT(mHLAMP_SWITCHED)
			HLampSwitch();
		MSG_HND_END
  }	
  return MSG_PASSED;
}

void HLampSwitch()
{
		if (VINP_ACTIVE(FAR_HLAMP_SWITCH))
		{
			VPIN_ON(FAR_HLAMP_PIN);
			VPIN_OFF(NEAR_HLAMP_PIN);
			HeadlampFSMState=sHLAMP_FAR_ON;
		}
		else
		{
			VPIN_ON(NEAR_HLAMP_PIN);
			VPIN_OFF(FAR_HLAMP_PIN);
			HeadlampFSMState=sHLAMP_NEAR_ON;
		}
	}


/* ----------------------------------------------------------------------------------------------  
 *  Function:       ParkLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    Обработчие событий для конечного автомата габаритов
 * 
 *  parameters:     tMsg *Msg      – обрабатывоемое сообщение 
 * 
 *  on return:      tMsgHndlResult -  результат обработки (обработано, пропушено)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
                                    
tMsgHndlResult ParkLamp_FSM_Handl(tMsg* Msg)
{

#ifndef PARKLAMP_NO_FSM	     /*        Альтернативная реализация обработчика                      */

/*   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      */
/*                              Реализация с использованием  FSM                                  */
/* ---------------------------------------------------------------------------------------------- */
	switch (ParklampFSMState)
	{
    case sPLAMP_OFF:
      MSG_HND_BEGIN(mPLAMP_PRESSED) // if (Msg->Message==(mHLAMP_PRESSED){
        VPIN_OFF(FAR_HLAMP_PIN);
        ParklampFSMState = sPLAMP_ON;
      MSG_HND_END
      break;
    case sPLAMP_ON:
      MSG_HND_BEGIN(mPLAMP_PRESSED)
        VPIN_OFF(FAR_HLAMP_PIN);
        ParklampFSMState = sPLAMP_OFF;
      MSG_HND_END
  }	
  return MSG_PASSED;
/*   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      */

#else

/*   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      */
/*     Простая реализация, занимающая меньше кода, но плохо расширяемая в будущем                 */
/* ---------------------------------------------------------------------------------------------- */
  if (Msg->Message == mHLAMP_PRESSED){
		VPIN_INV(FAR_HLAMP_PIN);
	  return MSG_PROCESSED;	
  }
  else
  {
    return MSG_PASSED;
  }
/*   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      */  

#endif    /*         Конец альтернативной реализация обработчика                                  */ 
}


/* ----------------------------------------------------------------------------------------------  
 *  Function:       ReverseLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    Обработчие событий для конечного автомата заднего хода
 * 
 *  parameters:     tMsg *Msg      – обрабатывоемое сообщение 
 * 
 *  on return:      tMsgHndlResult -  результат обработки (обработано, пропушено)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- 
 */
/* IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
 *  Данная реализация для FSM без учета состояний. 
 *  Это позволяет упростить код, но она плохо расширяемая.
 *
 * !!!  При реализации с явням учетом состояния автомата необходимо добавить их описание       !!!
 * !!   в соответсвующую секцию                                                                 !!
 * IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
 */
 /* Прим. 06.09.2013 г.                                                                           */
 /* TODO: необходимо добавить функцию пишалки. Значит нужно явные состояния автомата              */
 
tMsgHndlResult ReverseLamp_FSM_Handl(tMsg* Msg)
{
                                               /* Обязательно проверяем поступивщее сообщение     */
  if (Msg->Message==mREVLAMP_SWITCHED){       /* Если изменилось состояние датчика заднего хода  */
		if (VINP_ACTIVE(REVERSE_LAMP_HC)) {        /* Проверяем текущее его состояние.
                                                * Если датчик Ззаднего хода активен,                                                  */
			VPIN_ON(REVERSE_LAMP_PIN);               /* то включаем лампу                               */
    }
		else	{                                    /* Иначе, если датчик заднего хода не активен      */
			VPIN_OFF(REVERSE_LAMP_PIN);              /* то выключаем лампу    */
    }
	  return MSG_PROCESSED;	
  }
  else  {
    return MSG_PASSED;
  }
}

static  void TurnLampSwitch ()
{
  switch (RLampFSMState)                       /* Обрабатываем изменения состояний переключателей */
  {                                            /* и датчиков                                      */
    case sRLamp_OFF:							// Повороты выключены
		case sRLamp_IDLE:						// Неактивное состояние
      VPIN_OFF(LFT_TLAMP_PIN); 
      VPIN_OFF(RTH_TLAMP_PIN);
      break;
		case sRLamp_LEFT_ON:					// Включен поворот налево
      if (TurnLampCnt & 2){
        VPIN_ON(LFT_TLAMP_PIN);
      } else {
        VPIN_OFF(LFT_TLAMP_PIN);
      }        
      VPIN_OFF(RTH_TLAMP_PIN);
		  break;
		case sRLamp_RIGHT_ON:				// Включен поворот направо
		  if (TurnLampCnt & 2){
        VPIN_ON(RTH_TLAMP_PIN);
      } else {
        VPIN_OFF(RTH_TLAMP_PIN);
      }        
      VPIN_OFF(LFT_TLAMP_PIN);
      break;
		case sRLamp_FAILT:						// Аварийный сигнал
		  if (TurnLampCnt & 2){
        VPIN_ON(LFT_TLAMP_PIN);
        VPIN_ON(RTH_TLAMP_PIN);
        VPIN_ON(RELE_TLAMP_PIN);
        VPIN_ON(FAULT_LAMP_PIN);
      } else {
        VPIN_OFF(LFT_TLAMP_PIN);
        VPIN_OFF(RTH_TLAMP_PIN);
        VPIN_OFF(RELE_TLAMP_PIN);
        VPIN_OFF(FAULT_LAMP_PIN);
      }
      break;
		case sRLamp_FAILT_LEFT_ON:		// Аварийный сигнал и поворот налево
		  if (TurnLampCnt & 1){
        VPIN_ON(LFT_TLAMP_PIN);
      } else {
        VPIN_OFF(LFT_TLAMP_PIN);
      }
      if (TurnLampCnt & 2){
        VPIN_ON(RTH_TLAMP_PIN);
        VPIN_ON(RELE_TLAMP_PIN);
        VPIN_ON(FAULT_LAMP_PIN);
      } else {
        VPIN_OFF(RTH_TLAMP_PIN);
        VPIN_OFF(RELE_TLAMP_PIN);
        VPIN_OFF(FAULT_LAMP_PIN);
      }
      break;
		case sRLamp_FAILT_RIGHT_ON:	// Аварийный сигнал и поворот направо
      if (TurnLampCnt & 1){
        VPIN_ON(RTH_TLAMP_PIN);
      } else {
        VPIN_OFF(RTH_TLAMP_PIN);
      }
      if (TurnLampCnt & 2){
        VPIN_ON(LFT_TLAMP_PIN);
        VPIN_ON(RELE_TLAMP_PIN);
        VPIN_ON(FAULT_LAMP_PIN);
      } else {
        VPIN_OFF(LFT_TLAMP_PIN);
        VPIN_OFF(RELE_TLAMP_PIN);
        VPIN_OFF(FAULT_LAMP_PIN);
      }
  }         
}
/* ----------------------------------------------------------------------------------------------  
 *  Function:       TurnLamp_FSM_Init (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    Обработчие событий для конечного автомата поворотов и аврийного сигнала
 * 
 *  parameters:     tMsg *Msg      – обрабатывоемое сообщение 
 * 
 *  on return:      tMsgHndlResult -  результат обработки (обработано, пропушено)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
 
void           TurnLamp_FSM_Init  (void)
{
  VPIN_OFF(LFT_TLAMP_PIN); 
  VPIN_OFF(RTH_TLAMP_PIN);
  TurnLampCnt = 4;
  RLampFSMState = sRLamp_OFF;
  TimerInitM(TurnLampTmr,500,TRUE,ACTIVE,mTL_Trig);
}
/* ----------------------------------------------------------------------------------------------  
 *  Function:       TurnLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    Обработчие событий для конечного автомата поворотов и аврийного сигнала
 * 
 *  parameters:     tMsg *Msg      – обрабатывоемое сообщение 
 * 
 *  on return:      tMsgHndlResult -  результат обработки (обработано, пропушено)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
 tMsgHndlResult TurnLamp_FSM_Handl(tMsg *Msg)
{
  if (Msg->Message == mTL_Trig){               /* Вне зависимости от состяния автомата            * 
                                                * обрабатываем события от таймера.                */
    TurnLampCnt--;                              /* Если сработал таймер поворотов - декриминируем  *
                                                * локальный счетчик                               */
    if (!TurnLampCnt){                          /* Проверяем на обнуление    локальный счетчик     */
      TurnLampCnt = 4;                         /* и перезапускаем его, если он обнулился          */ 
    }  
    TurnLampSwitch();                          /* Переключаем лампы поворотов согласно состоянию  *
                                                * локального счетчика и состоянию автомата        */
    return MSG_PROCESSED;                      /* Выходим из обработчика                          */    
  }    
                                               /* !!!!!!!!!!!!!!  В Н И М А Н И Е  !!!!!!!!!!!!!!!!
                                                ! Логика обработки отличает от других в программе.!
                                                ! Возврат результата и выход из обработчика       !
                                                ! происходит если поступило сообщение             !
                                                ! несвойственное текущему состоянию или автомату, !
                                                ! иначе управление достигает конца обработчика,   !
                                                ! считается, что поступило сообщение изменивщее   !
                                                ! состояние автомата и вызывается функция         !
                                                ! переключения ламп. Это позволяет уменьшить      !
                                                ! длину кода.                                     !
                                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                               */
                                               
  switch (RLampFSMState)                       /* Обрабатываем изменения состояний переключателей */
  {                                            /* и датчиков                                      */
    case sRLamp_OFF:                           /* S Повороты и аварийный сигнал выключены       S */
      switch (Msg->Message)
      {
        case mLFT_TLAMP_KEY_PRESSED:           /* Еv Включен левый поворот                     Ev */
          RLampFSMState = sRLamp_LEFT_ON;
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev Включен правый поворот                    Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev Включен аварийный сигнал                  Ev */
          RLampFSMState =  sRLamp_FAILT;         
          break;
        default: return MSG_PASSED;
      }                                         
      break;                                         
    case sRLamp_LEFT_ON:                       /* S Левый поворот  ВКЛ, аварийный сигнал  ВЫКЛ  S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* Еv Выключены повороты                        Ev */
          RLampFSMState = sRLamp_OFF;          
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev Включен правый поворот                    Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev Включен аварийный сигнал                  Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* Еv Сработал датчик возврата руля             Ev */
          RLampFSMState = sRLamp_OFF;            
          break;
        default: return MSG_PASSED;
      }
      break;                                                                                        
    case sRLamp_RIGHT_ON:                      /* S Правый поворот ВКЛ, аварийный сигнал  ВЫКЛ  S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* Еv Выключены повороты                        Ev */
          RLampFSMState = sRLamp_OFF;          
          break;        
        case mLFT_TLAMP_KEY_PRESSED:           /* Ev Включен левый поворот                     Ev */
          RLampFSMState = sRLamp_LEFT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev Включен аварийный сигнал                  Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* Еv Сработал датчик возврата руля             Ev */
          RLampFSMState = sRLamp_OFF;           
          break;
        default: return MSG_PASSED;
      }
      break;                                          
    case sRLamp_FAILT:                         /* S Повороты      ВЫКЛ, аварийный сигнал  ВКЛ   S */
      switch (Msg->Message)
      {
        case mLFT_TLAMP_KEY_PRESSED:           /* Еv Включен левый поворот                     Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev Включен правый поворот                    Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;
        case mFAULT_KEY_RELEASED:              /* Ev Выключен аварийный сигнал                 Ev */
          RLampFSMState = sRLamp_OFF;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    case sRLamp_FAILT_LEFT_ON:                 /* S Левый певорот  ВКЛ, аварийный сигнал  ВКЛ   S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* Еv Выключены повороты                        Ev */
          RLampFSMState = sRLamp_FAILT;          
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev Включен правый поворот                    Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* Еv Сработал датчик возврата руля             Ev */
          RLampFSMState = sRLamp_FAILT;          
          break;        
        case mFAULT_KEY_RELEASED:              /* Ev Выключен аварийный сигнал                 Ev */
          RLampFSMState = sRLamp_LEFT_ON;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    case sRLamp_FAILT_RIGHT_ON:                /* S Правый поворот ВКЛ, аварийный сигнал  ВКЛ   S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* Еv Выключены повороты                        Ev */
          RLampFSMState = sRLamp_FAILT;             
          break;        
        case mLFT_TLAMP_KEY_PRESSED:           /* Ev Включен левый поворот                     Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* Еv Сработал датчик возврата руля             Ev */
          RLampFSMState = sRLamp_FAILT;             
          break;        
        case mFAULT_KEY_RELEASED:              /* Ev Выключен аварийный сигнал                 Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    default: {return MSG_PASSED;}
  }                                           
  TurnLampSwitch();                            /* Переключаем лампы поворотов согласно состоянию  *
                                                * локального счетчика и состоянию автомата        */
  return MSG_PROCESSED;                        /* Выходим из обработчика                          */
}

/***    (C)COPYRIGHT 2013 Агеев Д.В.        ***END OF FILE ***  */        