#ifndef __PID_H_
#define __PID_H_


#include "sys.h"


typedef struct
{
		float p;
		float i;
		float d;
}Vector3f_pid;


typedef struct
{
		u8 Err_Limit_Flag :1;//ƫ���޷���־
		u8 Integrate_Limit_Flag :1;//�����޷���־
		u8 Integrate_Separation_Flag :1;//���ַ����־
		float Expect;//����
		float FeedBack;//����ֵ
		float Err;//ƫ��
		float Last_Err;//�ϴ�ƫ��
		float Err_Max;//ƫ���޷�ֵ
		float Integrate_Separation_Err;//���ַ���ƫ��ֵ
		float Integrate;//����ֵ
		float Integrate_Max;//�����޷�ֵ
		float Kp;//���Ʋ���Kp  12
		float Ki;//���Ʋ���Ki  13
		float Kd;//���Ʋ���Kd  14
		float Control_OutPut;//�����������
		float Last_Control_OutPut;//�ϴο����������
		float Control_OutPut_Limit;//����޷�
		/***************************************/
		float Pre_Last_Err;//���ϴ�ƫ��
		float Adaptable_Kd;//����Ӧ΢�ֲ���
		float Last_FeedBack;//�ϴη���ֵ
		float Dis_Err;//΢����
		float Dis_Error_History[5];//��ʷ΢����
		float Err_LPF;
		float Last_Err_LPF;
		float Dis_Err_LPF;
		float Last_Dis_Err_LPF;
		float Pre_Last_Dis_Err_LPF;
		float Scale_Kp;
		float Scale_Ki;
		float Scale_Kd;
}PID_Controler;


typedef struct
{
		PID_Controler Roll_Gyro_Control;  //���ٶ�
		PID_Controler Pitch_Gyro_Control;
		PID_Controler Yaw_Gyro_Control;
	
		PID_Controler Roll_Angle_Control; //���Ƚ�
		PID_Controler Pitch_Angle_Control;
		PID_Controler Yaw_Angle_Control;
	
		PID_Controler High_Speed_Control;  //�߶�
		PID_Controler High_Position_Control;

}AllControler;

typedef enum
{

		Roll_Gyro_Controler=0,  //���ٶ�
		Pitch_Gyro_Controler=1,
		Yaw_Gyro_Controler=2,
	
		Roll_Angle_Controler=3,  //�Ƕ�
		Pitch_Angle_Controler=4, 
		Yaw_Angle_Controler=5,
	
		High_Speed_Controler=6,	
		High_Position_Controler=7

}Controler_Label;


extern AllControler Total_Controller;


void Total_PID_Init(void);


#endif



