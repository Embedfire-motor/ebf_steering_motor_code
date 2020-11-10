#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_GeneralTim.h"  
#include "./key/bsp_key.h"  
#include "bsp_usart.h"

__IO uint16_t ChannelPulse = 0.5/20.0*GENERAL_TIM_Period;

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	/* led �˿����� */ 
	LED_GPIO_Config();
	
  /* ������ʼ�� */
  Key_GPIO_Config();
  
  /* ���ڳ�ʼ�� */
  USART_Config();
  
	/* ��ʱ����ʼ�� */
	GENERAL_TIM_Init();
	
  printf("Ұ��������ʵ��\r\n");

  show_help();
  
	while(1)
	{  
    /* �������� */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      ChannelPulse -= 10;    // ����ռ�ձ�
      
      ChannelPulse = 0.5/20.0*GENERAL_TIM_Period > ChannelPulse ? 0.5/20.0*GENERAL_TIM_Period : ChannelPulse;    // ���ռ�ձȵĺϷ���
      
      set_steering_gear_dutyfactor(ChannelPulse);    // ����ռ�ձ�
    }
    
    /* �������� */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      ChannelPulse += 10;    // ����ռ�ձ�
      
      ChannelPulse = (2.5/20.0*GENERAL_TIM_Period) < ChannelPulse ? (2.5/20.0*GENERAL_TIM_Period) : ChannelPulse;    // ���ռ�ձȵĺϷ���
      
      set_steering_gear_dutyfactor(ChannelPulse);    // ����ռ�ձ�
    }
    
    /* ���ڴ��� */
    deal_serial_data();
	}
}
/*********************************************END OF FILE**********************/
