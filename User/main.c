#include "bsp_adc.h"
#include "bsp_rtc.h"
#include "bsp_iwdg.h"
#include "eeprom.h"
#include "functional.h"
#include "communication.h"

//������
const uint32_t activation_code = 0x2A16E4E8;
const uint16_t sw_version = 0x2710;

//��״̬(0δ�󶨣�1��)
uint8_t binding_flag = 0;
//E��״̬(0Ϊ���ϣ�1����)
uint8_t eeprom_status = 1;
//��·�����Ϣ
Step walking = {0.3f, 0.3f, 0};
//�ܲ������Ϣ
Step running = {0.5f, 0.5f, 0};
//FFT������
uint16_t fillcounter = 0;
//����ѹ��
float hanging_a = 2.61f;
//����ѹ��2
float hanging_b = 2.61f;
//ѹ��ֵ
float pressure_a = 0;
//ѹ��ֵ
float pressure_b = 0;
//�Ʋ���־(0��ʾδ����Ʋ���1��ʾ�Ʋ��У�2��ʾ���ر궨��3��ʾ���ñ궨)
uint8_t step_flag = 0;
//���״̬(0δ��磬1���) ���Ƿ���RTC�ж����ж�
volatile uint8_t charging_flag = 0;
//���ڻظ���־
volatile uint8_t response_flag = 0;
//���ʵ��SOC
volatile float battsoc = 0;
//��ص�ѹ
volatile float battvolt = 0;

//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 8, 8, 2019, 0
};

static void Initialization(void) {
	//adc��ʼ��
	ADCx_Init();
	//��ȡE����ʼֵ
	EEP_Initial_Read();
	//��ʼ���˲���ѹ�������ʼSOC
	FUNC_Functional_Initial();
	//������
	FUNC_ChargeOrNot();
	//�Ƿ��ǴӴ���ģʽ���˳���
	if(PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) {
		PWR_ClearFlag(PWR_FLAG_SB);
		//����Դ��ھ���״̬����������
		if (pressure_a > HANG_RATIO * hanging_a && pressure_b > HANG_RATIO * hanging_b && charging_flag == 0) {
			RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
			RTC_WaitForLastTask();
			IWDG_Feed();
			IWDG_Config(IWDG_Prescaler_256, 4095);
			PWR_EnterSTANDBYMode();
		}
	}
	//LED��ʼ��
	LED_GPIO_Config();
	//USART��ʼ����
	USART_Config();
	//���������Ʋ�
	if (binding_flag == 1) {
		step_flag = 1;
	}
	//���Ź���ʼ��
	IWDG_Init();
}

//RTC�ж�
void RTC_IRQHandler(void)
{
	//1s�жϣ���ɼ���SOC�������״̬
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
		FUNC_Led_Breath();
		FUNC_ChargeOrNot();
		FUNC_BattSOC_Caculation();
	}
}

//���ڽ����ж�
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		COM_Listen();
	}
}

int main(void) {
	Initialization();
	//��ѭ��10msһ��
	while (FUNC_SleepOrNot())
	{
		COM_Response();
		COM_Listen_Reset();
		FUNC_Pressure_Filter();
		FUNC_Step_CountOrCalibrate();
		IWDG_Feed();
	}
	EEP_Sleep_Write();
	RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
	RTC_WaitForLastTask();
	IWDG_Config(IWDG_Prescaler_256, 4095);
	IWDG_Feed();
	//�������ģʽ
	PWR_EnterSTANDBYMode();
	return 0;
}

/*********************************************END OF FILE**********************/
