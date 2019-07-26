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
char ACC1 = 0,ACC2 = 0,ACC3 = 0,ACC4 = 0;
Direction_Type Direction = {1,1,1,1,1,1};


extern int16 PowerPercent;
extern uint8 Frame_EndFlag;



#define STEP_VLAUE  1 

/*******************************************
* �� �� ����void BufferMember(int BufferMember,int BufferRange)
* ��    �ܣ��ٶȻ�����
* ���������1.�������Ա��BufferMenber�� 2.����ֵ��BufferRange��     
* �� �� ֵ��none
* ע    �⣺����ֵΪ ��STEP_VALUE
********************************************/
void Speed_Buffer(int BufferMember,int BufferRange)
{		
		static int LastMember = 0 ;
		if(abs(abs(LastMember) - abs(BufferMember))>=BufferRange)
		{
				if(BufferMember < LastMember)
				{
						BufferMember = LastMember - STEP_VLAUE;
				}
				else
				{
						BufferMember = LastMember + STEP_VLAUE;
				}
						LastMember = BufferMember;	
		}
}



/**
  * @brief  FourtAxis_Control(���ƽ���ˮƽ����)
  * @param  ҡ�����ݽṹ��ָ��
  * @retval None
  * @notice 
  */	
void FourtAxis_Control(Rocker_Type *rc)		//�ƽ������ƺ���
{
		static int16 left_speed  = 0;
		static int16 right_speed = 0;
		static float speed;			   //�ٶ��ܺ�
		static float left_precent;	 //���ƽ�����ֵ�ٷֱ�
		static float right_precent; //���ƽ�����ֵ�ٷֱ�
		speed = sqrt(pow(rc->X,2)+pow(rc->Y,2));	//�ٶ��ܺ�
		if(rc->Y >= 0)						//��Y�� >= 0 ʱ�����ƽ����ٶ� >=���ƽ���
		{
				left_precent  = rc->X / (abs(rc->X)+1);		
				right_precent = rc->X / speed;
		}
		else										  //��Y�� < 0 ʱ�� ���ƽ����ٶ� >=���ƽ���
		{
				left_precent  = rc->X / speed;
				right_precent = rc->X / (abs(rc->X)+1);
		}
		left_speed  = left_precent  * speed;			//����ٶ�
		right_speed = right_precent * speed;
		
		Speed_Buffer(left_speed, 4);	//����ٶȻ���
		Speed_Buffer(right_speed,4);	
		
		PropellerPower.leftDown = PropellerDir.leftDown *left_speed; 		//�����ƽ���
		PropellerPower.rightDown =PropellerDir.rightDown*right_speed;
}


void SixAxis_Control(Rocker_Type *rc)
{
		static int16 LeftUp,LeftDown,LeftMid,RightUp,RightDown,RightMid;
		static int16 LeftUpFlag,LeftDownFlag,LeftMidFlag,RightUpFlag,RightDownFlag,RightMidFlag;
		static int16 LeftUpCoe_X,LeftDownCoe_X,LeftMidCoe = 1,RightUpCoe_X,RightDonwCoe_X,RightMidCoe = 1;
		static int16 LeftUpCoe_Y,LeftDownCoe_Y,RightUpCoe_Y,RightDonwCoe_Y;
	
		LeftUpCoe_X   = 0; 						
		LeftDownCoe_X = 0;
		RightUpCoe_X  = 0;
		RightDonwCoe_X= 0;
		LeftUpCoe_Y   = 0;
		LeftDownCoe_Y = 0;
		RightUpCoe_Y  = 0;
		RightDonwCoe_Y= 0;
				
		if(rc->X != 0 && rc->Y != 0)
		{		
				LeftUpCoe_Y   =  1;
				LeftDownCoe_Y =  1;
				RightUpCoe_Y  = -1;
				RightDonwCoe_Y= -1;		
		}
		else
		{
				if(abs(rc->X) > 0)
				{
						LeftUpCoe_X   = 1;
						LeftDownCoe_X = 1;
						RightUpCoe_X  = 1;
						RightDonwCoe_X= 1;
				}
								
				if(abs(rc->Y) > 0)
				{
						LeftUpCoe_Y   =  1;
						LeftDownCoe_Y = -1;
						RightUpCoe_Y  = -1;
						RightDonwCoe_Y=  1;
				}
		}
		
		LeftUpFlag    = LeftUpCoe_X   * rc->X + LeftUpCoe_Y   * rc->Y ;
		LeftDownFlag  = LeftDownCoe_X * rc->X + LeftDownCoe_Y * rc->Y ;
		RightUpFlag   = RightUpCoe_X  * rc->X + RightUpCoe_Y  * rc->Y ;
		RightDownFlag = RightDonwCoe_X* rc->X + RightDonwCoe_Y* rc->Y ;
		
		Speed_Buffer(LeftUpFlag   ,4);
		Speed_Buffer(LeftDownFlag ,4);
		Speed_Buffer(RightUpFlag  ,4);
		Speed_Buffer(RightDownFlag,4);

		
		PropellerPower.leftUp     = PropellerDir.leftUp     * (LeftUpFlag   + LeftUp)   ;
		PropellerPower.leftDown   = PropellerDir.leftDown   * (LeftDownFlag + LeftDown) ;
		PropellerPower.rightUp    = PropellerDir.rightUp    * (RightUpFlag  + RightUp)  ;
		PropellerPower.rightDown  = PropellerDir.rightDown  * (RightDownFlag+ RightDown);

	
}
/**
  * @brief  Convert_RockerValue(ң��������ת��Ϊ�ƽ�������ֵ)
  * @param  ҡ�����ݽṹ��ָ��
  * @retval None
  * @notice 
  */
void Convert_RockerValue(Rocker_Type *rc) //��ȡҡ��ֵ
{

		static int16 LastRcX = 0 ,LastRcY = 0;
		if(Frame_EndFlag){	

				rc->X = ControlCmd.Move - 128; 			  //ҡ��ֵ�任��X��ҡ��ֵ -127 ~ +127
				rc->Y = ControlCmd.Translation - 128  ;//					  Y��ҡ��ֵ -127 ~ +127
				rc->Z = ControlCmd.Vertical - 128;    //������128ʱ�ϸ�,С��128ʱ��Ǳ����ֵԽ���ٶ�Խ��
				rc->Yaw = ControlCmd.Rotate - 128;    //ƫ��
		}


		
		

//		if(SIX_AXIS == VehicleMode){
//				rc->Angle = Rad2Deg(atan2(rc->X,rc->Y));// ��ȡatan�Ƕȣ�180 ~ -180
//				if(rc->Angle < 0){rc->Angle += 360;}  /*�Ƕȱ任 �Լ����궨�� �Ƕ�˳�� 0~360��*/ 	
//																				
//				rc->Force = sqrt(rc->X*rc->X + rc->Y*rc->Y);	//�����б��
//				rc->Fx = (sqrt(2)/2)*(rc->X - rc->Y);//ת���� X�����	  ��Ϊ�Ľ�����Ϊ45���
//				rc->Fy = (sqrt(2)/2)*(rc->X + rc->Y);//ת���� Y�����	  ��Ϊ�Ľ�����Ϊ45���
//				   
//				/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */   //ControlCmd.Power
//				PropellerPower.leftUp =    (PropellerDir.leftUp    * (PowerPercent) * ( rc->Fy) )/70 + ACC1 + PropellerError.leftUp;  //PowerΪ�ƽ���ϵ�� 0~300%
//				PropellerPower.rightUp =   (PropellerDir.rightUp   * (PowerPercent) * ( rc->Fx) )/70 + ACC2 + PropellerError.rightUp;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
//				PropellerPower.leftDown =  (PropellerDir.leftDown  * (PowerPercent) * ( rc->Fx) )/70 + ACC3 + PropellerError.leftDown ; 
//				PropellerPower.rightDown = (PropellerDir.rightDown * (PowerPercent) * ( rc->Fy) )/70 + ACC4 + PropellerError.rightDown;
//				

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
	
		robot_upDown(Total_Controller.High_Position_Control.Control_OutPut);		//��ֱ�ƽ�������
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

