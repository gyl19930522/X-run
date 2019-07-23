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
const uint8_t sw_version = 100;

//��״̬(0δ�󶨣�1��)
uint8_t binding_flag = 0;
//APP ID
uint32_t app_id;
//E��״̬(0Ϊ���ϣ�1����)
uint8_t eeprom_status = 1;
//���״̬(0δ��磬1���) ���Ƿ���RTC�ж����ж�
volatile uint8_t charging_flag = 1;
//��·�����Ϣ
Step walking = {1.8, 0, 0};
//�ܲ������Ϣ
Step running = {1.5, 0, 0};
//����ѹ��
float weight = 2.1;
//����ѹ��
float hanging = 2.55;
//�Ʋ���־λ
volatile uint8_t step_flag = 0;
//ͨѶ����״̬
volatile uint8_t interactstatus = 0;

//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 8, 1, 2019, 0
};

//���ʵ��SOC
float actualsoc = 0;
//���SOC
uint8_t displaysoc = 0;
//��ص�ѹ
volatile float battvolt = 0;
//ѹ��ֵ
float pressure = 0;

//������E������
static void Powersupply_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//�򿪹���
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

static void Initialization(void) {
	//�򿪹���
	Powersupply_Init();
	//adc��ʼ��
	ADCx_Init();
	//��ȡE����ʼֵ
	EEP_Initial_Read();
	//��ʼ���˲���ѹ�������ʼSOC
	FUNC_Functional_Initial();
	//�Ƿ��ǴӴ���ģʽ���˳���
	if(PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) {
		PWR_ClearFlag(PWR_FLAG_SB);
		//����Դ��ھ���״̬����������
		if (pressure > hanging) {
			RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
			RTC_WaitForLastTask();
			GPIO_SetBits(GPIOA, GPIO_Pin_2); //�رչ���
			IWDG_Feed();
			IWDG_Config(IWDG_Prescaler_256 ,4095);
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
	//2���ж�(����SOC)
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
		//FUNC_Check_Charging();
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
	//��ѭ��5msһ��
	while (FUNC_SleepOrNot())
	{
		COM_Listen_Reset();
		COM_Response();
		FUNC_Step_Counter();
		IWDG_Feed();
	}
	EEP_Sleep_Write();
	LEDDELAY;
	RTC_SetAlarm(RTC_GetCounter() + 25); //25s��
	RTC_WaitForLastTask();
	GPIO_ResetBits(GPIOA, GPIO_Pin_2); //�رչ���
	IWDG_Config(IWDG_Prescaler_256 ,4095);
	IWDG_Feed();
	//�������ģʽ
	PWR_EnterSTANDBYMode();
}

/*********************************************END OF FILE**********************/
