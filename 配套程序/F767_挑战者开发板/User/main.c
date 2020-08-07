/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   使用串口和按键控制舵机
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32F767 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_general_tim.h"
#include ".\key\bsp_key.h" 


int pulse_num=0;

__IO uint16_t ChannelPulse = 0.5/20.0*PWM_PERIOD_COUNT;
	
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}	
	
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
  /* HAL 库初始化 */
  HAL_Init();
  
	/* 初始化系统时钟为216MHz */
	SystemClock_Config();
  
	/* 初始化按键GPIO */
	Key_GPIO_Config();
  
  /* 初始化串口 */
  DEBUG_USART_Config();

  /* 通用定时器初始化并配置PWM输出功能 */
  TIMx_Configuration();
  
  printf("野火舵机控制实验\r\n");

  show_help();
	
	while(1)
	{
    /* 处理数据 */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      ChannelPulse -= 10;    // 减少占空比
      
      ChannelPulse = 0.5/20.0*PWM_PERIOD_COUNT > ChannelPulse ? 0.5/20.0*PWM_PERIOD_COUNT : ChannelPulse;    // 检查占空比的合法性
      
      set_steering_gear_dutyfactor(ChannelPulse);    // 设置占空比
    }
    
    /* 处理数据 */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      ChannelPulse += 10;    // 增加占空比
      
      ChannelPulse = (2.5/20.0*PWM_PERIOD_COUNT) < ChannelPulse ? (2.5/20.0*PWM_PERIOD_COUNT) : ChannelPulse;    // 检查占空比的合法性
      
      set_steering_gear_dutyfactor(ChannelPulse);    // 设置占空比
    }
    
    /* 串口处理 */
    deal_serial_data();
	}
}


/**
  * @brief  System Clock 配置
  *         System Clock 配置如下 : 
  *         System Clock source            = PLL (HSE)
  *         SYSCLK(Hz)                     = 216000000
  *         HCLK(Hz)                       = 216000000
  *         AHB Prescaler                  = 1
  *         APB1 Prescaler                 = 4
  *         APB2 Prescaler                 = 2
  *         HSE Frequency(Hz)              = 25000000
  *         PLL_M                          = 25
  *         PLL_N                          = 432
  *         PLL_P                          = 2
  *         PLL_Q                          = 9
  *         VDD(V)                         = 3.3
  *         Main regulator output voltage  = Scale1 mode
  *         Flash Latency(WS)              = 7
  * @param  无
  * @retval 无
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 激活 OverDrive 模式以达到216M频率  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
	/* 在HAL_RCC_ClockConfig函数里面同时初始化好了系统定时器systick，配置为1ms中断一次 */
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}

/*********************************************END OF FILE**********************/

