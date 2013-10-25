/**
  **********************************************************
  * @file    fsm.�
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

tHeadlampFSMState HeadlampFSMState = sHLAMP_OFF;       /* ��������� ��������� ���� ���������    */

#ifndef PARKLAMP_NO_FSM                                /* ���� ������� ���������� �����������  
                                                        * ��� FSM, �� �������� ������           */
tParklampFSMState ParklampFSMState = sPLAMP_OFF;       /* ��������� ��������� ���� ���������    */
#endif

tRLampFSMState    RLampFSMState    = sRLamp_OFF;       /* ��������� ��������� �������� ��������� */
volatile uint8_t TurnLampCnt;                /* ������� ������ ���������               */

static  void HLampSwitch ();
static  void TurnLampSwitch ();

/* ----------------------------------------------------------------------------------------------  
 *  Function:       HLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    ���������� ������� ��� ��������� �������� ���
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
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
 *  description:    ���������� ������� ��� ��������� �������� ���������
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
                                    
tMsgHndlResult ParkLamp_FSM_Handl(tMsg* Msg)
{

#ifndef PARKLAMP_NO_FSM	     /*        �������������� ���������� �����������                      */

/*   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      */
/*                              ���������� � ��������������  FSM                                  */
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
/*     ������� ����������, ���������� ������ ����, �� ����� ����������� � �������                 */
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

#endif    /*         ����� �������������� ���������� �����������                                  */ 
}


/* ----------------------------------------------------------------------------------------------  
 *  Function:       ReverseLamp_FSM_Handl (...) 
 * ---------------------------------------------------------------------------------------------- 
 *  description:    ���������� ������� ��� ��������� �������� ������� ����
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- 
 */
/* IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
 *  ������ ���������� ��� FSM ��� ����� ���������. 
 *  ��� ��������� ��������� ���, �� ��� ����� �����������.
 *
 * !!!  ��� ���������� � ����� ������ ��������� �������� ���������� �������� �� ��������       !!!
 * !!   � �������������� ������                                                                 !!
 * IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
 */
 /* ����. 06.09.2013 �.                                                                           */
 /* TODO: ���������� �������� ������� �������. ������ ����� ����� ��������� ��������              */
 
tMsgHndlResult ReverseLamp_FSM_Handl(tMsg* Msg)
{
                                               /* ����������� ��������� ����������� ���������     */
  if (Msg->Message==mREVLAMP_SWITCHED){       /* ���� ���������� ��������� ������� ������� ����  */
		if (VINP_ACTIVE(REVERSE_LAMP_HC)) {        /* ��������� ������� ��� ���������.
                                                * ���� ������ �������� ���� �������,                                                  */
			VPIN_ON(REVERSE_LAMP_PIN);               /* �� �������� �����                               */
    }
		else	{                                    /* �����, ���� ������ ������� ���� �� �������      */
			VPIN_OFF(REVERSE_LAMP_PIN);              /* �� ��������� �����    */
    }
	  return MSG_PROCESSED;	
  }
  else  {
    return MSG_PASSED;
  }
}

static  void TurnLampSwitch ()
{
  switch (RLampFSMState)                       /* ������������ ��������� ��������� �������������� */
  {                                            /* � ��������                                      */
    case sRLamp_OFF:							// �������� ���������
		case sRLamp_IDLE:						// ���������� ���������
      VPIN_OFF(LFT_TLAMP_PIN); 
      VPIN_OFF(RTH_TLAMP_PIN);
      break;
		case sRLamp_LEFT_ON:					// ������� ������� ������
      if (TurnLampCnt & 2){
        VPIN_ON(LFT_TLAMP_PIN);
      } else {
        VPIN_OFF(LFT_TLAMP_PIN);
      }        
      VPIN_OFF(RTH_TLAMP_PIN);
		  break;
		case sRLamp_RIGHT_ON:				// ������� ������� �������
		  if (TurnLampCnt & 2){
        VPIN_ON(RTH_TLAMP_PIN);
      } else {
        VPIN_OFF(RTH_TLAMP_PIN);
      }        
      VPIN_OFF(LFT_TLAMP_PIN);
      break;
		case sRLamp_FAILT:						// ��������� ������
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
		case sRLamp_FAILT_LEFT_ON:		// ��������� ������ � ������� ������
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
		case sRLamp_FAILT_RIGHT_ON:	// ��������� ������ � ������� �������
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
 *  description:    ���������� ������� ��� ��������� �������� ��������� � ��������� �������
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
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
 *  description:    ���������� ������� ��� ��������� �������� ��������� � ��������� �������
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------------- */
 tMsgHndlResult TurnLamp_FSM_Handl(tMsg *Msg)
{
  if (Msg->Message == mTL_Trig){               /* ��� ����������� �� �������� ��������            * 
                                                * ������������ ������� �� �������.                */
    TurnLampCnt--;                              /* ���� �������� ������ ��������� - �������������  *
                                                * ��������� �������                               */
    if (!TurnLampCnt){                          /* ��������� �� ���������    ��������� �������     */
      TurnLampCnt = 4;                         /* � ������������� ���, ���� �� ���������          */ 
    }  
    TurnLampSwitch();                          /* ����������� ����� ��������� �������� ���������  *
                                                * ���������� �������� � ��������� ��������        */
    return MSG_PROCESSED;                      /* ������� �� �����������                          */    
  }    
                                               /* !!!!!!!!!!!!!!  � � � � � � � �  !!!!!!!!!!!!!!!!
                                                ! ������ ��������� �������� �� ������ � ���������.!
                                                ! ������� ���������� � ����� �� �����������       !
                                                ! ���������� ���� ��������� ���������             !
                                                ! �������������� �������� ��������� ��� ��������, !
                                                ! ����� ���������� ��������� ����� �����������,   !
                                                ! ���������, ��� ��������� ��������� ����������   !
                                                ! ��������� �������� � ���������� �������         !
                                                ! ������������ ����. ��� ��������� ���������      !
                                                ! ����� ����.                                     !
                                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                               */
                                               
  switch (RLampFSMState)                       /* ������������ ��������� ��������� �������������� */
  {                                            /* � ��������                                      */
    case sRLamp_OFF:                           /* S �������� � ��������� ������ ���������       S */
      switch (Msg->Message)
      {
        case mLFT_TLAMP_KEY_PRESSED:           /* �v ������� ����� �������                     Ev */
          RLampFSMState = sRLamp_LEFT_ON;
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev ������� ������ �������                    Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev ������� ��������� ������                  Ev */
          RLampFSMState =  sRLamp_FAILT;         
          break;
        default: return MSG_PASSED;
      }                                         
      break;                                         
    case sRLamp_LEFT_ON:                       /* S ����� �������  ���, ��������� ������  ����  S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* �v ��������� ��������                        Ev */
          RLampFSMState = sRLamp_OFF;          
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev ������� ������ �������                    Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev ������� ��������� ������                  Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* �v �������� ������ �������� ����             Ev */
          RLampFSMState = sRLamp_OFF;            
          break;
        default: return MSG_PASSED;
      }
      break;                                                                                        
    case sRLamp_RIGHT_ON:                      /* S ������ ������� ���, ��������� ������  ����  S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* �v ��������� ��������                        Ev */
          RLampFSMState = sRLamp_OFF;          
          break;        
        case mLFT_TLAMP_KEY_PRESSED:           /* Ev ������� ����� �������                     Ev */
          RLampFSMState = sRLamp_LEFT_ON;          
          break;        
        case mFAULT_KEY_PRESSED:               /* Ev ������� ��������� ������                  Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* �v �������� ������ �������� ����             Ev */
          RLampFSMState = sRLamp_OFF;           
          break;
        default: return MSG_PASSED;
      }
      break;                                          
    case sRLamp_FAILT:                         /* S ��������      ����, ��������� ������  ���   S */
      switch (Msg->Message)
      {
        case mLFT_TLAMP_KEY_PRESSED:           /* �v ������� ����� �������                     Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev ������� ������ �������                    Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;
        case mFAULT_KEY_RELEASED:              /* Ev �������� ��������� ������                 Ev */
          RLampFSMState = sRLamp_OFF;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    case sRLamp_FAILT_LEFT_ON:                 /* S ����� �������  ���, ��������� ������  ���   S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* �v ��������� ��������                        Ev */
          RLampFSMState = sRLamp_FAILT;          
          break;        
        case mRTH_TLAMP_KEY_PRESSED:           /* Ev ������� ������ �������                    Ev */
          RLampFSMState = sRLamp_FAILT_RIGHT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* �v �������� ������ �������� ����             Ev */
          RLampFSMState = sRLamp_FAILT;          
          break;        
        case mFAULT_KEY_RELEASED:              /* Ev �������� ��������� ������                 Ev */
          RLampFSMState = sRLamp_LEFT_ON;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    case sRLamp_FAILT_RIGHT_ON:                /* S ������ ������� ���, ��������� ������  ���   S */
      switch (Msg->Message)
      {
        case mTLAMP_KEY_RELEASED:              /* �v ��������� ��������                        Ev */
          RLampFSMState = sRLamp_FAILT;             
          break;        
        case mLFT_TLAMP_KEY_PRESSED:           /* Ev ������� ����� �������                     Ev */
          RLampFSMState = sRLamp_FAILT_LEFT_ON;          
          break;        
        case mTLAMP_HC_TRIGERED:               /* �v �������� ������ �������� ����             Ev */
          RLampFSMState = sRLamp_FAILT;             
          break;        
        case mFAULT_KEY_RELEASED:              /* Ev �������� ��������� ������                 Ev */
          RLampFSMState = sRLamp_RIGHT_ON;          
          break;
        default: return MSG_PASSED;
      }
      break;                                         
    default: {return MSG_PASSED;}
  }                                           
  TurnLampSwitch();                            /* ����������� ����� ��������� �������� ���������  *
                                                * ���������� �������� � ��������� ��������        */
  return MSG_PROCESSED;                        /* ������� �� �����������                          */
}

/***    (C)COPYRIGHT 2013 ����� �.�.        ***END OF FILE ***  */        