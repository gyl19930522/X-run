#ifndef __FUNCTION_
#define __FUNCTION_

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_led.h"
#include "bsp_usart.h"

#define PRESSURE_FACTOR 0.45 //�˲�ϵ��
#define BATTVOLT_FACTOR 0.3 //�˲�ϵ��

<<<<<<< Updated upstream
#define WALK_WEIGHT_RATIO 0.85 //��·ѹ����
#define RUN_WEIGHT_RATIO 0.75 //�ܲ�ѹ����

#define SAMPLEDELAY delay_ms(1); //��1ms��һ�ε�ѹ
#define LEDDELAY twinkle(100);

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float threshold;
	uint32_t current_steps;
	uint32_t total_steps;
=======
#define INITIAL_FACTOR 						0.35f //��ʼ�˲�ϵ��
#define BATTVOLT_FACTOR 					0.55f //����˲�ϵ��
#define PRESSURE_FACTOR 					0.75f //ѹ���˲�ϵ��

#define HANG_RATIO 								0.98f //����ϵ��
#define WALK_AMP_RATIO						0.4f
#define RUN_AMP_RATIO							0.65f

#define HANG_AMP_THRESHOLD	 			0.001f
#define WEIGHT_AMP_THRESHOLD			0.05f

#define MIN_HANG 									3.2f
#define MAX_WEIGHT								2.9f

#define INITIALDELAY delay_ms(1); //��1ms��һ�ε�ѹ
#define SAMPLEDELAY delay_ms(2); //��2ms��һ�ε�ѹ

//��ʾ������run��walkͨ��
typedef struct Stepping {
	float 	 		threshold;
	uint32_t 		total_steps;
>>>>>>> Stashed changes
} Step;

void FUNC_battSOC_caculation(void);
void FUNC_functional_initial(void);
void FUNC_step_counter(void);

<<<<<<< Updated upstream
void Usart_binding_listen(void);
void Usart_upload_listen(void);
void Usart_binding_sendseed(void);
void Usart_sendkey(void);
void Usart_binding_receivekey(void);
void Usart_receiveseed(void);
void Usart_binding_receiveid(void);
void Usart_sendsoc(void);
void Usart_sendstep(void);
void Usart_binding_verify(void);
=======
uint8_t FUNC_SleepOrNot(void);
void FUNC_ChargeOrNot(void);
void FUNC_Led_Breath(void);
void FUNC_BattSOC_Caculation(void);
void FUNC_Functional_Initial(void);
void FUNC_Pressure_Filter(void);
void FUNC_Step_CountOrCalibrate(void);
>>>>>>> Stashed changes

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
