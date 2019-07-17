

#ifndef __RTC_H
#define	__RTC_H


#include "stm32f10x.h"
#include "./rtc/bsp_calendar.h"
#include "./rtc/bsp_date.h"


//ʹ��LSE�ⲿʱ�� �� LSI�ڲ�ʱ��
#define RTC_CLOCK_SOURCE_LSE      
//#define RTC_CLOCK_SOURCE_LSI



#define RTC_BKP_DRX          BKP_DR1
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0x5050

//����ʱ���ʱ��������
#define TIME_ZOOM						(8*60*60)



void RTC_NVIC_Config(void);
void RTC_NVICAlarm_Config(void);
void RTC_Configuration(void);
void Time_Adjust(struct rtc_time *tm);
void RTC_CheckAndConfig(struct rtc_time *tm);
//void RTC_IRQHandler(void);
#endif /* __XXX_H */
