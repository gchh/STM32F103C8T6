#include "pid.h"

/* PID�ṹ�� */
PID_TypeDef  cPID,sPID;                     // PID�����ṹ��

/******************** PID ������� ***************************/
/**
  * ��������: PID������ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void PID_ParamInit(void) 
{
    cPID.LastError = 0;            // Error[-1]
    cPID.PrevError = 0;            // Error[-2]
    cPID.Proportion = CUR_P_DATA;  // �������� Proportional Const
    cPID.Integral = CUR_I_DATA;    // ���ֳ���  Integral Const
    cPID.Derivative = CUR_D_DATA;  // ΢�ֳ��� Derivative Const
    cPID.SetPoint = TARGET_CURRENT;// �趨Ŀ��Desired Value

    sPID.LastError = 0;               // Error[-1]
    sPID.PrevError = 0;               // Error[-2]
    sPID.Proportion = SPD_P_DATA; // �������� Proportional Const
    sPID.Integral = SPD_I_DATA;   // ���ֳ���  Integral Const
    sPID.Derivative = SPD_D_DATA; // ΢�ֳ��� Derivative Const
    sPID.SetPoint = TARGET_SPEED;     // �趨Ŀ��Desired Value
}


/** 
  * �������ƣ������ջ�PID�������
  * �����������ǰ������
  * �� �� ֵ��Ŀ�������
  * ˵    ������
  */
int32_t CurPIDCalc(int32_t NextPoint)
{
  int32_t iError,dError;
  iError = cPID.SetPoint - NextPoint; //ƫ��
  /* �趨�ջ����� */
  if((iError >= -3) && (iError <= 3))
    iError = 0;
  
  cPID.SumError += iError; //����
  dError = iError - cPID.LastError; //΢��
  cPID.LastError = iError;
  
  return (int32_t)(cPID.Proportion * (float)iError //������
  + cPID.Integral * (float)cPID.SumError //������
  + cPID.Derivative * (float)dError);    //΢����
}

/** 
  * �������ƣ��ٶȱջ�PID�������
  * �����������ǰ������
  * �� �� ֵ��Ŀ�������
  * ˵    ������
  */
int32_t SpdPIDCalc(float NextPoint)
{
  float iError,dError;
  iError = sPID.SetPoint - NextPoint; //ƫ��
  
  if((iError<0.3f )&& (iError>-0.3f))
    iError = 0.0f;
  
  sPID.SumError += iError; //����
  /* �趨�������� */
  if(sPID.SumError >= (TARGET_CURRENT*10.0f))
     sPID.SumError  = (TARGET_CURRENT*10.0f);
  if(sPID.SumError <= -(TARGET_CURRENT*10.0f))
    sPID.SumError = -(TARGET_CURRENT*10.0f);
  
  dError = iError - sPID.LastError; //΢��
  sPID.LastError = iError;
  return (int32_t)(sPID.Proportion * iError //������
  + sPID.Integral * (float)sPID.SumError //������
  + sPID.Derivative * dError); //΢����
}




