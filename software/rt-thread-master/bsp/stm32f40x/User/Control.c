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
#include "RC_Data.h"

#include "focus.h"
#include "led.h"
#include "servo.h"
#include "PropellerControl.h"
#include "propeller.h"
#include "sensor.h"
#include "Depth.h"


float Yaw_Control = 0.0f;//Yaw���� ƫ������ 
float Yaw = 0.0f;
char ACC1 = 0,ACC2 = 0,ACC3 = 0,ACC4 = 0;
Direction_Type Direction = {1,1,1,1,1,1};


extern int16 PowerPercent;
extern uint8 Frame_EndFlag;



#define ShutDown 1

/**
  * @brief  highSpeed Devices_Control(�����豸����)
  * @param  None
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

		if(abs(abs(LastRcX) - abs(rc->X))>=ShutDown*4) //�����ֵ����9������������
		{
				if(rc->X < LastRcX)														//�ж�ǰ�����Ǻ���
				{
					rc->X = LastRcX - ShutDown;									//��С���ٶ�
				}
				else
				{
					rc->X = LastRcX + ShutDown;
				}
				LastRcX = rc->X;	
		}
		if(abs(abs(LastRcY) - abs(rc->Y))>=ShutDown*4)
		{
				if(rc->Y < LastRcY)
				{
					rc->Y = LastRcY - ShutDown;
				}
				else
				{
					rc->Y = LastRcY + ShutDown;
				}
				LastRcY = rc->Y;	
		}
		
		
<<<<<<< HEAD
		if(ROV_Mode == VehicleMode){
				rc->Angle = Rad2Deg(atan2(rc->X,rc->Y));// ��ȡatan�Ƕȣ�180 ~ -180
				if(rc->Angle < 0){rc->Angle += 360;}  /*�Ƕȱ任 �Լ����궨�� �Ƕ�˳�� 0~360��*/ 	
																				
				rc->Force = sqrt(rc->X*rc->X + rc->Y*rc->Y);	//�����б��
				rc->Fx = (sqrt(2)/2)*(rc->X - rc->Y);//ת���� X�����	  ��Ϊ�Ľ�����Ϊ45���
				rc->Fy = (sqrt(2)/2)*(rc->X + rc->Y);//ת���� Y�����	  ��Ϊ�Ľ�����Ϊ45���
				   
				/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */   //ControlCmd.Power
				PropellerPower.leftUp =    (PropellerDir.leftUp    * (PowerPercent) * ( rc->Fy) )/70 + ACC1 + PropellerError.leftUp;  //PowerΪ�ƽ���ϵ�� 0~300%
				PropellerPower.rightUp =   (PropellerDir.rightUp   * (PowerPercent) * ( rc->Fx) )/70 + ACC2 + PropellerError.rightUp;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
				PropellerPower.leftDown =  (PropellerDir.leftDown  * (PowerPercent) * ( rc->Fx) )/70 + ACC3 + PropellerError.leftDown ; 
				PropellerPower.rightDown = (PropellerDir.rightDown * (PowerPercent) * ( rc->Fy) )/70 + ACC4 + PropellerError.rightDown;
				

		}
=======
//		if(ROV_Mode == VehicleMode){
//				rc->Angle = Rad2Deg(atan2(rc->X,rc->Y));// ��ȡatan�Ƕȣ�180 ~ -180
//				if(rc->Angle < 0){rc->Angle += 360;}  /*�Ƕȱ任 �Լ����궨�� �Ƕ�˳�� 0~360��*/ 	
//																				
//				rc->Force = sqrt(rc->X*rc->X + rc->Y*rc->Y);	//�����б��
//				rc->Fx = (sqrt(2)/2)*(rc->X - rc->Y);//ת���� X�����	  ��Ϊ�Ľ�����Ϊ45���
//				rc->Fy = (sqrt(2)/2)*(rc->X + rc->Y);//ת���� Y�����	  ��Ϊ�Ľ�����Ϊ45���
//			
//				/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */   //ControlCmd.Power
//				PropellerPower.leftUp =    (PropellerDir.leftUp    * (PowerPercent) * ( rc->Fy) )/70  + PropellerError.leftUp;  //PowerΪ�ƽ���ϵ�� 0~300%
//				PropellerPower.rightUp =   (PropellerDir.rightUp   * (PowerPercent) * ( rc->Fx) )/70  + PropellerError.rightUp;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
//				PropellerPower.leftDown =  (PropellerDir.leftDown  * (PowerPercent) * ( rc->Fx) )/70  + PropellerError.leftDown ; 
//				PropellerPower.rightDown = (PropellerDir.rightDown * (PowerPercent) * ( rc->Fy) )/70  + PropellerError.rightDown;
//								
////				/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */   //ControlCmd.Power
////				PropellerPower.leftUp =    (PropellerDir.leftUp    * (PowerPercent) * ( rc->Fy) )/70 + ACC1 + PropellerError.leftUp;  //PowerΪ�ƽ���ϵ�� 0~300%
////				PropellerPower.rightUp =   (PropellerDir.rightUp   * (PowerPercent) * ( rc->Fx) )/70 + ACC2 + PropellerError.rightUp;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
////				PropellerPower.leftDown =  (PropellerDir.leftDown  * (PowerPercent) * ( rc->Fx) )/70 + ACC3 + PropellerError.leftDown ; 
////				PropellerPower.rightDown = (PropellerDir.rightDown * (PowerPercent) * ( rc->Fy) )/70 + ACC4 + PropellerError.rightDown;
////				

//		}
>>>>>>> d7be8108aa196336e709695979ebaca59562c13a
		
		else if(AUV_Mode == VehicleMode){
				/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */ 
				PropellerPower.leftUp =    (PropellerDir.leftUp    * ((PowerPercent) * ( rc->X ) /70 ))	+ ACC1 + PropellerError.leftUp  ;  //����ֵΪ 10 PowerΪ�ƽ���ϵ��0~100%
				PropellerPower.rightUp =   (PropellerDir.rightUp   * ((PowerPercent) * ( rc->Y ) /70 )) + ACC2 + PropellerError.rightUp ;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
				PropellerPower.leftDown =  (PropellerDir.leftDown  * ((PowerPercent) * ( rc->X ) /70 )) + ACC3 + PropellerError.leftDown ; 
				PropellerPower.rightDown = (PropellerDir.rightDown * ((PowerPercent) * ( rc->Y ) /70 )) + ACC4 + PropellerError.rightDown;
			
		}
}
int16 LeftFlag = 0,RightFlag = 0 ;
float X_FLAG1 = 0,X_FLAG2 = 0,Y_FLAG1 = 0 ,Y_FLAG2 = 0;
float TurnFlag = 0;
void FourtAxis_RovControl(Rocker_Type *rc)
{
	
						//	�����ƽ��������ϵ��
	//  ת����ټ��ٱ���ϵ��

	if(rc->X>=0)					//���ң��X���ڵ���0���ж�Ϊǰ��,����ǰ��ϵ�������ƽ���X��������ϵ��
	{
		X_FLAG1 =1;//Direction.UP_P1;			
		X_FLAG2 = 1;//Direction.UP_P2;
	}
	else							//���Ҫ��XС��0���ж�Ϊ���ˣ���������ϵ�������ƽ���X��������ϵ��
	{
		X_FLAG1 = 1;//Direction.DOWN_P1;
		X_FLAG2 =1;// Direction.DOWN_P2;
	}
	if(abs(abs(rc->Y)-abs(rc->X))>=50&&abs(rc->X <= 50))   //���ң��X���С����Y��Զ����X��������Y����ϵ��,������ת��ϵ��
	{
		if(rc->Y >= 0 )					
		{
			Y_FLAG1  = 1;//Direction.LEFT_P;		//���ң��Y���ڵ���0���ж�Ϊ��ת,������תϵ�������ƽ���Y��������ϵ��
			Y_FLAG2  = 0;
			TurnFlag = 0;
		}
		else						//���ң��YС��0���ж�Ϊ��ת,������תϵ�������ƽ���Y��������ϵ��
		{
			Y_FLAG1  = 0;
			Y_FLAG2  = -1;//Direction.RIGHT_P;
			TurnFlag = 0;
		}
	}
	else							//X,Y��ֵ��С��ת��ϵ���ϴ�����ת��ϵ�����رշ���ϵ����		
	{
		Y_FLAG1   = 0;
		Y_FLAG2   = 0;
		TurnFlag  = (float)rc->X / 128;
	}
	
	LeftFlag  = X_FLAG1 * rc->X + ( TurnFlag * rc->Y /1.3f ) + Y_FLAG1 * rc->Y;		//�����ƽ������ϵ����X���ٶ�+ת��ϵ��*Y+Y�᷽��ϵ��*Y
	RightFlag = X_FLAG2 * rc->X - ( TurnFlag * rc->Y /1.3f ) + Y_FLAG2 * rc->Y;
	PropellerPower.leftDown = PropellerDir.leftDown*LeftFlag; 
	PropellerPower.rightDown = PropellerDir.rightDown*RightFlag;
	
}








/**
  * @brief  highSpeed Devices_Control(�����豸����)
  * @param  * parameter
  * @retval None
  * @notice 
  */
void control_highSpeed_thread_entry(void *parameter)//���ٿ����߳�
{
		
		rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�
		print_sensor_info();   //��ӡ������������Ϣ
		while(1)
		{
				Control_Cmd_Get(&ControlCmd); //���������ȡ ������λ������������ڴˡ�Important��

				if(UNLOCK == ControlCmd.All_Lock){ //�������
						Convert_RockerValue(&Rocker); //ң������ ת�� Ϊ�ƽ�������
						Focus_Zoom_Camera(&ControlCmd.Focus);//�佹�۽�����ͷ����
			
				}
<<<<<<< HEAD
				
				AUV_Depth_Control(&Rocker);  //AUV��ȿ���
				ROV_Depth_Control(&Rocker);  //ROV��ȿ���
=======
				FourtAxis_RovControl(&Rocker);
				AUV_Depth_Control(&Rocker);
				ROV_Depth_Control(&Rocker);
>>>>>>> d7be8108aa196336e709695979ebaca59562c13a
				ROV_Rotate_Control(&Rocker); //ROV��ת����
				
				Propeller_Control(); //�ƽ�����ʵPWM���

				rt_thread_mdelay(10);
		}

}

/**
  * @brief  lowSpeed Devices_Control(�����豸����)
  * @param  None
  * @retval None
  * @notice 
  */
void control_lowSpeed_thread_entry(void *parameter)//���ٿ����߳�
{

		rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�
		
		while(1)
		{

				Light_Control(&ControlCmd.Light);  //̽�յƿ���
				YunTai_Control(&ControlCmd.Yuntai); //��̨����
				RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���	
			
				rt_thread_mdelay(20);
		}
}



int control_thread_init(void)
{
		rt_thread_t control_lowSpeed_tid;
		rt_thread_t control_highSpeed_tid;
		/*������̬�߳�*/
    control_lowSpeed_tid = rt_thread_create("control_low",//�߳�����
                    control_lowSpeed_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

			/*������̬�߳�*/
    control_highSpeed_tid = rt_thread_create("control_high",//�߳�����
                    control_highSpeed_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms
	
    if (control_lowSpeed_tid != RT_NULL && control_highSpeed_tid != RT_NULL  ){
				rt_thread_startup(control_lowSpeed_tid);
				rt_thread_startup(control_highSpeed_tid);
				log_i("Control_Init()");
		}
		else {
				log_e("Control Error!");
		}
		return 0;
}
INIT_APP_EXPORT(control_thread_init);





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

