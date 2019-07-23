#ifndef __FUNCTION_
#define __FUNCTION_

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_led.h"

#define PRESSURE_FACTOR 0.55 //�˲�ϵ��
#define BATTVOLT_FACTOR 0.05 //�˲�ϵ��

#define HANG_RATIO 0.98 //����ѹ����
#define WALK_WEIGHT_RATIO 0.85 //��·ѹ����
#define RUN_WEIGHT_RATIO 0.65 //�ܲ�ѹ����

#define MIN_HANG 2.53
#define MAX_HANG 2.57

#define MIN_WEIGHT 1.85
#define MAX_WEIGHT 2.25

#define SAMPLEDELAY delay_ms(1); //��1ms��һ�ε�ѹ
#define LEDDELAY twinkle(100);

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float threshold;
	uint32_t current_steps;
	uint32_t total_steps;
} Step;

uint8_t FUNC_SleepOrNot(void);
void FUNC_BattSOC_Caculation(void);
void FUNC_Functional_Initial(void);
void FUNC_Step_Counter(void);

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
