#ifndef __HUAWEIIOT_H
#define __HUAWEIIOT_H

///////////////////////////////////////////////////////////
/*头文件包含区*/
#include "sys.h"
///////////////////////////////////////////////////////////
/*宏定义区*/
//使用时请在这里补全相关信息
#define WIFI_SSID									"Library"
#define WIFI_PWD									"123456788"
//#define HUAWEI_MQTT_USERNAME			"645b7a79eb2ee73fc4d21149_20230510"
#define HUAWEI_MQTT_USERNAME			"64830c9901554a59339fdf41_20230609"
//#define HUAWEI_MQTT_PASSWORD			"f493d4ee82e57e3b915535b4ac6b37cd632c75a413090b16e3cd4a10342136de"
#define HUAWEI_MQTT_PASSWORD			"fd31f9dc508eec63a824cb50922d4667291d35ad853b7e44b5ec73509575fa8d"
//#define HUAWEI_MQTT_ClientID			"645b7a79eb2ee73fc4d21149_20230510_0_0_2023051102"
#define HUAWEI_MQTT_ClientID			"64830c9901554a59339fdf41_20230609_0_0_2023060912"
//#define HUAWEI_MQTT_ADDRESS				"746ad467ac.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define HUAWEI_MQTT_ADDRESS				"38702c0e79.iot-mqtts.cn-north-4.myhuaweicloud.com"
#define HUAWEI_MQTT_PORT					"1883"
//#define HUAWEI_MQTT_DeviceID			"645b7a79eb2ee73fc4d21149_20230510"
#define HUAWEI_MQTT_DeviceID			"64830c9901554a59339fdf41_20230609"
#define HUAWEI_MQTT_ServiceID			"BasicData"
///////////////////////////////////////////////////////////
/*外部变量声明区*/
extern uint8_t sync_time_flag;//时间同步标志，1未同步，0成功同步
///////////////////////////////////////////////////////////
/*函数声明区*/
void AT_write(char atstring[512]);//阻塞等待OK
void HuaweiIot_init(void);
void HuaweiIot_DevDate_publish(char * att,float data);
void HuaweiIot_DevDate_publish3(char *att1,float data1,char *att2,float data2,char *att3,float data3);
void sync_time(void);
///////////////////////////////////////////////////////////
#endif
