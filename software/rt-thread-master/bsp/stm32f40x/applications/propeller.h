#ifndef __PROPELLER_H
#define __PROPELLER_H

#include "DataType.h"


typedef struct //�ƽ�����ֵ�����ֵ����Сֵ
{
		int16 PowerMax;		
		int16 PowerMed;
		int16 PowerMin;	
	  int16 PowerDeadband;	//����ֵ
	
}PropellerParameter_Type;//�ƽ��������ṹ��

typedef struct //�ƽ�������
{
		char leftUp;  //����
		char rightUp;
		char leftDown; //����
		char rightDown;
		char leftMiddle; //����
		char rightMiddle;		

}PropellerDir_Type;

typedef struct//����ʱ�����ƽ�����ƫ���� 
{
		int16 leftUp;  //����
		int16 rightUp;
		int16 leftDown; //����
		int16 rightDown;
		int16 leftMiddle; //����
		int16 rightMiddle;

}PropellerError_Type;//�ƽ���ƫ��ֵ


typedef struct
{
	  int16 Power; //�ƽ�������ϵ�� ������Ϊ����
	
		int16 leftUp;
		int16 rightUp;
		int16 leftDown;
		int16 rightDown;
		int16 leftMiddle;
		int16 rightMiddle;

}PropellerPower_Type;//�����ƽ�������

typedef enum
{
		Forward   = 1, //ǰ��
		BackAway  = 2, //����
	
		TurnLeft  = 1, //��ת
		TurnRight = 2, //��ת
	
		RiseUp    = 1, //����
		Dive      = 2, //��Ǳ
	
		MoveLeft  = 1, //����
		MoveRight = 2, //����

		Stop      = 0
		
}ActionType_Enum; //����ָ��ö��

extern PropellerDir_Type PropellerDir;
extern PropellerParameter_Type PropellerParameter; 
extern PropellerPower_Type  PropellerPower; //�ƽ������������
extern PropellerError_Type  PropellerError; //�ƽ���ƫ��ֵ

void Propeller_Init(void);

void PWM_Update(PropellerPower_Type* power);//�ƽ���PWM ����





#endif




