/*
 * Control.c
 *
 *  Created on: 2019��3��20��
 *      Author: zengwangfa
 *      Notes:  �˶��ܿ���
 */
 
#define LOG_TAG "Control"

#include <rtthread.h>
#include <elog.h>
#include <stdlib.h>
#include <math.h>

#include "Control.h"
#include "PID.h"
#include "rc_data.h"

#include "focus.h"
#include "led.h"
#include "servo.h"
#include "PropellerControl.h"
#include "propeller.h"
#include "sensor.h"

float Yaw_Control = 0.0f;//Yaw���� ƫ������ 
float Yaw = 0.0f;

extern int16 PowerPercent;
extern uint8 Frame_EndFlag;



#define STEP_VLAUE  1

/**
  * @brief  Convert_RockerValue(ң��������ת��Ϊ�ƽ�������ֵ)
  * @param  ҡ�����ݽṹ��ָ��
  * @retval None
  * @notice 
  */
void Convert_RockerValue(Rocker_Type *rc) //��ȡҡ��ֵ
{
		rt_enter_critical();/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */

		if(Frame_EndFlag){	

				rc->X = ControlCmd.Move - 126; 			  //ҡ��ֵ�任��X��ҡ��ֵ -127 ~ +127
				rc->Y = ControlCmd.Translation - 128  ;//					  Y��ҡ��ֵ -127 ~ +127
				rc->Z = ControlCmd.Vertical - 127;    //������128ʱ�ϸ�,С��128ʱ��Ǳ����ֵԽ���ٶ�Խ��
				rc->Yaw = ControlCmd.Rotate - 128;    //ƫ��
		}


		rt_exit_critical();		/* ���������� */		
		


}



	
/*******************************************
* �� �� ����void Speed_Buffer(int *BufferMember,int BufferRange)
* ��    �ܣ��ٶȻ�����
* ���������1.�������Ա��ַ��*BufferMenber�� 2.����ֵ��BufferRange��     
* �� �� ֵ��none
* ע    �⣺����ֵΪ ��STEP_VALUE
********************************************/
void Speed_Buffer(short *now_value,short *last_value,short range)
{		
		static uint16 diff_value = 0;
		diff_value = abs((*last_value) - (*now_value));//�ݴ��ֵ�ľ���ֵ
		
		if(diff_value >= range)//΢�ִ���Ԥ��ֵ����������
		{
				if(*now_value <= *last_value){
						*now_value = *last_value - STEP_VLAUE;
				}
				else{
						*now_value = *last_value + STEP_VLAUE;
				}
				*last_value = *now_value;	
		}
}


short last_left_speed  = 0;
short last_right_speed = 0;
short left_speed  = 0;
short right_speed = 0;
float speed = 0;			   //�ٶ��ܺ�
float left_precent = 0;	 //���ƽ�����ֵ�ٷֱ�
float right_precent = 0; //���ƽ�����ֵ�ٷֱ�
float Angle_Rad = 0.0f;
/**
  * @brief  FourtAxis_Control(���ƽ���ˮƽ����)
  * @param  ҡ�����ݽṹ��ָ��
  * @retval None
  * @notice 
  */	
void FourtAxis_Control(Rocker_Type *rc)		//�ƽ������ƺ���
{
/*
			50 		100		150		255
			16%		32%		48%		82%
*/
	
		rt_enter_critical();/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
	
		PropellerPower.PowerPercent = (float)ControlCmd.Power/80;//���㶯���ٷֱ� ���ʱΪ400%
	
		speed = sqrt(pow(rc->X,2)+pow(rc->Y,2));	//�ٶ��ܺ�
	
		rc->Angle = Rad2Deg(atan2(rc->X,rc->Y));// ��ȡatan�Ƕȣ�180 ~ -180
		if(rc->Angle < 0){rc->Angle += 360;}     /*�Ƕȱ任 �Լ����궨�� �Ƕ�˳�� 0~360��*/ 

		Angle_Rad = Deg2Rad(rc->Angle);

		/* �����ƽ��� �˶����ƹ�ʽ */
		left_speed   = abs(rc->X) * sin(Angle_Rad) + abs(rc->Y) * cos(Angle_Rad);//����ҡ�˻�ȡ
		right_speed  = abs(rc->X) * sin(Angle_Rad) - abs(rc->Y) * cos(Angle_Rad);
		
		
		/* ֱ��ǰ��/���˱���   90���12��    270���12��*/
		if( (rc->Angle >= 78 && rc->Angle <= 102) || (rc->Angle >= 258 && rc->Angle <= 282)  )//��ҡ�� ��Ϊ����ʱ������Y��ҡ��ֵ������Ϊǰ��/���ˡ�
		{
				left_speed   = abs(rc->X) * sin(Angle_Rad);
				right_speed  = abs(rc->X) * sin(Angle_Rad);
		}
		

		if(rc->X >= 0){ /* ������ʽ = ����ϵ��*(�����ٷֱ�*ҡ�˶�Ӧ������ֵ+ƫ��ֵ) */
				PropellerPower.leftDown  =  PropellerDir.leftDown  * (PropellerPower.PowerPercent * left_speed  + PropellerError.rightUp);//������ʽ = 
				PropellerPower.rightDown =  PropellerDir.rightDown * (PropellerPower.PowerPercent * right_speed + PropellerError.rightDown);
		}
		else{
				PropellerPower.leftDown  =  PropellerDir.leftDown  * (PropellerPower.PowerPercent * left_speed  - PropellerError.leftDown) ;//������ʽ = �����ٷֱ�*����ϵ��*(ҡ�˶�Ӧ������ֵ+ƫ��ֵ)
				PropellerPower.rightDown =  PropellerDir.rightDown * (PropellerPower.PowerPercent * right_speed - PropellerError.leftUp);		
		}
		
		Speed_Buffer(&PropellerPower.leftDown , &last_left_speed, 4);	//����ٶȻ���
		Speed_Buffer(&PropellerPower.rightDown, &last_right_speed,4);		

		rt_exit_critical();		/* ���������� */
		
		
		
		
		
//		if(rc->Y >= 0 )						//��Y�� >= 0 ʱ�����ƽ����ٶ� >= ���ƽ���
//		{
//				left_precent  = rc->X / abs(rc->X);		
//				right_precent = rc->X / speed;
//		}
//		else										  //��Y�� < 0 ʱ�� ���ƽ����ٶ� >=���ƽ���
//		{
//				left_precent  = rc->X / speed;
//				right_precent = rc->X / abs(rc->X);
//		}
//		if(rc->X >= 0)
//		{
//				left_speed  = left_precent  * speed ;			//����ٶ�
//				right_speed = right_precent * (speed +30);
//		}
//		else
//		{
//				left_speed  = left_precent  * (speed + 30);			//����ٶ�
//				right_speed = right_precent * speed ;
//		}

}




void Angle_Control(void)
{
	
		if(Sensor.JY901.Euler.Yaw < 0) Yaw = (180+Sensor.JY901.Euler.Yaw) + 180;//�ǶȲ���
		if(Sensor.JY901.Euler.Yaw > 0) Yaw = (float)Sensor.JY901.Euler.Yaw;            //�ǶȲ���
		Total_Controller.Yaw_Angle_Control.Expect = (float)Yaw_Control;//ƫ�����ٶȻ�������ֱ����Դ��ң���������
		Total_Controller.Yaw_Angle_Control.FeedBack = (float)Yaw;//ƫ���Ƿ���
	
		PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);//ƫ���Ƕȿ���
	

		//ƫ�����ٶȻ���������Դ��ƫ���Ƕȿ��������
		//Total_Controller.Yaw_Gyro_Control.Expect = Total_Controller.Yaw_Angle_Control.Control_OutPut;
}



void Depth_PID_Control(float expect_depth,float sensor_depth)
{
		
		Total_Controller.High_Position_Control.Expect = expect_depth ; //���������ң��������
		Total_Controller.High_Position_Control.FeedBack = sensor_depth;  //��ǰ��ȷ���
		PID_Control(&Total_Controller.High_Position_Control);//�߶�λ�ÿ�����
	
		robot_upDown(&Total_Controller.High_Position_Control.Control_OutPut);		//��ֱ�ƽ�������
}



void Gyro_Control(void)//���ٶȻ�
{

//  	ƫ����ǰ������
//  	Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;


//		PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
//		Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
//			/Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
//		//**************************ƫ����ǰ������**********************************
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
//			+Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;//ƫ����ǰ������
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
//																																		 -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
//																																		 Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
//		Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
//		

}

/*����� ������yaw MSH���� */
static int depth(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1])<100){
				Expect_Depth = atoi(argv[1]);
		}
		else {
				log_e("Error! The  value is out of range!");
		}

		
_exit:
    return result;
}
MSH_CMD_EXPORT(depth,ag: depth 10);



/*����е�ۡ���� ����yaw MSH���� */
static int yaw(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		Yaw_Control = atoi(argv[1]); //ASCII to Integer
		
_exit:
    return result;
}
MSH_CMD_EXPORT(yaw,ag: yaw 100);



/*�������� �޸�MSH���� */
static int unlock(int argc, char **argv) //ֻ���� 0~3.0f
{
		ControlCmd.All_Lock = UNLOCK;
		return 0;
}
MSH_CMD_EXPORT(unlock,unlock);


/*�������� �޸�MSH���� */
static int lock(int argc, char **argv) //ֻ���� 0~3.0f
{
		ControlCmd.All_Lock = LOCK;
		return 0;
}
MSH_CMD_EXPORT(lock,lock);

