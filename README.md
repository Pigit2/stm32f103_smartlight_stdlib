stm32f103rct6_smartlight_huaweiiot

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
