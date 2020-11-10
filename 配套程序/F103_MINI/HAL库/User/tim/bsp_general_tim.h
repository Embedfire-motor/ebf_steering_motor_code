#ifndef __BSP_GENERAL_TIM_H
#define	__BSP_GENERAL_TIM_H

#include "stm32f1xx.h"

/*�궨��*/
#define GENERAL_TIM                        	TIM4
#define GENERAL_TIM_CLK_ENABLE()  					__HAL_RCC_TIM4_CLK_ENABLE()

//#define PWM_CHANNEL                       TIM_CHANNEL_1
//#define PWM_CHANNEL                       TIM_CHANNEL_2
#define PWM_CHANNEL                       TIM_CHANNEL_3
//#define PWM_CHANNEL                       TIM_CHANNEL_4

#define STEERING_ENGINE_CHANNEL             PWM_CHANNEL

/* �ۼ� TIM_Period�������һ�����»����ж�*/		
/* ����ʱ����0������PWM_PERIOD_COUNT����ΪPWM_PERIOD_COUNT+1�Σ�Ϊһ����ʱ���� */
#define PWM_PERIOD_COUNT     999

/* ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=72MHz */
/* �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(PWM_PRESCALER_COUNT+1) */
#define PWM_PRESCALER_COUNT     1440

/*PWM����*/
//#define GENERAL_TIM1_CH_GPIO_PORT           GPIOB
//#define GENERAL_TIM1_CH_PIN                 GPIO_PIN_6

//#define GENERAL_TIM2_CH_GPIO_PORT           GPIOB
//#define GENERAL_TIM2_CH_PIN                 GPIO_PIN_7

#define GENERAL_TIM_CH3_GPIO_PORT           GPIOB
#define GENERAL_TIM_CH3_PIN                 GPIO_PIN_8

//#define GENERAL_TIM_CH4_GPIO_PORT           GPIOB
//#define GENERAL_TIM_CH4_PIN                 GPIO_PIN_9

#define STEERING_ENGINE_PORT          GENERAL_TIM_CH3_GPIO_PORT
#define STEERING_ENGINE_PIN           GENERAL_TIM_CH3_PIN

extern TIM_HandleTypeDef  TIM_TimeBaseStructure;

extern void TIMx_Configuration(void);
extern void TIM_SetPWM_pulse(uint32_t channel,int compare);
void set_steering_gear_dutyfactor(uint16_t dutyfactor);
void set_steering_gear_angle(uint16_t angle);
void show_help(void);
void deal_serial_data(void);

#endif

