#include "tim.h"
#include "hwiot.h"
#include "adc.h"
#include "usart.h"
#include "delay.h"

uint16_t time_flag = 0;  //计时标志为，一个5s

void TIM6_init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
  GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_14);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=arr;  //Tout=(arr+1)*(psc+1)/72
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
}


void TIM6_IRQHandler(void)
{
		
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		GPIO_WriteBit(GPIOC, GPIO_Pin_14,(BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_14))));
		ADC_get_voltage();         //获取实时电信息
		HuaweiIot_DevDate_publish3((char*)"voltage",Voltage_value(),(char*)"current",Current_value(),(char*)"power",Power_value());   //电源信息上报
		
		time_flag ++;
		//printf("tf= %d \n",time_flag);
		if(time_flag == 120) //10分钟时同步一次时间，10*60/5
		{
			time_flag = 0;
			delay_ms(500);
			printf("AT+CIPSNTPTIME?\r\n");//网络校准时间
		}
	}
}

