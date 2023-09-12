
/*************************************硬件功能*****************************************/

//0：OLED显示时间，电源信息或远程定时时间
//1：按键14切换OLED显示1.5秒电流，按键13切换OLED显示1.5秒电压
//2：云端控制单个或多个灯亮度或开关
//3：云端控制灯定时开关，定时开为上次关闭时的亮度，如上次关闭时亮度为0则默认40%
//4：云端监控灯亮度状态和电源信息

/*************************************软件功能*****************************************/

//0：定时时间覆盖功率显示，定时结束恢复
//1：电源信息5秒中断获取和上报一次，减少负载，上报时造成板载led反转
//2：定时校准时间，中断定时每10分钟联网同步一次时间（因使用内部时钟）


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

//uint8_t sync_time_flag=1;//时间同步标志，1未同步，0成功同步
uint16_t light_sum=0;//灯亮度接收转换后的值，用于设置占空比
char u_week[5],u_month[5],u_data[5],u_hour[5],u_min[5],u_sec[5],u_year[6];//接收的时间，数组字符类型
uint16_t month2,year2,day2,hour2,min2,sec2; //转换后的时间，短整型，用于rtc_set()
uint16_t rec_hour,rec_min,rec_sec;//接收定时的设置时分秒，用于rtc_alarm_set()


/*可以删*/
//extern uint16_t light_ctr; //接收定时控制灯的操作，开或关
//extern uint16_t light_int[3];//定时关灯前灯亮度储存
//extern uint16_t oledinfo_flag; //显示信息标志位
//extern uint16_t ADCConvertedValue[1000][2];



void display_info()  //首页显示
{
	char str[9];
	
	/*-----------时间-------------*/
	str[0] = calendar.hour/10+'0';
	str[1] = calendar.hour%10+'0';	str[2] = ':';
	str[3] = calendar.min/10+'0';
	str[4] = calendar.min%10+'0';str[5] = ':';
	str[6] = calendar.sec/10+'0';
	str[7] = calendar.sec%10+'0';
	str[8] = '\0';
	
	OLED_ShowNum(0,0,calendar.w_year,4,16);
	OLED_ShowCHinese(32,0,0);//年
	if(calendar.w_month<10)
	{
		OLED_ShowNum(48,0,0,1,16);
		OLED_ShowNum(56,0,calendar.w_month,1,16);
		OLED_ShowCHinese(64,0,1);//月
	}
	else
	{
		OLED_ShowNum(48,0,calendar.w_month,2,16);
		OLED_ShowCHinese(64,0,1);//月
	}
	if(calendar.w_date<10)
	{
		OLED_ShowNum(80,0,0,1,16);
		OLED_ShowNum(88,0,calendar.w_date,1,16);
		OLED_ShowCHinese(96,0,2);//日
	}
	else
	{
		OLED_ShowNum(80,0,calendar.w_date,2,16);
		OLED_ShowCHinese(96,0,2);//日
	}
	OLED_ShowString(0,2,(u8 *)str,16);//时间显示
	OLED_ShowString(80,2,(u8 *)u_week,16);//星期显示
	
	/*显示功率*/
	if(oledinfo_flag == 0)
	{
		OLED_ShowCHinese2(16,6,9,3); //功率：
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
	
	delay_init(); //systick延时初始化
	
	OLED_Init();  //OLED初始化
	key_init();  //按键初始化
	
	uart_init(115200);  //esp串口1初始化
	
	HuaweiIot_init();  //esp8266网络初始化
	
	
	LED_Init();  //LED的PWM初始化
	ADC_int();   //ADC读取初始化
	


		
	RTC_Init(); //RTC初始化
			
	TIM6_init(49999,7199); //TIM6初始化，定时5秒
//	ADC_get_voltage();
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	TIM_SetCompare1(TIM4,999);//LED最亮
	
	sync_time();  //时间同步
//	delay_ms(1000);
//	RTC_Alarm_Set(2023,5,25,22,32,30);
	//RTC_Set(2020,5,20,17,20,20);
	
	
	
	HuaweiIot_DevDate_publish3((char*)"L1",0,(char*)"L2",0,(char*)"L3",0);  //灯初始状态上报全0
	
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


		
		
		
//		ADC_get_voltage();//获取实时电信息
		
		 
//		rl=ADC_Value[0]*1220/220;
//		rr=ADC_Value[1]*1220/220;
//		cu=(rl-rr)/220;
//		pw=rl*cu;
//		

		

//		HuaweiIot_DevDate_publish((char*)"voltage1",RL_value());//上报电压1
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"voltage2",RR_value());//上报电压2
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"current",Current_value());//上报电流
//		delay_ms(500);
//		HuaweiIot_DevDate_publish((char*)"power",Power_value());//上报功率
//		delay_ms(500);


		
		//printf("RTC: %d年%d月%d日，%d:%d:%d \r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
		
		//printf("voltage: %f V ; current: %f A ; power: %f W \n",Voltage_value(),Current_value(),Power_value());
		
		//HuaweiIot_DevDate_publish3((char*)"voltage",Voltage_value(),(char*)"current",Current_value(),(char*)"power",Power_value()); //电源信息上报
		
//		delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"voltage",Voltage_value());
//	  delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"current",Current_value());
//	  delay_ms(250);
//		HuaweiIot_DevDate_publish((char*)"power",Power_value());
	  //delay_ms(250);
		
		//printf("res1: %f V ; res2: %f V \r\n",ADC_Value[0],ADC_Value[1]);
		//printf("value: %f V  \r\n",RR_value());
		
		display_info(); //OLED显示信息
		
		//OLED_ShowChar(48,48,calendar.w_year,64);
		
		//printf("AT+CIPSNTPTIME?\r\n");//网络校准时间
		
		
		
		
		
		delay_ms(1000);
		//delay_ms(1000);
		
		
	}
}



void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	char str_temp[128];
	char request_id[37];
	char set_temp[10];

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//接收中断
	{
			USART_ClearITPendingBit(USART1,USART_IT_RXNE); 			//清除中断标志
			uart1_recdata =USART_ReceiveData(USART1);						//读取接收到的数据
			if(uart1_recdata!='\n')
			{	
				//防止第一个接收为空格
				if((uart1_rec_i==0)&&(uart1_recdata==' '))
				{
					//测试发现在接收MQTT服务器下发的命令时，有时接收的数据第一位为空格
					//通过上述判断方式可以解决，触发原因暂时不明，贾工留坑-2022.9.3.1
				}
				else
				{
					uart1_recstring[uart1_rec_i]=uart1_recdata;
					//printf("%c",uart1_recstring[uart1_rec_i]);		//调试时开启
					uart1_rec_i++;	//统计接收字符数量
				}
			}
			else
			{
				//printf("Rec Over,uart1_rec_i=%d，data=%s\r\n",uart1_rec_i,uart1_recstring);//调试时开启，用于检查接收数据内容
				if(uart1_rec_i>=2)
				{
					strncpy(sub_string,uart1_recstring,2);	//截取前两位
					if(strcmp(sub_string,"OK")==0)
					{
						atok_rec_flag=1;
						//printf("Get Ok\r\n");//调试时开启，用于确定是否接收到"ok"
					}
					
					strncpy(sub_string,uart1_recstring,5);
					if(strcmp(sub_string,"ready")==0)
					{ 
						atok_rec_flag=1;
						//printf("Get ready\r\n");//调试时开启,用于确定是否接收到“ready”
					}
					
					strncpy(sub_string,uart1_recstring,5);
					if(strcmp(sub_string,"ERROR")==0)
					{
						atok_rec_flag=0;
						//printf("Get ERROR\r\n");//调试时开启,用于确定是否接收到“ERROR”
					}
					
					
					
					
          if(uart1_rec_i>=10)
					{
						strncpy(analysis_Str, uart1_recstring, 12);							//提取出“+MQTTSUBRECV”
						
						
						//GET TIME
						if(strcmp(analysis_Str,"+CIPSNTPTIME")==0)
            {
							memset(analysis_Str,0,sizeof(analysis_Str));					//清空缓存区
							//printf("TIME命令接收头正确\r\n");
							strncpy(u_year, uart1_recstring+33, 4);    		//提取年
							year2 = (u_year[0]-'0')*1000+(u_year[1]-'0')*100+(u_year[2]-'0')*10+(u_year[3]-'0');
							if(strcmp(u_year,"1970")==0)
							{
								printf("1970等待时间同步\n");
								OLED_Clear();
								OLED_ShowString(0,0,(u8 *)"Wait RTC...",16);
								sync_time_flag = 1;
								//delay_ms(200);
								//get_time();
								
							}
							else
							{
								printf("时间同步成功\n");
								sync_time_flag = 0;
								strncpy(u_week, uart1_recstring+13, 3);     		//提取星期
								
								//printf("week=%s\r\n",u_week);
								strncpy(u_month, uart1_recstring+17, 3);    		//提取月份
								
								//printf("month=%s\r\n",u_month);
								strncpy(u_data, uart1_recstring+21, 2);    		//提取日期
								day2 = (u_data[0]-'0')*10+(u_data[1]-'0');
								if(u_data[0]==' ')                            //日期是个位数时去除第一位的空格
									day2=(u_data[1]-'0');
								//printf("day2=%d\r\n",day2);
								strncpy(u_hour, uart1_recstring+24, 2);    		//提取时
								hour2 = (u_hour[0]-'0')*10+(u_hour[1]-'0');
								//printf("hour=%s\r\n",u_hour);
								strncpy(u_min, uart1_recstring+27, 2);    		//提取分
								min2 = (u_min[0]-'0')*10+(u_min[1]-'0');
								//printf("min=%s\r\n",u_min);
								strncpy(u_sec, uart1_recstring+30, 2);    		//提取秒
								sec2 = (u_sec[0]-'0')*10+(u_sec[1]-'0');
								//printf("sec=%s\r\n",u_sec);
//							strncpy(u_year, uart1_recstring+33, 4);    		//提取年
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
									//printf("%s年%d月%s日 %s:%s:%s %s \n",u_year,month2,u_data,u_hour,u_min,u_sec,u_week);
									//printf("%d年%d月%d日 %d:%d:%d %s \n",year2,month2,day2,hour2,min2,sec2,u_week);
							}
														
							memset(analysis_Str,0,sizeof(analysis_Str));//清空缓存区  		
							memset(str_temp,0,sizeof(str_temp));				//清空缓存区  																	

						}
					
						
						
						
					//接收数据长度为201，下面采用JSON字符串硬解析的方式，具体下标请根据自己实际接收的参数处理
					//+MQTTSUBRECV:0,"$oc/devices/61fb2d7fde9933029funiot_esp8266_test01/sys/commands/request_id=4152fb5d-e5ae-4b89-b39d-283ba59cf033",68,{"paras":{"led":1},"service_id":"Dev_data","command_name":"Control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=8ed5858f-8813-4b95-98b2-48b11da0a5c5",70,{"paras":{"power":"1"},"service_id":"BasicData","command_name":"control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=8ed5858f-8813-4b95-98b2-48b11da0a5c5",70,{"paras":{"power":"00%","switch":"ALL"},"service_id":"BasicData","command_name":"control"}
					//+MQTTSUBRECV:0,"$oc/devices/645b7a79eb2ee73fc4d21149_20230510/sys/commands/request_id=ae359c49-31df-4086-b8ab-cffec685b237",86,{"paras":{"power":"ON","set":"160600"},"service_id":"BasicData","command_name":"time"}
						//GET MQTT REQUEST
						if(strcmp(analysis_Str,"+MQTTSUBRECV")==0)
            {
							memset(analysis_Str,0,sizeof(analysis_Str));					//清空缓存区
							printf("MQTT命令接收头正确\r\n");
							strncpy(request_id, uart1_recstring+86, 36);     			//提取出request_id
							printf("request_id=%s\r\n",request_id);
							strncpy(analysis_Str, uart1_recstring+129, 5);    		//提取出"paras"
							//printf("paras=%s\r\n",analysis_Str);
							if(strcmp(analysis_Str,"paras")==0)        						//有效参数体
							{
									memset(analysis_Str,0,sizeof(analysis_Str));	//清空缓存区
									strncpy(analysis_Str, uart1_recstring+138,5);	//提取出"power"
									//printf("att is %s\r\n",analysis_Str);
									if(strcmp(analysis_Str,"power")==0)
									{
										memset(analysis_Str,0,sizeof(analysis_Str));	//清空缓存区
											//printf("led set %c\r\n",uart1_recstring[149]);
													
										
										//printf("set: %s\r\n",analysis_Str);
										
											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='N')
											{
												light_ctr = 1;  //接收到定时打开信号
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
												
												oledinfo_flag = 1; //标志为不显示功耗
												OLED_Clear();  //显示定时前把功率显示清楚
												//OLED_ShowString(16,6,(u8 *)"            ",16);
												OLED_ShowString(0,6,(u8 *)"SET ",24);
//												OLED_ShowNum(32,6,calendar.w_year,4,24);OLED_ShowString(40,6,(u8 *)"/",24);
//												OLED_ShowNum(48,6,calendar.w_month,2,24);OLED_ShowString(56,6,(u8 *)"/",24);
//												OLED_ShowNum(64,6,calendar.w_date,2,24);
												OLED_ShowString(32,6,(u8 *)set_temp,24);//定时时间显示
											}
											if(uart1_recstring[146]=='O' && uart1_recstring[147]=='F' && uart1_recstring[148]=='F')
											{
												light_ctr = 0;  //接收到定时关闭信号
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
	
												oledinfo_flag = 1;  //标志为不显示功耗
												OLED_Clear();  //显示定时前把功率显示清楚
												//OLED_ShowString(16,6,(u8 *)"            ",16);
												OLED_ShowString(0,6,(u8 *)"SET ",24);
//												OLED_ShowNum(32,3,calendar.w_year,4,24);OLED_ShowString(40,3,(u8 *)"/",24);
//												OLED_ShowNum(48,3,calendar.w_month,2,24);OLED_ShowString(56,3,(u8 *)"/",24);
//												OLED_ShowNum(64,3,calendar.w_date,2,24);
												OLED_ShowString(32,6,(u8 *)set_temp,24);//定时时间显示
											}
										
										
										
//										if(strcmp(analysis_Str,"set")==0)             //定时接收转换时间设置
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
										
										
											if(uart1_recstring[161]=='A' && uart1_recstring[162]=='L' && uart1_recstring[163]=='L')      //控制全部灯
											{
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[0] = light_sum;
												light_int[1] = light_sum;
												light_int[2] = light_sum;
												//printf("ls: %d ",light_sum);
												printf("关灯\r\n");
												TIM_SetCompare1(TIM4,light_sum);//LED1
												TIM_SetCompare2(TIM4,light_sum);//LED2
												TIM_SetCompare4(TIM4,light_sum);//LED3
												delay_ms(300);
												HuaweiIot_DevDate_publish3((char*)"L1",light_sum/10,(char*)"L2",light_sum/10,(char*)"L3",light_sum/10); //灯状态上报，全0
											}
											
											if(uart1_recstring[161]=='L' && uart1_recstring[162]=='1')            //单独控制L1
											{
													
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[0] = light_sum;
												TIM_SetCompare1(TIM4,light_sum);//LED1 
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L1",light_sum/10); //灯L1状态上报
											}
											if(uart1_recstring[161]=='L' && uart1_recstring[162]=='2')            //单独控制L2
											{
													
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[1] = light_sum;
												TIM_SetCompare2(TIM4,light_sum);//LED1 
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L2",light_sum/10);  //灯L2状态上报
											}
											else if(uart1_recstring[161]=='L' && uart1_recstring[162]=='3')       //单独控制L3
											{
												light_sum = (uart1_recstring[146]-'0')*100+(uart1_recstring[147]-'0')*10;
												light_int[2] = light_sum;
												TIM_SetCompare4(TIM4,light_sum);//LED1
												delay_ms(300);
												HuaweiIot_DevDate_publish((char*)"L3",light_sum/10);  //灯L3状态上报
											}
											//向云平台完成命令响应
											printf("AT+MQTTPUB=0,\"$oc/devices/%s/sys/commands/response/request_id=%s\",\"\",0,0\r\n",HUAWEI_MQTT_DeviceID,request_id);
															
											memset(analysis_Str,0,sizeof(analysis_Str));//清空缓存区  		
											memset(str_temp,0,sizeof(str_temp));				//清空缓存区  																	
								 }        
							}
						}
						
					}
				}
				uart1_rec_i=0;
				memset(uart1_recstring,0,sizeof(uart1_recstring));//清空uart1_recstring字符串
				memset(sub_string,0,sizeof(sub_string));					//清空sub_string字符串				
			}				
	 } 	
}
