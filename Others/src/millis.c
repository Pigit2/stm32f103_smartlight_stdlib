#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"
#include "millis.h"

void MILLIS_Init(void)
{
#ifdef USE_LSE
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //?????????????

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);     //????????

    /* Reset Backup Domain */

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)   //???????
    {
        PWR_ClearFlag(PWR_FLAG_WU); // ??????
    } else
    {
        BKP_DeInit();    //??????
    }

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC ALARM */
    RTC_ITConfig(RTC_IT_ALR, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();


    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#else
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //??????
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);





    //???????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    //??RTC????
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //???????17?????
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //PWR_WakeUpPinCmd(DISABLE);

    //??????????
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    //?????????
    PWR_BackupAccessCmd(ENABLE);


    /* Reset Backup Domain */

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)   //???????
    {
        PWR_ClearFlag(PWR_FLAG_WU); // ??????
    } else
    {
        BKP_DeInit();    //??????
    }

    //BKP_ClearFlag();
    //    BKP_DeInit();
    //??LSI
    RCC_LSICmd(ENABLE);
    //??????
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {}
    //?????????
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);
    //??RTC_CTL?????RSF?(???????)????1
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    //??????
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();
    //????
    RTC_SetPrescaler(32); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32+1) */
    RTC_WaitForLastTask();
    //?????
    RTC_SetCounter(0);
    RTC_WaitForLastTask();
#endif
}

void RTCAlarm_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALR);
    }

    return;
}
void RTC_IRQHandler()
{
    if(RTC_GetITStatus(RTC_IT_ALR) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line17);   //???????17?????
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_WaitForLastTask();
    }
    else if(RTC_GetITStatus(RTC_IT_SEC) == SET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
    }
}
unsigned long millis()//???????????????(ms)
{
    return RTC_CURRENT_VAL;
}
