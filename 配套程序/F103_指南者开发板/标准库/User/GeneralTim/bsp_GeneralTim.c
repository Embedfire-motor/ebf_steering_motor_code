
#include "bsp_GeneralTim.h" 
#include <math.h>
#include <stdlib.h>
#include "bsp_usart.h"

static void GENERAL_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// 输出比较通道3 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);

}


///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void GENERAL_TIM_Mode_Config(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/	
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出通道电平极性配置	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// 输出比较通道 3
	TIM_OCInitStructure.TIM_Pulse = 0.5/20.0*GENERAL_TIM_Period;	// 占空比配置
	TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	
	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config();		
}

/**
  * @brief  设置舵机占空比
  * @param  angle: 占空比，（0.5/20.0*PWM_PERIOD_COUNT 到 2.5/20.0*PWM_PERIOD_COUNT）
  * @retval 无
  */
void set_steering_gear_dutyfactor(uint16_t dutyfactor)
{
  #if 1
  {
    /* 对超过范围的占空比进行边界处理 */
    dutyfactor = 0.5/20.0*GENERAL_TIM_Period > dutyfactor ? 0.5/20.0*GENERAL_TIM_Period : dutyfactor;
    dutyfactor = 2.5/20.0*GENERAL_TIM_Period < dutyfactor ? 2.5/20.0*GENERAL_TIM_Period : dutyfactor;
  }
  #endif
  
  TIM_SetCompare3(TIM4, dutyfactor);
}

/**
  * @brief  设置舵机角度
  * @param  angle: 角度，（0 到 180（舵机为0°-180°））
  * @retval 无
  */
void set_steering_gear_angle(uint16_t angle_temp)
{
  angle_temp = (0.5 + angle_temp / 180.0 * (2.5 - 0.5)) / 20.0 * GENERAL_TIM_Period;    // 计算角度对应的占空比
  
  set_steering_gear_dutyfactor(angle_temp);    // 设置占空比
}

/**
  * @brief  打印帮助命令
  * @param  无
  * @retval 无
  */
void show_help(void)
{
    printf("——————————————野火舵机驱动演示程序——————————————\n\r");
    printf("输入命令(<>内的内容，以回车结束)：\n\r");
    printf("<?>       -帮助菜单\n\r");
    printf("<a+角度>  -设置舵机的角度（范围：%d—%d），使用空格代替+号\n\r", 0, 180);
}


extern uint16_t ChannelPulse;

/**
  * @brief  处理串口接收到的数据
  * @param  无
  * @retval 无
  */
void deal_serial_data(void)
{
    int angle_temp=0;
    
    //接收到正确的指令才为1
    char okCmd = 0;

    //检查是否接收到指令
    if(receive_cmd == 1)
    {
      if(UART_RxBuffer[0] == 'a' || UART_RxBuffer[0] == 'A')
      {
        //设置速度
        if(UART_RxBuffer[1] == ' ')
        {
          angle_temp = atoi((char const *)UART_RxBuffer+2);
          if(angle_temp>=0 && angle_temp <= 180)
          {
            printf("\n\r角度: %d\n\r", angle_temp);
            ChannelPulse = (0.5 + angle_temp / 180.0 * (2.5 - 0.5)) / 20.0 * GENERAL_TIM_Period;    // 更新按钮控制的占空比
            set_steering_gear_angle(angle_temp);
//            printf("\n\r角度: %d\n\r", (uint16_t)(angle_temp/PWM_PERIOD_COUNT*20.0/(2.5-0.5)*180.0));
            okCmd = 1;
          }
        }
      }
      else if(UART_RxBuffer[0] == '?')
      {
        //打印帮助命令
        show_help();
        okCmd = 1;
      }
      //如果指令有无则打印帮助命令
      if(okCmd != 1)
      {
        printf("\n\r 输入有误，请重新输入...\n\r");
        show_help();
      }

      //清空串口接收缓冲数组
      receive_cmd = 0;
      uart_FlushRxBuffer();

    }
}

/*********************************************END OF FILE**********************/
