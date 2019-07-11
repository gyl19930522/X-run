// @gyl
#include "functional.h"

extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
extern float battvolt;
extern float pressure;
extern uint8_t battsoc;
extern volatile Step walking;
extern volatile Step running;
extern volatile float hanging;
extern volatile uint8_t stepflag;

//��ǰcycle���еĽ׶α�־
static uint8_t cycleflag = 0;
//�ϸ�cycle���еĽ׶α�־
static uint8_t cyclelastflag = 0;
//��ǰcycle��ĳ���׶γ���������
static uint8_t cyclecounter = 0;
//��ǰcycle�ĳ�ʱ������
static uint8_t overtimecounter = 0;

//ѹ���˲�������2ms���ȡ5����ѹ����ֵ��Ȼ����һ���ͺ��˲�
static void FUNC_pressure_filter(void) {
	uint8_t filtercounter;
	float pressurebuffer;
	for (filtercounter = 0 ; filtercounter < 5; filtercounter++) {
		pressurebuffer = ((float)filtercounter * pressurebuffer + (float) ADC_ConvertedValue[2] / 4096 * 3.3) / (float)(filtercounter + 1);
		SAMPLEDELAY;
	}
	pressure = (1 - PRESSURE_FACTOR) * pressurebuffer + PRESSURE_FACTOR * pressure;
}

//���SOC����
void FUNC_battSOC_caculation(void) {
	battvolt = (1 - BATTVOLT_FACTOR) * ((float) ADC_ConvertedValue[1] / 2048 * 3.3) + BATTVOLT_FACTOR * battvolt;
	if (battvolt > 4.2) {
		battsoc = 100;
	}
	else if (battvolt < 3.35) {
		battsoc = 0;
	}
	else {
		battsoc = (uint8_t)((battvolt - 3.35) / 0.85 * 100);
	}
}

//�����Գ�ʼ��
void FUNC_functional_initial(void) {
	uint8_t i;
	for (i = 0; i < 30; i++) {
		FUNC_pressure_filter();
		FUNC_battSOC_caculation();
	}
}

//cycle��ʱ�жϣ�����100������reset
static void FUNC_overtime_reset(void) {
	if (overtimecounter > 100) {
		LED1_OFF;
		LED2_OFF;
		cyclelastflag = 0;
		cycleflag = 0;
		overtimecounter = 0;
		cyclecounter = 0;
	}
	if (cyclelastflag == 0 && cycleflag > 0) {
		overtimecounter = 1;
	}
	else if (cyclelastflag > 0 && cycleflag > 0) {
		overtimecounter++;
	}
	else if (cyclelastflag > 4 && cycleflag == 0) {
		overtimecounter = 0;
	}
	cyclelastflag = cycleflag;
}
	
/*                    �Ʋ�ͼ��

            * * * 
----------*------ *--------------------------------�ܲ�ѹ��
         *|       |* 
        * |       | * 
-------*--|-------|--*-----------------------------��·ѹ��
      *|  |       |  |*
     * |  |       |  | *   ����3/4    �˳�3/4������5/6�����һ��Cycle���ع�0
----*--|--|-------|--|--*---|-----------|---*------������
    0  �� ��      �� ��  *  |           |  *
       �� ��      �� ��   * |           | *
       1  2       2  1     *|           |*
----------------------------*-----------*----------����ѹ����
														 *         *
                              *       *
                                * * *
*/
//stepflag = 0ʱ�����Ʋ�����
void FUNC_step_counter(void) {
	FUNC_pressure_filter();
	if (stepflag < 1) {
		FUNC_overtime_reset();
		switch (cycleflag) {
			case 0:
				if (pressure < walking.threshold && pressure >= running.threshold) {
					cycleflag = 1;
				}
				else if (pressure < running.threshold) {
					cycleflag = 2;
				}
				break;
			case 1:
				if (pressure < walking.threshold && pressure >= running.threshold) {
					cyclecounter++;
					if (cyclecounter >= 20) {
						LED1_ON;
						cyclecounter = 0;
						cycleflag = 3;
					}
				}
				else if (pressure < running.threshold) {
					cyclecounter = 0;
					cycleflag = 2;
				}
				break;
			case 2:
				if (pressure < running.threshold) {
					cyclecounter++;
					if (cyclecounter >= 10) {
						LED2_ON;
						cyclecounter = 0;
						cycleflag = 4;
					}
				}
				break;
			case 3:
				if (pressure > hanging) {
					cyclecounter++;
					if (cyclecounter >= 20) {
						cyclecounter = 0;
						cycleflag = 5;
					}
				}
				break;
			case 4:
				if (pressure > hanging) {
					cyclecounter++;
					if (cyclecounter >= 10) {
						cyclecounter = 0;
						cycleflag = 6;
					}
				}
				break;
			case 5:
				cycleflag = 0;
				walking.current_steps++;
				walking.total_steps++;
				LED1_OFF;
				break;
			case 6:
				cycleflag = 0;
				running.current_steps++;
				running.total_steps++;
				LED2_OFF;
				break;
		}
	}
}
