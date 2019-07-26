#ifndef __SERVO_H
#define __SERVO_H

#include "drv_pwm.h"

typedef struct  /*  */
{
		short MaxValue;		//��� �������ֵ
		short MinValue;	  //��� ����
		short MedValue;   //��� ��ֵ
		short CurrentValue; //�����ǰֵ
		uint8 Speed;		  //����ٶ�ֵ
}ServoType;



void RoboticArm_Control(uint8 *action);

void YunTai_Control(uint8 *action);
void DirectionProportion(int Mode);


extern ServoType RoboticArm ;//��е��

extern ServoType YunTai; //��̨

extern int DirectionMode;


#endif
