#ifndef __FUNCTION_
#define __FUNCTION_

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_led.h"
#include "bsp_usart.h"

#define PRESSURE_FACTOR 0.45 //�˲�ϵ��
#define BATTVOLT_FACTOR 0.3 //�˲�ϵ��

#define WALK_WEIGHT_RATIO 0.85 //��·ѹ����
#define RUN_WEIGHT_RATIO 0.75 //�ܲ�ѹ����

#define SAMPLEDELAY delay_ms(1); //��1ms��һ�ε�ѹ
#define LEDDELAY twinkle(100);

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float threshold;
	uint32_t current_steps;
	uint32_t total_steps;
} Step;

void FUNC_BattSOC_Caculation(void);
void FUNC_Functional_Initial(void);
void FUNC_Step_Counter(void);

void FUNC_Binding_Listen(void);
void FUNC_Upload_Listen(void);
void FUNC_SendKey(void);
void FUNC_ReceiveSeed(void);
void FUNC_Binding_Receiveid(void);
void FUNC_SendSoc(void);
void FUNC_SendCurrentStep(void);
void FUNC_SendTotalStep(void);
uint8_t FUNC_Binding_Idchecksum(void);

//��ʱ����
static void delay_ms(uint16_t nms)
{
	uint32_t temp;
	SysTick->LOAD = 9000 * nms;
	SysTick->VAL = 0X00;//��ռ�����
	SysTick->CTRL = 0X01;//��ʼ����
	do {
		temp = SysTick -> CTRL; //��ȡ����ʱ
	}
	while((temp & 0x01) && (!(temp & (1 << 16))));//�ȴ�ʱ�䵽��
  SysTick->CTRL = 0x00; //�رռ�����
  SysTick->VAL = 0X00; //��ռ�����
}

static void twinkle(uint16_t nms) {
	LED1_ON;
	LED2_ON;
	delay_ms(nms);
	LED1_OFF;
	LED2_OFF;
	delay_ms(nms);
	LED1_ON;
	LED2_ON;
	delay_ms(nms);
	LED1_OFF;
	LED2_OFF;
}

#endif
