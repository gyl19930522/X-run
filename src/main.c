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
#include <inttypes.h>

const uint32_t device_id = 0x2A16E4E8;

//��״̬
uint8_t binding_flag = 0;
//APP ID
volatile uint32_t app_id;
//E��״̬
uint8_t eeprom_status = 0;
//��ص�ѹ
volatile float battvolt = 0;
//ѹ��ֵ
float pressure = 0;
//���SOC
volatile uint8_t battsoc = 0;
//��·�����Ϣ
Step walking = {2.1, 0, 0};
//�ܲ������Ϣ
Step running = {1.6, 0, 0};
//����ѹ��
float weight = 2.4;
//����ѹ��
float hanging = 2.65;
//�Ʋ���־λ
volatile uint8_t stepflag = 0;
//���ü���
static uint16_t standingcounter = 0;

//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 8, 1, 2019, 0
};

//����״̬
volatile uint8_t receivecounter = 0;
//ͨѶ����״̬
volatile uint8_t connectstatus = 0;
static volatile uint8_t lastconnectstatus = 0;
//��ʱ����
static volatile uint8_t overtimecounter = 0;

//������E������
void Powersupply_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//�򿪹���
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void Send_Message(void) {
	if (battsoc <= 5) {
		return;
	}
	if (binding_flag != 0x1A) {
		switch (connectstatus) {
			case 1:
				Usart_SendByte(USART1, 0xAA);
				break;
			case 3:
				FUNC_SendKey();
				break;
			case 6:
				if (FUNC_Binding_Idchecksum() > 0) {
					binding_flag = 0x1A;
					EEP_Binding_Write();
					Usart_SendByte(USART1, 0xAA);
					stepflag = 1;
				}
				else {
					Usart_SendByte(USART1, 0xFF);
				}
				connectstatus = 0;
				break;
		}
	}
	else {
		switch (connectstatus) {
			case 1:
				FUNC_SendSoc();
			  break;
			case 2:
				Usart_SendByte(USART1, 0xAA);
			  break;
			case 4:
				FUNC_SendKey();
			  break;
			case 6:
				FUNC_SendCurrentStep();
			  EEP_Step_Write();
			  break;
			case 7:
				FUNC_SendTotalStep();
				break;
			case 8:
				EEP_Weight_Write();
				Usart_SendByte(USART1, 0xAA);
				connectstatus = 0;
			  stepflag = 1;
			case 9:
				EEP_Hang_Write();
			  Usart_SendByte(USART1, 0xAA);
				connectstatus = 0;
			  stepflag = 1;
		}
	}
}
		
void Receive_Message(void) {
	uint8_t data;
	if (battsoc <= 5) {
		return;
	}
	if (binding_flag != 0x1A) {
		switch (connectstatus) {
			case 0:
				FUNC_Binding_Listen();
				break;
		  case 2:
				FUNC_ReceiveSeed();
				break;
			case 4:
				if (USART_ReceiveData(USART1) == 0xAA) {
					connectstatus++;
				}
				else {
				  connectstatus = 0;
				}
			  break;
			case 5:
				FUNC_Binding_Receiveid();
				break;
		}
	}
  else {
		switch (connectstatus) {
			case 0:
				FUNC_Upload_Listen();
				break;
			case 3:
				FUNC_ReceiveSeed();
			  break;
			case 5:
				data = USART_ReceiveData(USART1);
				switch (data) {
					case 0xBB:
						connectstatus++;
						break;
					case 0xCC:
						connectstatus = 7;
						break;
					case 0xDD:
						connectstatus = 8;
						break;
					case 0xEE:
						connectstatus = 9;
						break;
					default:
						connectstatus = 0;
						stepflag = 1;
					  break;
				}
				break;
		}
	}
}

void Listen_Reset(void) {
	lastconnectstatus = connectstatus;
	if (lastconnectstatus == 0 && connectstatus > 0) {
		overtimecounter = 1;
	}
	else if (lastconnectstatus == 0 && connectstatus == 0) {
		overtimecounter = 0;
	}
	else {
		overtimecounter++;
	}
	if (overtimecounter > 5) {
		overtimecounter = 0;
		connectstatus = 0;
		lastconnectstatus = 0;
		receivecounter = 0;
		if (binding_flag == 0x1A) {
			stepflag = 1;
		}
	}
}

//RTC�ж�
void RTC_IRQHandler(void)
{
	//���жϣ�����SOC����������ͨѶ�Ƿ�ʱ��
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
		FUNC_BattSOC_Caculation();
		Listen_Reset();
	}
}

//���ڽ����ж�
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		Receive_Message();
	}
}

void Initialization(void) {
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
	if (binding_flag == 0x1A) {
		stepflag = 1;
	}
	//���Ź���ʼ��
	IWDG_Init();
	LEDDELAY;
}

//�ж��Ƿ�������
uint8_t SleepOrNot(void) {
	//SOC = 0ֱ�ӽ������
	if (battsoc < 1) {
		return 0;
	}
	//����״̬��25s��������
	if (pressure > hanging && stepflag) {
		standingcounter++;
		if (standingcounter > 5000) {
			return 0;
		}
	}
	else {
		standingcounter = 0;
	}
	return 1;
}

int main(void)
{	
	Initialization();
	//��ѭ��5msһ��
	while (SleepOrNot())
	{
		Send_Message();
		FUNC_Step_Counter();
		IWDG_Feed();
	}
	EEP_Sleep_Write();
	LEDDELAY;
	RTC_SetAlarm(RTC_GetCounter() + 25); //25s��
	RTC_WaitForLastTask();
	GPIO_SetBits(GPIOA, GPIO_Pin_2); //�رչ���
	IWDG_Config(IWDG_Prescaler_256 ,4095);
	IWDG_Feed();
	//�������ģʽ
	PWR_EnterSTANDBYMode();
}

/*********************************************END OF FILE**********************/
