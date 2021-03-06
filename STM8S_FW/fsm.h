/**
  **********************************************************
  * @file    fsm.h
  * @author  Ageyev D.V.
  * @version V1.0
  * @date    7-aug-2013
  * @brief   This file contains all definitions for FSM.
  **********************************************************
  * @attention
  *
  **********************************************************
  */

/* Define to prevent recursive inclusion -----------------*/
#ifndef __FSM_H
#define __FSM_H

/* Includes --------------------------------------------------------------------------------*/
# include "sys_conf.h"
# include "system.h"

/* Exported types --------------------------------------------------------------------------*/


typedef enum {
    sHLAMP_OFF,					    // ���� ���������
		sHLAMP_FAR_ON,			    // ������� ���� �������
		sHLAMP_NEAR_ON			    // ������� ���� �������
} tHeadlampFSMState;

typedef enum {
		sRLamp_OFF,							// �������� ���������
		sRLamp_IDLE,						// ���������� ���������
		sRLamp_LEFT_ON,					// ������� ������� ������
		sRLamp_RIGHT_ON,				// ������� ������� �������
		sRLamp_FAILT,						// ��������� ������
		sRLamp_FAILT_LEFT_ON,		// ��������� ������ � ������� ������
		sRLamp_FAILT_RIGHT_ON,	// ��������� ������ � ������� �������
} tRLampFSMState;

typedef enum {
		sPLAMP_OFF,							// �������� ���������
		sPLAMP_ON,					    // �������� ��������
} tParklampFSMState;
						 
/* Exported constants -----------------------------------------------------------------------*/
/* Exported macro ---------------------------------------------------------------------------*/

/* ----------------------------------- �������� ����� ---------------------------------------*/
/* !!!!!!!!                           � � � � � � � �                              !!!!!!!!! */
/* !! ------------------------------------------------------------------------------------!! */
/* !!   ���������� ��������� ����� ������ ShRegHandlList � ����� main.c                   !! */
/* ------------------------------------------------------------------------------------------*/
/* FF      Format:        <array name>, <byte num>, <pin num>, <H | L>                    FF */
/* ------------------------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------------------------*/
/* PP                ���� (������� � ������� ����), ��������                              PP */
/* ------------------------------------------------------------------------------------------*/
/* -----------------------  �����  ----------------------------------------------------------*/
#define FAR_HLAMP_SWITCH  SPI_Fast_IO,  1,  0,  L      /* ������������� ��./�����.           */
#define HLAMP_KEY         SPI_Fast_IO,  1,  1,  L      /* ������ ��������� ���               */
#define PARK_LAMP_KEY 		SPI_Fast_IO,  1,  2,  L      /* ������ ��������� ���������         */
/* -----------------------  ������  ---------------------------------------------------------*/
#define FAR_HLAMP_PIN			SPI_Slow_Out, 0,  2,  H      /* ������� ����                       */
#define NEAR_HLAMP_PIN		SPI_Slow_Out, 0,  1,  H      /* ������� ����                       */
#define PARK_LAMP_PIN 		SPI_Slow_Out, 0,  0,  H      /* ��������                           */

/* ------------------------------------------------------------------------------------------*/
/* ��                 �������� � ��������� ������                                         �� */
/* ------------------------------------------------------------------------------------------*/
/* -----------------------  �����  ----------------------------------------------------------*/
#define LFT_TLAMP_KEY 		SPI_Fast_IO,  1,  4,  L      /* ������-�� � ����� �������          */
#define RTH_TLAMP_KEY			SPI_Fast_IO,  1,  5,  L      /* ������-�� � ������ �������         */
#define	FAULT_KEY					SPI_Fast_IO,  1,  6,  L      /* ������ ���������� �������          */
#define LFT_TLAMP_HC 			SPI_Fast_IO,  0,  3,  L      /* ������ �����                       */
#define RTH_TLAMP_HC			SPI_Fast_IO,  0,  2,  L      /* ������ ������                      */
/* -----------------------  ������  ---------------------------------------------------------*/
#define LFT_TLAMP_PIN 		SPI_Fast_IO,  1,  0,  H      /* ����� � �����. ���. ��������       */
#define RTH_TLAMP_PIN			SPI_Fast_IO,  1,  1,  H      /* ����� � �����. ���. ��������       */
#define FAULT_LAMP_PIN		SPI_Fast_IO,  0,  0,  H      /* ����� � ��-�� ���������� �������   */
#define RELE_TLAMP_PIN		SPI_Fast_IO,  0,  1,  H      /* ���� ��������� �������             */

/* ------------------------------------------------------------------------------------------*/
/*  PP                ���� � ������ ���                                                   �� */
/* ------------------------------------------------------------------------------------------*/
/* -----------------------  �����  ----------------------------------------------------------*/
#define REVERSE_LAMP_HC		SPI_Fast_IO,  0,  4,  L      /* ������ ������� ����                */
/* -----------------------  ������  ---------------------------------------------------------*/
#define REVERSE_LAMP_PIN	SPI_Fast_IO,  1,  3,  H      /* ����� ������� ����                 */
#define STOP_LAMP_PIN			SPI_Fast_IO,  1,  2,  H      /* ����� ���� �������                 */

/* ------------------------------------------------------------------------------------------*/
/*  PP                ��������� ������ � ����������                                       �� */
/* ------------------------------------------------------------------------------------------*/
/* -----------------------  �����  ----------------------------------------------------------*/
#define START_KEY       	SPI_Fast_IO,  0,  0,  L      /* ������ ���������                   */
/* -----------------------  ������  ---------------------------------------------------------*/
#define START_BLUE_LED  	SPI_Fast_IO,  1,  6,  H      /* �����   ��������� ���������        */
#define START_GREEN_LED		SPI_Fast_IO,  1,  7,  H      /* ������� ��������� ���������        */

/* ------------------------------------------------------------------------------------------*/
/*  PP                ��������� ������ �������                                            �� */
/* ------------------------------------------------------------------------------------------*/
/* -----------------------  �����  ----------------------------------------------------------*/
/* -----------------------  ������  ---------------------------------------------------------*/
#define BATT_RED_PIN	    SPI_Fast_IO,  1,  4,  H      /* ������� ��������� ������           */
#define BATT_GREEN_PIN		SPI_Fast_IO,  1,  5,  H      /* ������� ��������� ������           */

/*
  ********************************************************************************************* 
  *   ������   ����������   ����������   �������  
  *********************************************************************************************
*/

/* --------------------------------------------------------------------------------------------  
 *  Function:       HLamp_FSM_Handl (...) 
 * -------------------------------------------------------------------------------------------- 
 *  description:    ���������� ������� ��� ��������� �������� ���
 * 
 *  parameters:     tMsg *Msg      � �������������� ��������� 
 * 
 *  on return:      tMsgHndlResult -  ��������� ��������� (����������, ���������)
 *                                                   < MSG_PROCESSED | MSG_PASSED >  
 *----------------------------------------------------------------------------------------- */
tMsgHndlResult HLamp_FSM_Handl    (tMsg *Msg);

void           TurnLamp_FSM_Init  (void);
tMsgHndlResult TurnLamp_FSM_Handl (tMsg *Msg);

#define PARKLAMP_NO_FSM                     /* ����� ������� ���������� ����������� ��� FSM */
tMsgHndlResult ParkLamp_FSM_Handl (tMsg* Msg);


#endif /* __FSM_H */

/***    (C)COPYRIGHT 2013 ����� �.�.        ***END OF FILE ***                              */