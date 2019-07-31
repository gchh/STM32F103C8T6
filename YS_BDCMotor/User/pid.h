#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"


/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;       //�趨Ŀ�� Desired Value
  __IO float     SumError;      //����ۼ�
  __IO float    Proportion;     //�������� Proportional Const
  __IO float    Integral;       //���ֳ��� Integral Const
  __IO float    Derivative;     //΢�ֳ��� Derivative Const
  __IO int      LastError;      //Error[-1]
  __IO int      PrevError;      //Error[-2]
}PID_TypeDef;


/*************************************/
// ����PID��غ�
// �����������趨�Ե������Ӱ��ǳ���
// PID����������ʱ��ϢϢ���
/*************************************/
#define  CUR_P_DATA      0.35f   // P����
#define  CUR_I_DATA      0.6f    // I����
#define  CUR_D_DATA      0.0f    // D����
#define  TARGET_CURRENT  300     // ������ֵ 300mA

#define  SPD_P_DATA      2.5f    // P����
#define  SPD_I_DATA      0.5f    // I����
#define  SPD_D_DATA      0.0f    // D����
#define  TARGET_SPEED    10.0f   // Ŀ���ٶ�    10r/m

extern PID_TypeDef  cPID,sPID;                     // PID�����ṹ��

void PID_ParamInit(void);
int32_t CurPIDCalc(int32_t NextPoint);
int32_t SpdPIDCalc(float NextPoint);

#endif
