#include "key.h"
#include "delay.h"
#include "oled.h"
#include "rtc.h"
#include "adc.h"


void key_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
  GPIO_Init(GPIOB,&GPIO_InitStructure);
	
  //EXIT
	EXTI_ClearITPendingBit(EXTI_Line13);
  EXTI_ClearITPendingBit(EXTI_Line14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13 | EXTI_Line14; 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                                          
  EXTI_Init(&EXTI_InitStructure);
	
	//NVIC
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) != RESET) //PB13按键中断显示电压
	{
		delay_ms(100); //按键消抖
		oledinfo_flag = 1;  //标志为不显示功耗
		OLED_ShowString(0,6,(u8 *)"              ",16); //显示电压电流前清除功率
		/*显示电压*/
		OLED_ShowCHinese2(16,6,3,3); //电压：
		OLED_Showdecimal(64,6,Voltage_value(),2,2,16);
		OLED_ShowChar(102,6,'V',16);
		
		delay_ms(1500);
		oledinfo_flag = 0;  //恢复标志为显示功耗
		OLED_Clear();
	}
	
	if (EXTI_GetITStatus(EXTI_Line14) != RESET)  //PB14按键中断显示电流
	{
		delay_ms(200);  //按键消抖
		oledinfo_flag = 1;  //标志为不显示功耗
		OLED_ShowString(0,6,(u8 *)"              ",16);  //显示电压电流前清除功率
		/*显示电流*/
		OLED_ShowCHinese2(16,6,6,3); //电流：
		OLED_Showdecimal(64,6,Current_value(),2,2,16);
		OLED_ShowChar(102,6,'A',16);
		
		delay_ms(1500);
		oledinfo_flag = 0;  //恢复标志为显示功耗
		OLED_Clear();
	}
	
  EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line14);
}

