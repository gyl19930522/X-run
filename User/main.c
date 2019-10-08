#include "bsp_adc.h"
#include "bsp_rtc.h"
#include "bsp_iwdg.h"
#include "eeprom.h"
#include "functional.h"
#include <inttypes.h>

<<<<<<< Updated upstream
const uint32_t device_id = 0x2A16E4E8;

//��״̬��0x1AΪ�󶨳ɹ�
volatile uint8_t binding_flag = 0x1A;
//APP ID
volatile uint32_t app_id;
//E��״̬��0������1����
volatile uint8_t eeprom_status = 0;
//��ص�ѹ
=======
//������
const uint32_t activation_code = 0x2A16E4E8; //0x80C5A77A��0x4A8B3FFF
const uint16_t sw_version = 0x3039;

//��״̬(0δ�󶨣�1��)
uint8_t binding_flag = 1; //20191005����Ϊ1
//E��״̬(0Ϊ���ϣ�1����)
uint8_t eeprom_status = 1;
//��·�����Ϣ
Step walking = {0.8f, 0};
//�ܲ������Ϣ
Step running = {1.1f, 0};
//FFT������
uint16_t fillcounter = 0;
//����ѹ��
float hanging = 3.27f;
//ѹ��ֵ
float pressure = 0;
//�Ʋ���־(0��ʾδ����Ʋ���1��ʾ�Ʋ��У�2��ʾ���ر궨��3��ʾ���ñ궨)
uint8_t step_flag = 0;
//���״̬(0δ��磬1���) ���Ƿ���RTC�ж����ж�
volatile uint8_t charging_flag = 0;
//���ڻظ���־
volatile uint8_t response_flag = 0;
//���ʵ��SOC
volatile float battsoc = 0;
//��ص�ѹ
>>>>>>> Stashed changes
volatile float battvolt = 0;
//ѹ�������˲�ֵ
volatile float pressure = 0;
//���SOC(%)
volatile uint8_t battsoc = 0;
//��·�����Ϣ
volatile Step walking = {2.1, 0, 0};
//�ܲ������Ϣ
volatile Step running = {1.6, 0, 0};
//����
volatile float weight = 2.4;
//����ѹ��
volatile float hanging = 2.65;
//���ü���
static uint16_t standingcounter = 0;
//�Ʋ�״̬(0Ϊ�Ʋ���1Ϊ�ر�)
volatile uint8_t stepflag = 1;

//ϵͳʱ��
struct rtc_time systmtime = {
	0, 0, 0, 8, 1, 2019, 0
};

<<<<<<< Updated upstream
//�������ڽ��ռ���
uint8_t receivecounter = 0;
//������������״̬
volatile uint8_t connectstatus = 0;

static uint8_t lastconnectstatus = 0;
static uint8_t overtimecounter = 0;

void sendmessage(void) {
	if (battsoc <= 5) {
		return;
	}
	if (binding_flag != 0x1A) {
		switch (connectstatus) {
			case 1:
				Usart_binding_sendseed();
				break;
			case 3:
				Usart_binding_verify();
				break;
			case 5:
				binding_flag = 0x1A;
				EEP_binding_write();
				Usart_SendByte(USART1, 0xAA);
				connectstatus = 0;
				stepflag = 1;
				break;
		}
	}
	else {
		switch (connectstatus) {
			case 1:
				Usart_sendsoc();
			  break;
			case 3:
				Usart_sendkey();
			  break;
			case 5:
				Usart_sendstep();
			  EEP_step_write();
			  break;
			case 6:
				weight = pressure;
				EEP_weight_write();
				Usart_SendByte(USART1, 0xAA);
				connectstatus = 0;
			  stepflag = 1;
			case 7:
				hanging = pressure * 0.98;
				EEP_hang_write();
			  Usart_SendByte(USART1, 0xAA);
				connectstatus = 0;
			  stepflag = 1;
		}
	}
}
		
void receivemessage(void) {
	if (battsoc <= 5) {
		return;
	}
	if (binding_flag != 0x1A) {
		switch (connectstatus) {
			case 0:
				Usart_binding_listen();
				break;
		  case 2:
				Usart_binding_receivekey();
				break;
			case 4:
				Usart_binding_receiveid();
			  break;
		}
	}
  else {
		switch (connectstatus) {
			case 0:
				Usart_upload_listen();
				break;
			case 2:
				Usart_receiveseed();
			  break;
			case 4:
				if (USART_ReceiveData(USART1) == 0xBB) {
					connectstatus++;
				}
				else if (USART_ReceiveData(USART1) == 0xCC) {
				  connectstatus = 6;
				}
				else if (USART_ReceiveData(USART1) == 0xDD) {
					connectstatus = 7;
				}
				else {
					connectstatus = 0;
					stepflag = 1;
				}
				break;
		}
	}
}

void listen_reset(void) {
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
=======
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
		if (pressure > HANG_RATIO * hanging && charging_flag == 0) {
			RTC_SetAlarm(RTC_GetCounter() + 5); //5s����
			RTC_WaitForLastTask();
			IWDG_Feed();
			IWDG_Config(IWDG_Prescaler_256, 938);
			PWR_EnterSTANDBYMode();
		}
	}
	//LED��ʼ��
	LED_GPIO_Config();
	//USART��ʼ����
	USART_Config();
	//20191005���
	binding_flag = 1;
	//���������Ʋ�
	if (binding_flag == 1) {
		step_flag = 1;
	}
	//���Ź���ʼ��
	IWDG_Init();
>>>>>>> Stashed changes
}

//RTC�жϴ���
void RTC_IRQHandler(void)
{
	//���ж�
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
	{
		RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
		RTC_WaitForLastTask();
<<<<<<< Updated upstream
		//�����жϴ�������
		FUNC_battSOC_caculation();
		listen_reset();
=======
		FUNC_Led_Breath();
		FUNC_ChargeOrNot();
		FUNC_BattSOC_Caculation();
>>>>>>> Stashed changes
	}
}

//�����жϴ���
void USART1_IRQHandler(void) {
	//�����ж�
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		receivemessage();
	}
}

<<<<<<< Updated upstream
void Initialization(void) {
  //led��ʼ��
	LED_GPIO_Config();
	//adc��ʼ��
	ADCx_Init();
	//оƬID
	Get_ChipID();
	//E����ʼ����ȡ
	EEP_initial_read();
	//���ڳ�ʼ��
	USART_Config();
	//�����빦�ܳ�ʼ��
	FUNC_functional_initial();
	//�Ƿ��Ǵ�standy���˳���
	if(PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) {
		PWR_ClearFlag(PWR_FLAG_SB);
		//����Դ�������״̬����������
		if (pressure > hanging) {
			RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
			RTC_WaitForLastTask();
			IWDG_Feed();
			IWDG_Config(IWDG_Prescaler_256 ,4095);
			//LEDDELAY;
			PWR_EnterSTANDBYMode();
		}
	}
	//δ��ʱ�������Ʋ�����
	if (binding_flag == 0x1A) {
		stepflag = 1;
	}
	//���Ź���ʼ��
	IWDG_Init();
	LEDDELAY;
}

//����͹���ģʽ�ж�
uint8_t SleepOrNot(void) {
	//SOC = 0ֱ������
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
=======
int main(void) {
>>>>>>> Stashed changes
	Initialization();
	//��ѭ��5msһ��
	while (SleepOrNot())
	{
		sendmessage();
		FUNC_step_counter();
		IWDG_Feed();
	}
<<<<<<< Updated upstream
	EEP_sleep_write();
	LEDDELAY;
	RTC_SetAlarm(RTC_GetCounter() + 25); //25s����
	RTC_WaitForLastTask();
	IWDG_Config(IWDG_Prescaler_256 ,4095); //���Ź����ʱ������Ϊ���
=======
	EEP_Sleep_Write();
	RTC_SetAlarm(RTC_GetCounter() + 5); //5s����
	RTC_WaitForLastTask();
	IWDG_Config(IWDG_Prescaler_256, 938);
>>>>>>> Stashed changes
	IWDG_Feed();
	//����͹���ģʽ
	PWR_EnterSTANDBYMode();
}

/*********************************************END OF FILE**********************/
