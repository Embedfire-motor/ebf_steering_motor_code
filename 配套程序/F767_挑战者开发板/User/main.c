/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ʹ�ô��ںͰ������ƶ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F767 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
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
	
void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}	
	
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
  /* HAL ���ʼ�� */
  HAL_Init();
  
	/* ��ʼ��ϵͳʱ��Ϊ216MHz */
	SystemClock_Config();
  
	/* ��ʼ������GPIO */
	Key_GPIO_Config();
  
  /* ��ʼ������ */
  DEBUG_USART_Config();

  /* ͨ�ö�ʱ����ʼ��������PWM������� */
  TIMx_Configuration();
  
  printf("Ұ��������ʵ��\r\n");

  show_help();
	
	while(1)
	{
    /* �������� */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      ChannelPulse -= 10;    // ����ռ�ձ�
      
      ChannelPulse = 0.5/20.0*PWM_PERIOD_COUNT > ChannelPulse ? 0.5/20.0*PWM_PERIOD_COUNT : ChannelPulse;    // ���ռ�ձȵĺϷ���
      
      set_steering_gear_dutyfactor(ChannelPulse);    // ����ռ�ձ�
    }
    
    /* �������� */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      ChannelPulse += 10;    // ����ռ�ձ�
      
      ChannelPulse = (2.5/20.0*PWM_PERIOD_COUNT) < ChannelPulse ? (2.5/20.0*PWM_PERIOD_COUNT) : ChannelPulse;    // ���ռ�ձȵĺϷ���
      
      set_steering_gear_dutyfactor(ChannelPulse);    // ����ռ�ձ�
    }
    
    /* ���ڴ��� */
    deal_serial_data();
	}
}


/**
  * @brief  System Clock ����
  *         System Clock �������� : 
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
  * @param  ��
  * @retval ��
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
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
  
  /* ���� OverDrive ģʽ�Դﵽ216MƵ��  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
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
  
	/* ��HAL_RCC_ClockConfig��������ͬʱ��ʼ������ϵͳ��ʱ��systick������Ϊ1ms�ж�һ�� */
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}

/*********************************************END OF FILE**********************/

