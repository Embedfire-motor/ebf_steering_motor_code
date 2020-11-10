#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_GeneralTim.h"  
#include "./key/bsp_key.h"  
#include "bsp_usart.h"

__IO uint16_t ChannelPulse = 0.5/20.0*GENERAL_TIM_Period;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	/* led 端口配置 */ 
	LED_GPIO_Config();
	
  /* 按键初始化 */
  Key_GPIO_Config();
  
  /* 串口初始化 */
  USART_Config();
  
	/* 定时器初始化 */
	GENERAL_TIM_Init();
	
  printf("野火舵机控制实验\r\n");

  show_help();
  
	while(1)
	{  
    /* 处理数据 */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      ChannelPulse -= 10;    // 减少占空比
      
      ChannelPulse = 0.5/20.0*GENERAL_TIM_Period > ChannelPulse ? 0.5/20.0*GENERAL_TIM_Period : ChannelPulse;    // 检查占空比的合法性
      
      set_steering_gear_dutyfactor(ChannelPulse);    // 设置占空比
    }
    
    /* 处理数据 */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      ChannelPulse += 10;    // 增加占空比
      
      ChannelPulse = (2.5/20.0*GENERAL_TIM_Period) < ChannelPulse ? (2.5/20.0*GENERAL_TIM_Period) : ChannelPulse;    // 检查占空比的合法性
      
      set_steering_gear_dutyfactor(ChannelPulse);    // 设置占空比
    }
    
    /* 串口处理 */
    deal_serial_data();
	}
}
/*********************************************END OF FILE**********************/
