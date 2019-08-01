#ifndef __FUNCTION_
#define __FUNCTION_

#include "stm32f10x.h"
#include "bsp_led.h"

//�����˷�
#define ProductRe(a, b, c, d) ((a) * (c) - (b) * (d))
#define ProductIm(a, b, c, d) ((a) * (d) + (b) * (c))

#define INITIAL_FACTOR 						0.55f //��ʼ�˲�ϵ��
#define BATTVOLT_FACTOR 					0.75f //����˲�ϵ��
#define PRESSURE_FACTOR 					0.65f //ѹ���˲�ϵ��

#define HANG_RATIO 								0.98f //����ϵ��
#define WALK_AMP_RATIO 						0.9f
#define RUN_AMP_RATIO 						1.2f

#define HANG_AMP_THRESHOLD 				0.001f
#define WEIGHT_AMP_THRESHOLD 			0.05f

#define MIN_HANG 									2.45f
#define MAX_WEIGHT 								2.2f

#define INITIALDELAY delay_ms(1); //��1ms��һ�ε�ѹ
#define SAMPLEDELAY delay_ms(2); //��2ms��һ�ε�ѹ
#define LEDDELAY twinkle(100);

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float 	 threshold;
	uint32_t current_steps;
	uint32_t total_steps;
} Step;

typedef struct Complex {
	float re;
	float im;
} complex;

typedef struct Amplitude {
	int16_t  index;
	float    value;
} amplitude;

uint8_t FUNC_SleepOrNot(void);
void FUNC_ChargeOrNot(void);
void FUNC_BattSOC_Caculation(void);
void FUNC_Functional_Initial(void);
void FUNC_Pressure_Filter(void);
void FUNC_Step_CountOrCalibrate(void);

//��ʱ����
static void delay_ms(uint16_t nms)
{
	uint32_t temp;
	SysTick->LOAD = 9000 * nms;
	SysTick->VAL = 0x00;//��ռ�����
	SysTick->CTRL = 0x01;//��ʼ����
	do {
		temp = SysTick -> CTRL; //��ȡ����ʱ
	}
	while((temp & 0x01) && (!(temp & (1 << 16))));//�ȴ�ʱ�䵽��
  SysTick->CTRL = 0x00; //�رռ�����
  SysTick->VAL = 0x00; //��ռ�����
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
