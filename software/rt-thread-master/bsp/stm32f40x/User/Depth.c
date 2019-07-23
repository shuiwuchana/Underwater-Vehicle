/*
 * Control.c
 *
 *  Created on: 2019��7��15��
 *      Author: zengwangfa
 *      Notes:  ��ȿ���
 */
 
 
 
#include "Depth.h"
#include <math.h>
#include "propeller.h"
#include "PropellerControl.h"
#include "RC_Data.h"
#include "drv_pwm.h"
#include "PID.h"
#include "Control.h"
 
 /*******************************************
* �� �� ����AUV_Depth_Control
* ��    �ܣ�AUV��ȿ���
* ���������ҡ�˽ṹ��
* �� �� ֵ��none
* ע    �⣺none
********************************************/

void AUV_Depth_Control(Rocker_Type *rc)
{
		
		if(AUV_Mode == VehicleMode){	 //AUV��ȿ���λ������
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
				//ControlCmd.Vertical = 0x00;
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
		if(ROV_Mode == VehicleMode)	 //ROV�� ҡ��ģ��������
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
				//ControlCmd.Vertical = 0x00;
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
		if(ROV_Mode == VehicleMode)	 //ROV�� ҡ��ģ��������
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
