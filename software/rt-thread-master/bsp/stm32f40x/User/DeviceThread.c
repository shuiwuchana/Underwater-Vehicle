/*
 * DeviceThread.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �豸��������
 */

#include "DeviceThread.h"
#include <rtthread.h>
#include <elog.h>
#include "propeller.h"
#include "servo.h"
#include "light.h"
#include "rc_data.h"
#include "Control.h"
#include "PropellerControl.h"
/**
  * @brief  propeller_thread_entry(�ƽ�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void propeller_thread_entry(void *parameter)
{

		Propeller_Init();       //�ƽ�����ʼ��
		while(1)
		{
			
				Control_Cmd_Get(&ControlCmd); //���������ȡ ������λ������������ڴˡ�Important��

				if(UNLOCK == ControlCmd.All_Lock){ //�������
						Convert_RockerValue(&Rocker); //ң������ ת�� Ϊ�ƽ�������
					
				}
				
				if(FOUR_AXIS == VehicleMode){
						FourtAxis_Control(&Rocker);
				}
				else if(SIX_AXIS == VehicleMode){
						SixAxis_Control(&Rocker);
				}
				else{
						log_e("not yet set vehicle mode !");
				}
				
				//Propeller_Output(); //�ƽ�����ʵPWM���		
				rt_thread_mdelay(10);
		}
	
}

/**
  * @brief  servo_thread_entry(�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void servo_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{


		rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�
		
		while(1)
		{
				//Light_Control(&ControlCmd.Light);  //̽�յƿ���
				YunTai_Control(&ControlCmd.Yuntai); //��̨����
				RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���
				
				rt_thread_mdelay(20);
		}
	
}











