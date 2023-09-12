#include "led.h"



void LED_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TimOCInitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_7|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
  TIM_DeInit(TIM4);
  TIM_InternalClockConfig(TIM4);
	TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 719;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	
  TIM_OCStructInit(&TimOCInitStructure);
	
  TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TimOCInitStructure.TIM_Pulse = 0;
  TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC4Init(TIM4, &TimOCInitStructure);

  TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TimOCInitStructure.TIM_Pulse = 0;
  TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC2Init(TIM4, &TimOCInitStructure);
	
	TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TimOCInitStructure.TIM_Pulse = 0;
  TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM4, &TimOCInitStructure);

  TIM_CtrlPWMOutputs(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}
