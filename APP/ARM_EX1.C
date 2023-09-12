
/*************************************Ӳ������*****************************************/

//0��OLED��ʾʱ�䣬��Դ��Ϣ��Զ�̶�ʱʱ��
//1������14�л�OLED��ʾ1.5�����������13�л�OLED��ʾ1.5���ѹ
//2���ƶ˿��Ƶ������������Ȼ򿪹�
//3���ƶ˿��Ƶƶ�ʱ���أ���ʱ��Ϊ�ϴιر�ʱ�����ȣ����ϴιر�ʱ����Ϊ0��Ĭ��40%
//4���ƶ˼�ص�����״̬�͵�Դ��Ϣ

/*************************************��������*****************************************/

//0����ʱʱ�串�ǹ�����ʾ����ʱ�����ָ�
//1����Դ��Ϣ5���жϻ�ȡ���ϱ�һ�Σ����ٸ��أ��ϱ�ʱ��ɰ���led��ת
//2����ʱУ׼ʱ�䣬�ж϶�ʱÿ10��������ͬ��һ��ʱ�䣨��ʹ���ڲ�ʱ�ӣ�


#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "hwiot.h"
#include "led.h"
#include "adc.h"
#include "rtc.h"
#include "oled.h"
#include "key.h"
#include "tim.h"


uint8_t uart1_rec_i=0;
extern uint8_t atok_rec_flag;
char uart1_recdata=0,uart1_recstring[256],sub_string[150],analysis_Str[256];

//uint8_t sync_time_flag=1;//ʱ��ͬ����־��1δͬ����0�ɹ�ͬ��
uint16_t light_sum=0;//�����Ƚ���ת�����ֵ����������ռ�ձ�
char u_week[5],u_month[5],u_data[5],u_hour[5],u_min[5],u_sec[5],u_year[6];//���յ�ʱ�䣬�����ַ�����
uint16_t month2,year2,day2,hour2,min2,sec2; //ת�����ʱ�䣬�����ͣ�����rtc_set()
uint16_t rec_hour,rec_min,rec_sec;//���ն�ʱ������ʱ���룬����rtc_alarm_set()


/*����ɾ*/
//extern uint16_t light_ctr; //���ն�ʱ���ƵƵĲ����������
//extern uint16_t light_int[3];//��ʱ�ص�ǰ�����ȴ���
//extern uint16_t oledinfo_flag; //��ʾ��Ϣ��־λ
//extern uint16_t ADCConvertedValue[1000][2];



void display_info()  //��ҳ��ʾ
{
	char str[9];
	
	/*-----------ʱ��-------------*/
	str[0] = calendar.hour/10+'0';
	str[1] = calendar.hour%10+'0';	str[2] = ':';
	str[3] = calendar.min/10+'0';
	str[4] = calendar.min%10+'0';str[5] = ':';
	str[6] = calendar.sec/10+'0';
	str[7] = calendar.sec%10+'0';
	str[8] = '\0';
	
	OLED_ShowNum(0,0,calendar.w_year,4,16);
	OLED_ShowCHinese(32,0,0);//��
	if(calendar.w_month<10)
	{
		OLED_ShowNum(48,0,0,1,16);
		OLED_ShowNum(56,0,calendar.w_month,1,16);
		OLED_ShowCHinese(64,0,1);//��
	}
	else
	{
		OLED_ShowNum(48,0,calendar.w_month,2,16);
		OLED_ShowCHinese(64,0,1);//��
	}
	if(calendar.w_date<10)
	{
		OLED_ShowNum(80,0,0,1,16);
		OLED_ShowNum(88,0,calendar.w_date,1,16);
		OLED_ShowCHinese(96,0,2);//��
	}
	else
	{
		OLED_ShowNum(80,0,calendar.w_date,2,16);
		OLED_ShowCHinese(96,0,2);//��
	}
	OLED_ShowString(0,2,(u8 *)str,16);//ʱ����ʾ
	OLED_ShowString(80,2,(u8 *)u_week,16);//������ʾ
	
	/*��ʾ����*/
	if(oledinfo_flag == 0)
	{
		OLED_ShowCHinese2(16,6,9,3); //���ʣ�
		OLED_Showdecimal(64,6,Power_value(),2,2,16);
		OLED_ShowChar(102,6,'W',16);
	}
}


int main(void)
{
//	unsigned int temp=0;
//	int i;
	
//	int sum;//adc temp
//	int j;//adc temp
//	float ADC_Value[2];//adc temp
//	float rl,rr,cu,pw;//adc
	
	
//	OLED_Clear();
	
	delay_init(); //systick��ʱ��ʼ��
	
	OLED_Init();  //OLED��ʼ��
	key_init();  //������ʼ��
	
	uart_init(115200);  //esp����1��ʼ��
	
	HuaweiIot_init();  //esp8266�����ʼ��
	
	
	LED_Init();  //LED��PWM��ʼ��
	ADC_int();   //ADC��ȡ��ʼ��
	


		
	RTC_Init(); //RTC��ʼ��
			
	TIM6_init(49999,7199); //TIM6��ʼ������ʱ5��
//	ADC_get_voltage();
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	TIM_SetCompare1(TIM4,999);//LED����
	
	sync_time();  //ʱ��ͬ��
//	delay_ms(1000);
//	RTC_Alarm_Set(2023,5,25,22,32,30);
	//RTC_Set(2020,5,20,17,20,20);
	
	
	
	HuaweiIot_DevDate_publish3((char*)"L1",0,(char*)"L2",0,(char*)"L3",0);  //�Ƴ�ʼ״̬�ϱ�ȫ0
	
//	HuaweiIot_DevDate_publish((char*)"L1",0);
//	delay_ms(300);
//	HuaweiIot_DevDate_publish((char*)"L2",0);
//	delay_ms(300);
//	HuaweiIot_DevDate_publish((char*)"L3",0);
	delay_ms(300);
	OLED_Clear();
	
	while(1)
	{
//		HuaweiIot_DevDate_publish((char*)"L1",41);
//			printf("AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"%s\\\"\\,\\\"properties\\\":{\\\"L1\\\": ON}}]}\",0,0\r\n",HUAWEI_MQTT_DeviceID,HUAWEI_MQTT_ServiceID);
		
		
		
		
		
//		printf("VALUE = %d V\r\n",ADC_ConvertedValue);
//		delay_ms(1500);
//		TIM_SetCompare1(TIM4,540);
//		printf("VALUE = %d V\r\n",ADC_ConvertedValue);
//		delay_ms(1500);
//		TIM_SetCompare1(TIM4,200);
//		printf("VALUE = %d V\r\n",ADC_ConvertedValue);
//		delay_ms(1500);
//		TIM_SetCompare1(TIM4,0);
//		printf("VALUE = %d V\r\n",ADC_ConvertedValue);
//		delay_ms(1500);


		
		
		
//		ADC_get_voltage();//��ȡʵʱ����Ϣ
		
		 
//		rl=ADC_Value[0]*1220/220;
//		rr=ADC_Value[1]*1220/220;
//		cu=(rl-rr)/220;
//		pw=rl*cu;
//		

		

//		HuaweiIot_DevDate_publish((char*)"voltage1",RL_value());//�ϱ���ѹ1
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"voltage2",RR_value());//�ϱ���ѹ2
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"current",Current_value());//�ϱ�����
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"power",Power_value());//�ϱ�����
//		delay_ms(500);


		
		//printf("RTC: %d��%d��%d�գ�%d:%d:%d \r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
		
		//printf("voltage: %f V ; current: %f A ; power: %f W \n",Voltage_value(),Current_value(),Power_value());
		
		//HuaweiIot_DevDate_publish3((char*)"voltage",Voltage_value(),(char*)"current",Current_value(),(char*)"power",Power_value()); //��Դ��Ϣ�ϱ�
		
//		delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"voltage",Voltage_value());
//	  delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"current",Current_value());
//	  delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"power",Power_value());
	  //delay_ms(250);
		
		//printf("res1: %f V ; res2: %f V \r\n",ADC_Value[0],ADC_Value[1]);
		//printf("value: %f V  \r\n",RR_value());
		
		display_info(); //OLED��ʾ��Ϣ
		
		//OLED_ShowChar(48,48,calendar.w_year,64);
		
		//printf("AT+CIPSNTPTIME?\r\n");//����У׼ʱ��
		
		
		
		
		
		delay_ms(1000);
		//delay_ms(1000);
		
		
	}
}



void USART1_IRQHandler(void)                	//����1�жϷ������
{
	char str_temp[128];
	char request_id[37];
	char set_temp[10];

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//�����ж�
	{
			USART_ClearITPendingBit(USART1,USART_IT_RXNE); 			//����жϱ�־
			uart1_recdata =USART_ReceiveData(USART1);						//��ȡ���յ�������
			if(uart1_recdata!='\n')
			{	
				//��ֹ��һ������Ϊ�ո�
				if((uart1_rec_i==0)&&(uart1_recdata==' '))
				{
					//���Է����ڽ���MQTT�������·�������ʱ����ʱ���յ����ݵ�һλΪ�ո�
					//ͨ�������жϷ�ʽ���Խ��������ԭ����ʱ�������ֹ�����-2022.9.3.1
				}
				else
				{
					uart1_recstring[uart1_rec_i]=uart1_recdata;
					//printf("%c",uart1_recstring[uart1_rec_i]);		//����ʱ����
					uart1_rec_i++;	//ͳ�ƽ����ַ�����
				}
			}
			else
			{
				//printf("Rec Over,uart1_rec_i=%d��data=%s\r\n",uart1_rec_i,uart1_recstring);//����ʱ���������ڼ�������������
				if(uart1_rec_i>=2)
				{
					strncpy(sub_string,uart1_recstring,2);	//��ȡǰ��λ
					if(strcmp(sub_string,"OK")==0)
					{
						atok_rec_flag=1;
						//printf("Get Ok\r\n");//����ʱ����������ȷ���Ƿ���յ�"ok"
					}
					
					strncpy(sub_string,uart1_recstring,5);
					if(strcmp(sub_string,"ready")==0)
					{ 
						atok_rec_flag=1;
						//printf("Get ready\r\n");//����ʱ����,����ȷ���Ƿ���յ���ready��
					}
					
					strncpy(sub_string,uart1_recstring,5);
					if(strcmp(sub_string,"ERROR")==0)
					{
						atok_rec_flag=0;
						//printf("Get ERROR\r\n");//����ʱ����,����ȷ���Ƿ���յ���ERROR��
					}
					
					
					
					
          if(uart1_rec_i>=10)
					{
						strncpy(analysis_Str, uart1_recstring, 12);							//��ȡ����+MQTTSUBRECV��
						
						
						//GET TIME
						if(strcmp(analysis_Str,"+CIPSNTPTIME")==0)
            {
							memset(analysis_Str,0,sizeof(analysis_Str));					//��ջ�����
							//printf("TIME�������ͷ��ȷ\r\n");
							strncpy(u_year, uart1_recstring+33, 4);    		//��ȡ��
							year2 = (u_year[0]-'0')*1000+(u_year[1]-'0')*100+(u_year[2]-'0')*10+(u_year[3]-'0');
							if(strcmp(u_year,"1970")==0)
							{
								printf("1970�ȴ�ʱ��ͬ��\n");
								OLED_Clear();
								OLED_ShowString(0,0,(u8 *)"Wait RTC...",16);
								sync_time_flag = 1;
								//delay_ms(200);
								//get_time();
								
							}
							else
							{
								printf("ʱ��ͬ���ɹ�\n");
								sync_time_flag = 0;
								strncpy(u_week, uart1_recstring+13, 3);     		//��ȡ����
								
								//printf("week=%s\r\n",u_week);
								strncpy(u_month, uart1_recstring+17, 3);    		//��ȡ�·�
								
								//printf("month=%s\r\n",u_month);
								strncpy(u_data, uart1_recstring+21, 2);    		//��ȡ����
								day2 = (u_data[0]-'0')*10+(u_data[1]-'0');
								if(u_data[0]==' ')                            //�����Ǹ�λ��ʱȥ����һλ�Ŀո�
									day2=(u_data[1]-'0');
								//printf("day2=%d\r\n",day2);
								strncpy(u_hour, uart1_recstring+24, 2);    		//��ȡʱ
								hour2 = (u_hour[0]-'0')*10+(u_hour[1]-'0');
								//printf("hour=%s\r\n",u_hour);
								strncpy(u_min, uart1_recstring+27, 2);    		//��ȡ��
								min2 = (u_min[0]-'0')*10+(u_min[1]-'0');
								//printf("min=%s\r\n",u_min);
								strncpy(u_sec, uart1_recstring+30, 2);    		//��ȡ��
								sec2 = (u_sec[0]-'0')*10+(u_sec[1]-'0');
								//printf("sec=%s\r\n",u_sec);
//							strncpy(u_year, uart1_recstring+33, 4);    		//��ȡ��
//							//printf("year=%s\r\n",u_year);
									if(strcmp(u_month,"Jan")==0)
									{
										month2=1;
									}
									if(strcmp(u_month,"Feb")==0)
									{
										month2=2;
									}
									if(strcmp(u_month,"Mar")==0)
									{
										month2=3;
									}
									if(strcmp(u_month,"Apr")==0)
									{
										month2=4;
									}
									if(strcmp(u_month,"May")==0)
									{
										month2=5;
									}
									if(strcmp(u_month,"Jun")==0)
									{
										month2=6;
									}
									if(strcmp(u_month,"Jul")==0)
									{
										month2=7;
									}
									if(strcmp(u_month,"Aug")==0)
									{
										month2=8;
									}
									if(strcmp(u_month,"Sept")==0)
									{
										month2=9;
									}
									if(strcmp(u_month,"Oct")==0)
									{
										month2=10;
									}
									if(strcmp(u_month,"Nov")==0)
									{
										month2=11;
									}
									if(strcmp(u_month,"Dec")==0)
									{
										month2=12;
									}
									RTC_Set(year2,month2,day2,hour2,min2,sec2);
									//printf("%s��%d��%s�� %s:%s:%s %s \n",u_year,month2,u_data,u_hour,u_min,u_sec,u_week);
									//printf("%d��%d��%d�� %d:%d:%d %s \n",year2,month2,day2,hour2,min2,sec2,u_week);
							}
														
							memset(analysis_Str,0,sizeof(analysis_Str));//��ջ�����  		
							memset(str_temp,0,sizeof(str_temp));				//��ջ�����  																	

						}
					
						
						
						
					//�������ݳ���Ϊ201���������JSON�ַ���Ӳ�����ķ�ʽ�������±�������Լ�ʵ�ʽ��յĲ�������
					//+MQTTSUBRECV:0,"$oc/devices/61fb2d7fde9933029funiot_esp8266_test01/sys/commands/request_id=4152fb5d-e5ae-4b89-b39d-283ba59cf033",68,{"paras":{"led":1},"service_id":"Dev_data","command_name":"Control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=8ed5858f-8813-4b95-98b2-48b11da0a5c5",70,{"paras":{"power":"1"},"service_id":"BasicData","command_name":"control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=8ed5858f-8813-4b95-98b2-48b11da0a5c5",70,{"paras":{"power":"00%","switch":"ALL"},"service_id":"BasicData","command_name":"control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=ae359c49-31df-4086-b8ab-cffec685b237",86,{"paras":{"power":"ON","set":"160600"},"service_id":"BasicData","command_name":"time"}
						//GET MQTT REQUEST
						if(strcmp(analysis_Str,"+MQTTSUBRECV")==0)
            {
							memset(analysis_Str,0,sizeof(analysis_Str));					//��ջ�����
							printf("MQTT�������ͷ��ȷ\r\n");
							strncpy(request_id, uart1_recstring+86, 36);     			//��ȡ��request_id
							printf("request_id=%s\r\n",request_id);
							strncpy(analysis_Str, uart1_recstring+129, 5);    		//��ȡ��"paras"
							//printf("paras=%s\r\n",analysis_Str);
							if(strcmp(analysis_Str,"paras")==0)        						//��Ч������
							{
									memset(analysis_Str,0,sizeof(analysis_Str));	//��ջ�����
									strncpy(analysis_Str, uart1_recstring+138,5);	//��ȡ��"power"
									//printf("att is %s\r\n",analysis_Str);
									if(strcmp(analysis_Str,"power")==0)
									{
										memset(analysis_Str,0,sizeof(analysis_Str));	//��ջ�����
											//printf("led set %c\r\n",uart1_recstring[149]);
													
										
										//printf("set: %s\r\n",analysis_Str);
										
											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='N')
											{
												light_ctr = 1;  //���յ���ʱ���ź�
												rec_hour = (uart1_recstring[157]-'0')*10+(uart1_recstring[158]-'0');
												rec_min  = (uart1_recstring[159]-'0')*10+(uart1_recstring[160]-'0');
												rec_sec  = (uart1_recstring[161]-'0')*10+(uart1_recstring[162]-'0');
												RTC_Alarm_Set(year2,month2,day2,rec_hour,rec_min,rec_sec);
												
												set_temp[0] = rec_hour/10+'0';
												set_temp[1] = rec_hour%10+'0'; set_temp[2] = ':';
												set_temp[3] = rec_min/10+'0';
												set_temp[4] = rec_min%10+'0';  set_temp[5] = ':';
												set_temp[6] = rec_sec/10+'0';
												set_temp[7] = rec_sec%10+'0';
												set_temp[8] = '\0';
												
												oledinfo_flag = 1; //��־Ϊ����ʾ����
												OLED_Clear();  //��ʾ��ʱǰ�ѹ�����ʾ���
												//OLED_ShowString(16,6,(u8 *)"            ",16);
												OLED_ShowString(0,6,(u8 *)"SET ",24);
//												OLED_ShowNum(32,6,calendar.w_year,4,24);OLED_ShowString(40,6,(u8 *)"/",24);
//												OLED_ShowNum(48,6,calendar.w_month,2,24);OLED_ShowString(56,6,(u8 *)"/",24);
//												OLED_ShowNum(64,6,calendar.w_date,2,24);
												OLED_ShowString(32,6,(u8 *)set_temp,24);//��ʱʱ����ʾ
											}
											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='F' && uart1_recstring[148]=='F')
											{
												light_ctr = 0;  //���յ���ʱ�ر��ź�
												rec_hour = (uart1_recstring[158]-'0')*10+(uart1_recstring[159]-'0');
												rec_min  = (uart1_recstring[160]-'0')*10+(uart1_recstring[161]-'0');
												rec_sec  = (uart1_recstring[162]-'0')*10+(uart1_recstring[163]-'0');
												RTC_Alarm_Set(year2,month2,day2,rec_hour,rec_min,rec_sec);
												
												set_temp[0] = rec_hour/10+'0';
												set_temp[1] = rec_hour%10+'0'; set_temp[2] = ':';
												set_temp[3] = rec_min/10+'0';
												set_temp[4] = rec_min%10+'0';  set_temp[5] = ':';
												set_temp[6] = rec_sec/10+'0';
												set_temp[7] = rec_sec%10+'0';
												set_temp[8] = '\0';
	
												oledinfo_flag = 1;  //��־Ϊ����ʾ����
												OLED_Clear();  //��ʾ��ʱǰ�ѹ�����ʾ���
												//OLED_ShowString(16,6,(u8 *)"            ",16);
												OLED_ShowString(0,6,(u8 *)"SET ",24);
//												OLED_ShowNum(32,3,calendar.w_year,4,24);OLED_ShowString(40,3,(u8 *)"/",24);
//												OLED_ShowNum(48,3,calendar.w_month,2,24);OLED_ShowString(56,3,(u8 *)"/",24);
//												OLED_ShowNum(64,3,calendar.w_date,2,24);
												OLED_ShowString(32,6,(u8 *)set_temp,24);//��ʱʱ����ʾ
											}
										
										
										
//										if(strcmp(analysis_Str,"set")==0)             //��ʱ����ת��ʱ������
//										{
//											rec_hour = (uart1_recstring[157]-'0')*10+(uart1_recstring[158]-'0');
//											rec_min  = (uart1_recstring[159]-'0')*10+(uart1_recstring[160]-'0');
//											rec_sec  = (uart1_recstring[161]-'0')*10+(uart1_recstring[162]-'0');
//											
//											RTC_Alarm_Set(year2,month2,day2,rec_hour,rec_min,rec_sec);
//											//printf("%d:%d:%d \r\n",rec_hour,rec_min,rec_sec);
//											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='N')
//												light_ctr = 1;
//											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='F' && uart1_recstring[148]=='F')
//												light_ctr = 0;
//										}
										
										
											if(uart1_recstring[161]=='A' && uart1_recstring[162]=='L' && uart1_recstring[163]=='L')      //����ȫ����
											{
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[0] = light_sum;
												light_int[1] = light_sum;
												light_int[2] = light_sum;
												//printf("ls: %d ",light_sum);
												printf("�ص�\r\n");
												TIM_SetCompare1(TIM4,light_sum);//LED1
												TIM_SetCompare2(TIM4,light_sum);//LED2
												TIM_SetCompare4(TIM4,light_sum);//LED3
												delay_ms(300);
												HuaweiIot_DevDate_publish3((char*)"L1",light_sum/10,(char*)"L2",light_sum/10,(char*)"L3",light_sum/10); //��״̬�ϱ���ȫ0
											}
											
											if(uart1_recstring[161]=='L' && uart1_recstring[162]=='1')            //��������L1
											{
													
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[0] = light_sum;
												TIM_SetCompare1(TIM4,light_sum);//LED1 
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L1",light_sum/10); //��L1״̬�ϱ�
											}
											if(uart1_recstring[161]=='L' && uart1_recstring[162]=='2')            //��������L2
											{
													
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[1] = light_sum;
												TIM_SetCompare2(TIM4,light_sum);//LED1 
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L2",light_sum/10);  //��L2״̬�ϱ�
											}
											else if(uart1_recstring[161]=='L' && uart1_recstring[162]=='3')       //��������L3
											{
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[2] = light_sum;
												TIM_SetCompare4(TIM4,light_sum);//LED1
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L3",light_sum/10);  //��L3״̬�ϱ�
											}
											//����ƽ̨���������Ӧ
											printf("AT+MQTTPUB=0,\"$oc/devices/%s/sys/commands/response/request_id=%s\",\"\",0,0\r\n",HUAWEI_MQTT_DeviceID,request_id);
															
											memset(analysis_Str,0,sizeof(analysis_Str));//��ջ�����  		
											memset(str_temp,0,sizeof(str_temp));				//��ջ�����  																	
								 }        
							}
						}
						
					}
				}
				uart1_rec_i=0;
				memset(uart1_recstring,0,sizeof(uart1_recstring));//���uart1_recstring�ַ���
				memset(sub_string,0,sizeof(sub_string));					//���sub_string�ַ���				
			}				
	 } 	
}