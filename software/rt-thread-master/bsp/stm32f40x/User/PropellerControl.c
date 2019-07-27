/*
 * PropellerControl.c
 *
 *  Created on: 2019��3��20��
 *      Author: 219
 *      Notes:  �ƽ�������
 */
#include <math.h>
#include "propeller.h"
#include "PropellerControl.h"
#include "rc_data.h"
#include "drv_pwm.h"
#include <rtthread.h>
#include "PID.h"
#include "ret_data.h"
#include "Control.h"

float Expect_Depth = 0.0f;

extern int16 PowerPercent;

 /*******************************************
* �� �� ����AUV_Depth_Control
* ��    �ܣ�AUV��ȿ���
* ���������ҡ�˽ṹ��
* �� �� ֵ��none
* ע    �⣺none
********************************************/

void AUV_Depth_Control(Rocker_Type *rc)
{
		
		if(SIX_AXIS == VehicleMode){	 //AUV��ȿ���λ������
				switch(ControlCmd.Vertical){//�п������ݲ������
						case RiseUp: 
								 Expect_Depth-=3 ; 
								 if(Expect_Depth < 0) {Expect_Depth= 0;}//���������е����ֵ������ֵ��������
								 break;  //����
					
						case Dive:   
									if(Total_Controller.High_Position_Control.Control_OutPut < 450){ //���������Χ ֹͣ�ۻ�
											Expect_Depth+=3 ;
									}
									
								 break;  //��Ǳ
						default:break/*�����PID*/;
				}
		}

	
		Depth_PID_Control(Expect_Depth,Sensor.DepthSensor.Depth);//��ȿ��� ���ܡ��������ֽڿ���
}


/*******************************************
* �� �� ����ROV_Depth_Control
* ��    �ܣ�ROV��ȿ���
* ���������ҡ�˽ṹ��
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void ROV_Depth_Control(Rocker_Type *rc){

			/* ��ֱ���� */
		if(FOUR_AXIS == VehicleMode)	 //ROV�� ҡ��ģ��������
		{
				if(rc->Z > 5){
					 Expect_Depth -=( (float)rc->Z /100); 
					 if(Expect_Depth < 0) {//���������е����ֵ������ֵ��������
							Expect_Depth= 0;
						}
				}
				else if(rc->Z < -5){
						if(Total_Controller.High_Position_Control.Control_OutPut < 450){ //���������Χ ֹͣ�ۻ�
								Expect_Depth += (fabs((float)rc->Z)/100);
						}
				}
		}
		Depth_PID_Control(Expect_Depth,Sensor.DepthSensor.Depth);//��ȿ��� ���ܡ��������ֽڿ���
}


 /*******************************************
* �� �� ����ROV_Rotate_Control
* ��    �ܣ�ROVˮƽ��ת����
* ���������ҡ�˽ṹ��
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void ROV_Rotate_Control(Rocker_Type *rc){

			/* ��ֱ���� */
		if(FOUR_AXIS == VehicleMode)	 //ROV�� ҡ��ģ��������
		{
				if((rc->Force)< 5){ //��ҡ�����ȼ����� ��ҡ�� ���ȼ�
						if(rc->Yaw > 5){
							 turnRight(rc->Yaw);
						}
						else if(rc->Yaw < -5){
							 turnLeft(-rc->Yaw);
						}
				}
		}
		Depth_PID_Control(Expect_Depth,Sensor.DepthSensor.Depth);//��ȿ��� ���ܡ��������ֽڿ���
}


/*******************************************
* �� �� ����Output_Limit
* ��    �ܣ��ƽ����������
* ���������PowerValue
* �� �� ֵ���޷���� PowerValue
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ1800
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1300
********************************************/
//uint16 Output_Limit(int16 *PowerValue)
//{
//		//������+500   ������-500
//		*PowerValue = (*PowerValue) > (16000 - PropellerParameter.PowerMed ) ? (PropellerParameter.PowerMax - PropellerParameter.PowerMed ): *PowerValue ;//�����޷�
//		*PowerValue = (*PowerValue) < (PropellerParameter.PowerMin - PropellerParameter.PowerMed ) ? (PropellerParameter.PowerMin - PropellerParameter.PowerMed ): *PowerValue ;//�����޷�
//	
//		return *PowerValue ;
//}
uint16 Propeller_Output_Limit(int16 value)
{
		//������+500   ������-500
		value = (value) > 150  ? 150  : value ;//�����޷�
		value = (value) < -150 ? -150 : value;//�����޷�
	
		return value ;
}



/*******************************************
* �� �� ����Propeller_Output
* ��    �ܣ��ƽ����������
* ����������˶�ֵ��MoveValue
* �� �� ֵ��none
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ2000
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1000
********************************************/
void Propeller_Output(void)
{
		
		PropellerPower.rightUp = Propeller_Output_Limit(PropellerPower.rightUp); //PWM�޷�
	
		PropellerPower.leftUp = Propeller_Output_Limit(PropellerPower.leftUp);
	
		PropellerPower.rightDown = Propeller_Output_Limit(PropellerPower.rightDown);
	
		PropellerPower.leftDown = Propeller_Output_Limit(PropellerPower.leftDown);
	
//		PropellerPower.leftMiddle = Propeller_Output_Limit(PropellerPower.leftMiddle);
//	
//		PropellerPower.rightMiddle = Propeller_Output_Limit(PropellerPower.rightMiddle);
		
		PWM_Update(&PropellerPower);//PWMֵ����
	
}


/*******************************************
* �� �� ����turnRight
* ��    �ܣ�����������
* ���������none
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void turnRight(uint16 power)  //����
{
		power = ((PowerPercent) * ( power) )/70;
		PropellerPower.leftUp =     PropellerDir.leftUp*(power) + PropellerError.leftUp;
		PropellerPower.rightUp =    0 + PropellerError.rightUp;
		PropellerPower.leftDown =   PropellerDir.leftDown*(power) + PropellerError.leftDown;
		PropellerPower.rightDown =  0 + PropellerError.rightDown;
}



void turnLeft(uint16 power)  //����
{
		power = ((PowerPercent) * ( power) )/70;
		PropellerPower.leftUp =    0 + PropellerError.leftUp;
		PropellerPower.rightUp =   PropellerDir.rightUp*(power) + PropellerError.rightUp;
		PropellerPower.leftDown =  0 + PropellerError.leftDown;
		PropellerPower.rightDown = PropellerDir.rightDown*(power) + PropellerError.rightDown;
}



void Propller_stop(void)  //�ƽ���ͣת
{
		PropellerPower.leftUp =    0 + PropellerError.leftUp;
		PropellerPower.rightUp =   0 + PropellerError.rightUp;
		PropellerPower.leftDown =  0 + PropellerError.leftDown;
		PropellerPower.rightDown = 0 + PropellerError.rightDown;
	
		PropellerPower.leftMiddle = 0 + PropellerError.leftMiddle;
		PropellerPower.rightMiddle = 0+ PropellerError.rightMiddle; 
}
MSH_CMD_EXPORT(Propller_stop,ag: propller_stop);


/*******************************************
* �� �� ����Propeller_upDown
* ��    �ܣ��ƽ����������½�
* ���������depth�������
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void robot_upDown(float depth_output)  
{
		//�޷� �������ƽ��� �趨���������ֵ-ͣתֵ(��ֵ)
	
		depth_output = depth_output < -(PropellerParameter.PowerMax - PropellerParameter.PowerMed ) ? -(PropellerParameter.PowerMax - PropellerParameter.PowerMed ):depth_output;
		depth_output = depth_output >  (PropellerParameter.PowerMax - PropellerParameter.PowerMed ) ?  (PropellerParameter.PowerMax - PropellerParameter.PowerMed ):depth_output;
		
		PropellerPower.leftMiddle   =  PropellerDir.leftMiddle  * ( -depth_output + PropellerError.leftMiddle);//������
		PropellerPower.rightMiddle  =  PropellerDir.rightMiddle * ( -depth_output + PropellerError.rightMiddle);//���Ϊ��ֵ
		
		if(FOUR_AXIS == VehicleMode){ //�����Ϊ��ƽ����������(��Ϊ�����ƽ�����������������һ��)

				
		}
		
		else if(SIX_AXIS == VehicleMode) //�����Ϊ�˲����ƽ�������ֵ
		{
//			  PropellerPower.leftMiddle  -= (PropellerDir.leftMiddle  * 20);//����ֵ 20
//				PropellerPower.rightMiddle -= (PropellerDir.rightMiddle * 20);	
		}
}
