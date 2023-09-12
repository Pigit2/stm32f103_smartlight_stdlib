///////////////////////////////////////////////////////////
/*头文件包含区*/
#include "hwiot.h"
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"

///////////////////////////////////////////////////////////
/*宏定义区*/

///////////////////////////////////////////////////////////
/*外部变量声明区*/
uint8_t atok_rec_flag;
uint8_t sync_time_flag=1;//时间同步标志，1未同步，0成功同步
///////////////////////////////////////////////////////////
/*函数声明区*/

///////////////////////////////////////////////////////////
/*函数实体区*/

/**********************************************************************************************************
* 函 数 名: HuaweiIot_init
* 功能说明: 华为云物联网平台初始化
* 形 参：无
* 返 回 值: 无
**********************************************************************************************************/
void HuaweiIot_init(void)
{
    uint8_t i=0;
	
		char str_temp[256];	//虽然在实际使用时str_temp最大不超100，但是测试时发现char str_temp[128]时程序会卡死
												//经过景哥和强哥帮助，将其改为char str_temp[256]时可以正常运行，或者将其定义为全局变量
												//初步分析为作用域不同导致分配栈空间不同，具体原因待日后分析。—贾工留坑-2023.3.10.1
	
		delay_ms(1000);			//等待模块上电稳定

    for(i=0;i<10;i++)
    {
       if(atok_rec_flag==1)
				{
          AT_write("AT");					//AT测试
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"Power ON",16);//Power ON
					
          printf("AT+RST\r\n");		//重启设备
          delay_ms(1800);					//等待设备重启
          AT_write("AT");					//AT测试
          AT_write("AT+CWMODE=1");//设置模块为STA模式
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"AT+CWMODE",16);//AT+CWMODE
					
					
					sprintf(str_temp,"AT+CWJAP=\"%s\",\"%s\"",WIFI_SSID,WIFI_PWD);	//连接WiFi
          AT_write(str_temp);
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"CONNECT SSID:",16);//CONNECT SSID:
					OLED_ShowString(0,2,(u8 *)WIFI_SSID,16);
					delay_ms(1000);
					
					AT_write("AT+CIPSNTPCFG=1,8");//设置时区GMT+8
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"SET GMT+8",16);//SET GMT+8
					
					sprintf(str_temp,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"",HUAWEI_MQTT_USERNAME,HUAWEI_MQTT_PASSWORD);//设置MQTT的登陆用户名与密码
          AT_write(str_temp);
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"AT+MQTTUSERCFG",16);//AT+MQTTUSERCFG
					
					sprintf(str_temp,"AT+MQTTCLIENTID=0,\"%s\"",HUAWEI_MQTT_ClientID);//设置MQTT的ClientID
          AT_write(str_temp);
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"AT+MQTTCLIENTID",16);//AT+MQTTCLIENTID
					
					sprintf(str_temp,"AT+MQTTCONN=0,\"%s\",%s,1",HUAWEI_MQTT_ADDRESS,HUAWEI_MQTT_PORT);//设置MQTT接入地址与端口号
					AT_write(str_temp);
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"AT+MQTTCONN",16);//AT+MQTTCONN
					
					sprintf(str_temp,"AT+MQTTSUB=0,\"$oc/devices/%s/sys/properties/report\",1",HUAWEI_MQTT_DeviceID);	//订阅设备属性上报的主题
					AT_write(str_temp);
					OLED_Clear();
					OLED_ShowString(0,0,(u8 *)"AT+MQTTSUB",16);//AT+MQTTSUB
						
					sprintf(str_temp,"AT+MQTTSUB=0,\"$oc/devices/%s/sys/commands/#\",1",HUAWEI_MQTT_DeviceID);//订阅设备命令接收的主题
          AT_write(str_temp);
						
					sprintf(str_temp,"AT+MQTTSUB=0,\"$oc/devices/%s/sys/commands/response/#\",1",HUAWEI_MQTT_DeviceID);//订阅设备命令响应的主题
          AT_write(str_temp);
						
            break;
        }    
        else
        {
						if(i==9)
						{
							printf("Connected ESP01s Fail!\r\n");	//连接超时
							OLED_Clear();
							OLED_ShowString(0,0,(u8 *)"CONNECTED FAILD",16);//CONNECTED FAILD
							break;
						}
						else 
						{
							printf("AT\r\n");
							delay_ms(1000);
						}
						
        }            
    }
}
/**********************************************************************************************************
* 函 数 名: AT_write
* 功能说明: AT指令发送
* 形 参：无
* 返 回 值: 无
**********************************************************************************************************/
void AT_write(char atstring[512])//阻塞等待OK
{
		atok_rec_flag=0;
    printf("%s\r\n",atstring);
		while(1)
		{
			if(atok_rec_flag==1)	//接收到OK后串口中断会将atok_rec_flag置1
			{
				atok_rec_flag=0;
				break;
			}
			else 
				delay_ms(50);
		}	
}
/**********************************************************************************************************
* 函 数 名: HuaweiIot_publish
* 功能说明: 华为云设备属性上报
* 形 参：char * att,属性名；uint16_t data：属性值
* 返 回 值: 无
**********************************************************************************************************/
void HuaweiIot_DevDate_publish(char *att,float data)       //上报单条
{
	printf("AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\": %f}}]}\",0,0\r\n",HUAWEI_MQTT_DeviceID,HUAWEI_MQTT_ServiceID,att,data);
}

void HuaweiIot_DevDate_publish3(char *att1,float data1,char *att2,float data2,char *att3,float data3)      //上报三条
{
	printf("AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\":%f\\,\\\"%s\\\":%f\\,\\\"%s\\\":%f}}]}\",0,0\r\n",HUAWEI_MQTT_DeviceID,HUAWEI_MQTT_ServiceID,att1,data1,att2,data2,att3,data3);
}
///////////////////////////////////////////////////////////


void sync_time(void)  //时间获取同步，标志位为1则一直同步
{
	while(sync_time_flag)
	{
		printf("AT+CIPSNTPTIME?\r\n");//网络校准时间
		delay_ms(1000);
	}
}
