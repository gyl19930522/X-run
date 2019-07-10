#ifndef __FUNCTION_
#define __FUNCTION_

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_led.h"

#define PRESSURE_FACTOR 0.5 //�˲�ϵ��
#define BATTVOLT_FACTOR 0.3 //�˲�ϵ��

#define WALK_WEIGHT_RATIO 0.85 //��·ѹ����
#define RUN_WEIGHT_RATIO 0.75 //�ܲ�ѹ����

#define SAMPLEDELAY delay_ms(2); //��2ms��һ�ε�ѹ

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float threshold;
	uint32_t current_steps;
	uint32_t total_steps;
} Step;

void FUNC_battSOC_caculation(void);
void FUNC_functional_initial(void);
void FUNC_step_counter(void);

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

#endif