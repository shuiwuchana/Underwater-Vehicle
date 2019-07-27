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
#include "sys.h"


int propeller_thread_init(void)
{
    rt_thread_t propeller_tid;
		/*������̬�߳�*/
    propeller_tid = rt_thread_create("propoller",//�߳�����
                    propeller_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (propeller_tid != RT_NULL){

				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  ������Ϊ500Hz��
				TIM3_PWM_Init(20000-1,84-1);  //��ȡ��
				TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
				TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
				rt_thread_startup(propeller_tid);
		}

		return 0;
}
INIT_APP_EXPORT(propeller_thread_init);
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
				
				Propeller_Output(); //�ƽ�����ʵPWM���		
				rt_thread_mdelay(10);
		}
	
}












int servo_thread_init(void)
{
    rt_thread_t servo_tid;
		/*������̬�߳�*/
    servo_tid = rt_thread_create("servo",//�߳�����
                    servo_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    15,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (servo_tid != RT_NULL){

				TIM4_PWM_Init(20000-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  

				log_i("Servo_init()");
				rt_thread_startup(servo_tid);
				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(servo_thread_init);
/**
  * @brief  servo_thread_entry(�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void servo_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{


		//rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�

		while(1)
		{
				//Light_Control(&ControlCmd.Light);  //̽�յƿ���
				YunTai_Control(&ControlCmd.Yuntai); //��̨����
				RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���
				
				rt_thread_mdelay(20);
		}
	
}











