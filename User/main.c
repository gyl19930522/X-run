#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_rtc.h"
#include "bsp_iwdg.h"
#include "bsp_chipid.h"
#include "stm32f10x_iwdg.h"
#include "eeprom.h"
#include "functional.h"
#include <inttypes.h>
#include <math.h>

extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
extern uint32_t ChipUniqueID[3];

extern void Crypto_CalcKey(uint32_t wSeed, uint32_t *keyset);
extern void Crypto_Random(uint32_t *randseedset);

//��״̬��0x1AΪ�󶨳ɹ�
uint8_t binding_flag = 0x1A;
//APP ID
uint32_t app_id[3];
//E��״̬��0������1����
uint8_t eeprom_status = 0;
//��ص�ѹ
float battvolt = 0;
//ѹ�������˲�ֵ
float pressure = 0;
//���SOC(%)
uint8_t battsoc = 0;
//��·�����Ϣ
volatile Step walking = {2.1, 0, 0};
//�ܲ������Ϣ
volatile Step running = {1.6, 0, 0};
//����
volatile float weight = 2.4;
//����ѹ��
volatile float hanging = 2.65;
//���ü���
uint16_t standingcounter = 0;
//�Ʋ�״̬(0Ϊ�Ʋ���1Ϊ�ر�)
volatile uint8_t stepflag = 0;

struct rtc_time clocktime = {
	8, 0, 0, 7, 1, 2019, 0
};
//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 7, 1, 2019, 0
};

//RTC�жϴ���
void RTC_IRQHandler(void)
{
	//2s�ж�
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
		//����жϴ�������
		FUNC_battSOC_caculation();
	}
	//�����ж�
	if (RTC_GetITStatus(RTC_IT_ALR) != RESET) 
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);
		RTC_WaitForLastTask();
	}
}

//����͹���ģʽ�ж�
uint8_t SleepOrNot(void) {
	//SOC = 0ֱ������
	if (battsoc < 1) {
		//return 0;
	}
	//����״̬ά��1min������
	if (pressure > hanging && stepflag < 1) {
		standingcounter++;
		if (standingcounter > 6000) {
			return 0;
		}
	}
	else {
		standingcounter = 0;
	}
	return 1;
}

void Initialization(void) {
  //led��ʼ��
	LED_GPIO_Config();
	LED1_ON;
	LED2_ON;
	//adc��ʼ��
	ADCx_Init();
	//оƬID
	Get_ChipID();
	//E����ʼ����ȡ
	EEP_initial_read();
	//�鿴��״̬
	if (binding_flag != 0x1A) {
		// ............
		// ............
		// ............
	}
	//�����빦�ܳ�ʼ��
	FUNC_functional_initial();
	//���Ź���ʼ��
	IWDG_Init();
	IWDG_Config(IWDG_Prescaler_16, 125);//���Ź����ʱ��50ms
	LED1_OFF;
	LED2_OFF;
}

int main(void)
{	
	Initialization();
	//��ѭ��10msһ��
	while (SleepOrNot())
	{
		FUNC_step_counter();
		IWDG_Feed();
	}
	EEP_sleep_write();
	RTC_SetAlarm(RTC_GetCounter() + 420); //420s����
	RTC_WaitForLastTask();
	IWDG_Config(IWDG_Prescaler_256 ,65535); //���Ź����ʱ������Ϊ���
	IWDG_Feed();
	//����ʱ��˸һ��
	LED1_ON;
	LED2_ON;
	LEDDELAY;
	LED1_OFF;
	LED2_OFF;
	LEDDELAY;
	LED1_ON;
	LED2_ON;
	LEDDELAY;
	//����͹���ģʽ
	PWR_EnterSTANDBYMode();
}

/*********************************************END OF FILE**********************/
