#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"


/* 私有类型定义 --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;       //设定目标 Desired Value
  __IO float    SumError;      //误差累计
  __IO float    Proportion;     //比例常数 Proportional Const
  __IO float    Integral;       //积分常数 Integral Const
  __IO float    Derivative;     //微分常数 Derivative Const
  __IO int      LastError;      //Error[-1]
  __IO int      PrevError;      //Error[-2]
}PID_TypeDef;


/*************************************/
// 定义PID相关宏
// 这三个参数设定对电机运行影响非常大
// PID参数跟采样时间息息相关
/*************************************/
#define  CUR_P_DATA      0.35f   // P参数
#define  CUR_I_DATA      0.6f    // I参数
#define  CUR_D_DATA      0.0f    // D参数
#define  TARGET_CURRENT  300     // 最大电流值 300mA

#define  SPD_P_DATA      2.5f    // P参数
#define  SPD_I_DATA      0.5f    // I参数
#define  SPD_D_DATA      0.0f    // D参数
#define  TARGET_SPEED    10.0f   // 目标速度    10r/m
#define  MAX_SPEED       380  // 空载满速380r/m

extern PID_TypeDef  cPID,sPID;                     // PID参数结构体

void PID_ParamInit(void);
int32_t CurPIDCalc(int32_t NextPoint);
int32_t SpdPIDCalc(float NextPoint);

#endif
