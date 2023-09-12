#include "adc.h"


float ADC_Value[2];
volatile uint16_t ADCConvertedValue[1000][2];//[10][3]用来存放ADC转换结果，也是DMA的目标地址,3通道，每通道采集10次后面取平均数
																					//A[0][0] A[0][1]
																					//A[1][0] A[1][1]
																					//A[2][0] A[2][1]
																					//A[3][0] A[3][1]
																					//   ...    ...
																					//   CH1    CH2

void ADC_int(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	ADC_InitTypeDef ADC_InitStruct;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC1的分频因子 72/6=12M<14M
	
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AIN;//PA1模拟输入
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AIN;//PA1模拟输入
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//AD转化模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据对齐模式：右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//触发启动模式
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//ADC工作模式:独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 2;//顺序进行规则转换的ADC 通道的数目1
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//AD 单通道模式 
	ADC_Init(ADC1,&ADC_InitStruct);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);//通道一转换结果保存到ADCConvertedValue[0~10][0]
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_239Cycles5);//通道二转换结果保存到ADCConvertedValue[0~10][1]
	
	ADC_DMACmd(ADC1, ENABLE);//开启DMA支持
	ADC_Cmd(ADC1,ENABLE);//使能ADC1
	
	ADC_ResetCalibration(ADC1);//ADC使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC使能复位校准结束
	
	ADC_StartCalibration(ADC1);//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));//等待ADC校准结束


	DMA_DeInit(DMA1_Channel1);    //将通道一寄存器设为默认值
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);//该参数用以定义DMA外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;//该参数用以定义DMA内存基地址(转换结果保存的地址)
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//该参数规定了外设是作为数据传输的目的地还是来源，此处是作为来源
  DMA_InitStructure.DMA_BufferSize = 2*1000;//定义指定DMA通道的DMA缓存的大小,单位为数据单位。这里也就是ADCConvertedValue的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设定外设地址寄存器递增与否,此处设为不变 Disable
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//用来设定内存地址寄存器递增与否,此处设为递增，Enable
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道拥有高优先级 分别4个等级 低、中、高、非常高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//使能DMA通道的内存到内存传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);//根据DMA_InitStruct中指定的参数初始化DMA的通道

  DMA_Cmd(DMA1_Channel1, ENABLE);//启动DMA通道一
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//开始采集
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//  
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
//  NVIC_Init( & NVIC_InitStructure);
}

void ADC_get_voltage(void)
{
	
	float sum;
	uint16_t i,j;
	
	for(i=0;i<2;i++)
	{
		sum=0;
		for(j=0;j<1000;j++)
		{
			sum+=ADCConvertedValue[j][i];

		}
		ADC_Value[i]=(float)sum/(1000*4096)*3.3;//求平均值并转换成电压值	
		
	}
	
}


float Voltage_value(void)
{
	float temp = ADC_Value[1]*1220/220;
	if(temp < 0)
		return 0;
	else
		return temp;
}


float Current_value(void)
{
	float temp = (ADC_Value[0]-2.58)/0.185;
	if(temp < 0)
		return 0;
	else
		return temp;
}


float Power_value(void)
{
	return Voltage_value() * Current_value();
}


