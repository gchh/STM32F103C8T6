#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"

/* �������� */
//��������x��������Ϊint32_t
#define ROUND_TO_INT32(x)   ((int32_t)(x)+0.5f)>=(x)? ((int32_t)(x)):((uint32_t)(x)+1) 


/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;       //�趨Ŀ�� Desired Value
  __IO float    SumError;      //����ۼ�
  __IO float    Proportion;     //�������� Proportional Const
  __IO float    Integral;       //���ֳ��� Integral Const
  __IO float    Derivative;     //΢�ֳ��� Derivative Const
  __IO int      LastError;      //Error[-1]
  __IO int      PrevError;      //Error[-2]
}PID_TypeDef;


#define SPD_PID //�ٶȻ�
//#define SPD_CUR_PID //�ٶ�+������
/*************************************/
// ����PID��غ�
// �����������趨�Ե������Ӱ��ǳ���
// PID����������ʱ��ϢϢ���
/*************************************/
#ifdef SPD_CUR_PID
#define  CUR_P_DATA      0.35f   // P����
#define  CUR_I_DATA      0.6f    // I����
#define  CUR_D_DATA      0.0f    // D����
#define  TARGET_CURRENT  300     // ������ֵ 300mA

#define  SPD_P_DATA      0.3f    // P����
#define  SPD_I_DATA      0.08f    // I����
#define  SPD_D_DATA      0.0f    // D����
#define  TARGET_SPEED    100.0f//10.0f   // Ŀ���ٶ�    10r/m
#define  MAX_SPEED       380  // ��������380r/m
#endif

#ifdef SPD_PID
//��Ӧϵͳʱ��8MHz
#define  SPD_P_DATA      0.3f    // P����
#define  SPD_I_DATA      0.08f    // I����
#define  SPD_D_DATA      0.0f    // D����
#define  TARGET_SPEED    100.0f//10.0f   // Ŀ���ٶ�    10r/m
#define  MAX_SPEED       380  // ��������380r/m
#endif


extern PID_TypeDef  cPID,sPID;                     // PID�����ṹ��

void PID_ParamInit(void);
int32_t CurPIDCalc(int32_t NextPoint);
int32_t SpdPIDCalc(float NextPoint);

#endif
