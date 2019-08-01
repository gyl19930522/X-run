#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_rtc.h"
#include "bsp_usart.h"
#include "bsp_iwdg.h"
#include "bsp_chipid.h"
#include "stm32f10x_iwdg.h"
#include "eeprom.h"
#include "functional.h"
#include "communication.h"

const uint32_t device_id = 0x2A16E4E8;
const uint16_t sw_version = 0x101;

//��״̬(0δ�󶨣�1��)
uint8_t binding_flag = 0;
//APP ID
uint32_t app_id;
//E��״̬(0Ϊ���ϣ�1����)
uint8_t eeprom_status = 1;
//��·�����Ϣ
Step walking = {0.3f, 0, 0};
//�ܲ������Ϣ
Step running = {0.5f, 0, 0};
//FFT������
uint16_t fillcounter = 0;
//����ѹ��
float hanging = 2.61f;
//ѹ��ֵ
float pressure = 0;
//���ʵ��SOC
volatile float battsoc = 0;
//���״̬(0δ��磬1���) ���Ƿ���RTC�ж����ж�
volatile uint8_t charging_flag = 0;
//�Ʋ���־λ(0��ʾδ����Ʋ���1��ʾ�Ʋ��У�2��ʾ���ر궨��3��ʾ���ñ궨
volatile uint8_t step_flag = 0;
//ͨѶ����״̬
volatile uint8_t interactstatus = 0;
//��ص�ѹ
volatile float battvolt = 0;

//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 8, 1, 2019, 0
};

//������E������
static void Power_Supply(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_1;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitStructure_1.GPIO_Pin = GPIO_Pin_1;	
	GPIO_InitStructure_1.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure_1.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure_1);
	//�򿪹���
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	//�жϳ��״̬
}

static void Initialization(void) {
	//�򿪹���
	Power_Supply();
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
		if (pressure > HANG_RATIO * hanging && charging_flag == 0) {
			RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
			RTC_WaitForLastTask();
			GPIO_SetBits(GPIOA, GPIO_Pin_1); //�رչ���
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
	LEDDELAY;
}

//RTC�ж�
void RTC_IRQHandler(void)
{
	//1s�жϣ���ɼ���SOC�������״̬
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
		FUNC_ChargeOrNot();
		FUNC_BattSOC_Caculation();
	}
}

//���ڽ����ж�
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		COM_Listening();
	}
}

int main(void)
{
	Initialization();
	//��ѭ��10msһ��
	while (FUNC_SleepOrNot())
	{
		FUNC_Pressure_Filter();
		FUNC_Step_CountOrCalibrate();
		COM_Listen_Reset();
		COM_Response();
		IWDG_Feed();
	}
	EEP_Sleep_Write();
	RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
	RTC_WaitForLastTask();
	GPIO_ResetBits(GPIOA, GPIO_Pin_1); //�رչ���
	IWDG_Config(IWDG_Prescaler_256, 4095);
	IWDG_Feed();
	//�������ģʽ
	PWR_EnterSTANDBYMode();
}

/*********************************************END OF FILE**********************/
