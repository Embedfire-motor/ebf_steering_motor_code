#ifndef __BSP_GENERAL_TIM_H
#define	__BSP_GENERAL_TIM_H

#include "stm32f1xx.h"

/*宏定义*/
#define GENERAL_TIM                        	TIM4
#define GENERAL_TIM_CLK_ENABLE()  					__HAL_RCC_TIM4_CLK_ENABLE()

//#define PWM_CHANNEL                       TIM_CHANNEL_1
//#define PWM_CHANNEL                       TIM_CHANNEL_2
#define PWM_CHANNEL                       TIM_CHANNEL_3
//#define PWM_CHANNEL                       TIM_CHANNEL_4

#define STEERING_ENGINE_CHANNEL             PWM_CHANNEL

/* 累计 TIM_Period个后产生一个更新或者中断*/		
/* 当定时器从0计数到PWM_PERIOD_COUNT，即为PWM_PERIOD_COUNT+1次，为一个定时周期 */
#define PWM_PERIOD_COUNT     999

/* 通用控制定时器时钟源TIMxCLK = HCLK=72MHz */
/* 设定定时器频率为=TIMxCLK/(PWM_PRESCALER_COUNT+1) */
#define PWM_PRESCALER_COUNT     1440

/*PWM引脚*/
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

