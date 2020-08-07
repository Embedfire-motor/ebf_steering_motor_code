#ifndef __BSP_GENERAL_TIM_H
#define	__BSP_GENERAL_TIM_H

#include "stm32h7xx.h"

/*�궨��*/
#define GENERAL_TIM                        	TIM2
#define GENERAL_TIM_GPIO_AF                 GPIO_AF1_TIM2
#define GENERAL_TIM_CLK_ENABLE()  					__TIM2_CLK_ENABLE()

#define PWM_CHANNEL_1                       TIM_CHANNEL_1
//#define PWM_CHANNEL_2                       TIM_CHANNEL_2
//#define PWM_CHANNEL_3                       TIM_CHANNEL_3
//#define PWM_CHANNEL_4                       TIM_CHANNEL_4

/* �ۼ� TIM_Period�������һ�����»����ж�*/		
/* ����ʱ����0������PWM_PERIOD_COUNT����ΪPWM_PERIOD_COUNT+1�Σ�Ϊһ����ʱ���� */
#define PWM_PERIOD_COUNT     999

/* ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=240MHz */
/* �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(PWM_PRESCALER_COUNT+1) */
#define PWM_PRESCALER_COUNT     4799

/*PWM����*/
#define GENERAL_TIM_CH1_GPIO_PORT           GPIOA
#define GENERAL_TIM_CH1_PIN                 GPIO_PIN_15

//#define GENERAL_TIM_CH2_GPIO_PORT           GPIOB
//#define GENERAL_TIM_CH2_PIN                 GPIO_PIN_3

//#define GENERAL_TIM_CH3_GPIO_PORT           GPIOB

//#define GENERAL_TIM_CH3_PIN                 GPIO_PIN_10

//#define GENERAL_TIM_CH4_GPIO_PORT           GPIOB
//#define GENERAL_TIM_CH4_PIN                 GPIO_PIN_11

extern TIM_HandleTypeDef  TIM_TimeBaseStructure;

extern void TIMx_Configuration(void);
extern void TIM2_SetPWM_pulse(uint32_t channel,int compare);
void set_steering_gear_dutyfactor(uint16_t dutyfactor);
void set_steering_gear_angle(uint16_t angle);
void show_help(void);
void deal_serial_data(void);

#endif

