#include "pid.h"

/* PID结构体 */
PID_TypeDef  cPID,sPID;                     // PID参数结构体

/******************** PID 控制设计 ***************************/
/**
  * 函数功能: PID参数初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void PID_ParamInit(void) 
{
#ifdef SPD_CUR_PID
    cPID.LastError = 0;            // Error[-1]
    cPID.PrevError = 0;            // Error[-2]
    cPID.SumError = 0;
    cPID.Proportion = CUR_P_DATA;  // 比例常数 Proportional Const
    cPID.Integral = CUR_I_DATA;    // 积分常数  Integral Const
    cPID.Derivative = CUR_D_DATA;  // 微分常数 Derivative Const
    cPID.SetPoint = TARGET_CURRENT;// 设定目标Desired Value
#endif
    sPID.LastError = 0;               // Error[-1]
    sPID.PrevError = 0;               // Error[-2]
    sPID.SumError = 0;
    sPID.Proportion = SPD_P_DATA; // 比例常数 Proportional Const
    sPID.Integral = SPD_I_DATA;   // 积分常数  Integral Const
    sPID.Derivative = SPD_D_DATA; // 微分常数 Derivative Const
    sPID.SetPoint = TARGET_SPEED;     // 设定目标Desired Value
}


/** 
  * 函数名称：电流闭环PID控制设计
  * 输入参数：当前控制量
  * 返 回 值：目标控制量
  * 说    明：无
  */
#ifdef SPD_CUR_PID
int32_t CurPIDCalc(int32_t NextPoint)
{
    int32_t iError,dError;
    iError = cPID.SetPoint - NextPoint; //偏差
    /* 设定闭环死区 */
    if((iError >= -3) && (iError <= 3))
        iError = 0;
  
    cPID.SumError += iError; //积分
//    if( cPID.SumError >= 5200)
//    {
//        cPID.SumError = 5200;
//    }
//    if( cPID.SumError <= -5200)
//    {
//        cPID.SumError = -5200;
//    }    
    
    dError = iError - cPID.LastError; //微分
    cPID.LastError = iError;
  
    return (int32_t)(cPID.Proportion * (float)iError //比例项
        + cPID.Integral * (float)cPID.SumError //积分项
        + cPID.Derivative * (float)dError);    //微分项
}
#endif

/** 
  * 函数名称：速度闭环PID控制设计
  * 输入参数：当前控制量
  * 返 回 值：目标控制量
  * 说    明：无
  */
int32_t SpdPIDCalc(float NextPoint)
{
    float iError,dError;
    iError = sPID.SetPoint - NextPoint; //偏差
#ifdef SPD_CUR_PID  
    if((iError<0.2f )&& (iError>-0.2f))
        iError = 0.0f;
#else   
    /* 小于0.5r/m的速度误差是正常的 */
    if((iError<0.5f )&& (iError>-0.5f))
        iError = 0.0f;
#endif  
    sPID.SumError += iError; //积分
//  /* 设定积分上限 */
#ifdef SPD_CUR_PID
    if(sPID.SumError >= (TARGET_CURRENT*10.0f))
        sPID.SumError  = (TARGET_CURRENT*10.0f);
    if(sPID.SumError <= -(TARGET_CURRENT*10.0f))
        sPID.SumError = -(TARGET_CURRENT*10.0f);
#endif  
    dError = iError - sPID.LastError; //微分
    sPID.LastError = iError;
    
//  return (int32_t)(sPID.Proportion * iError //比例项
//  + sPID.Integral * (float)sPID.SumError //积分项
//  + sPID.Derivative * dError); //微分项
    return (ROUND_TO_INT32(sPID.Proportion * iError //比例项
            + sPID.Integral * (float)sPID.SumError //积分项
            + sPID.Derivative * dError)); //微分项  
}




