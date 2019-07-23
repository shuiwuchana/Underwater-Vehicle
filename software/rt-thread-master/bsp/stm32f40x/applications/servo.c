/*
 * servo.c
 *
 *  Created on: 2019��3��30��
 *      Author: zengwangfa
 *      Notes:  ����豸
 */
#define  LOG_TAG    "servo"
#include <stdio.h>
#include <stdlib.h>
#include <rtthread.h>
#include <elog.h>

#include "sys.h"

#include "DataType.h"
#include "Control.h"
#include "ioDevices.h"
#include "servo.h"
#include "propeller.h"
#include "flash.h"
#include "Return_Data.h"
#include "RC_Data.h"
#include "focus.h"

#define RoboticArm_MedValue  1500
#define YunTai_MedValue  		 2000


ServoType RoboticArm = {
		 .MaxValue = 2000, 		//��е�� �������ֵ
		 .MinValue = 1500,	  //��е�� ����
		 .MedValue = 1900,
	   .Speed  = 5//��е�۵�ǰֵ
};  //��е��
ServoType  YunTai = {
		 .MaxValue = 1700, 		//��е�� �������ֵ
		 .MinValue = 1000,	  //��е�� ����
		 .MedValue = 1300,
	   .Speed  = 10//��̨ת���ٶ�
};      //��̨

uint16 propeller_power = 1500;

extern float Adjust1,Adjust2 ;
extern struct rt_event init_event;/* ALL_init �¼����ƿ�. */
/*******************************************
* �� �� ����Servo_Output_Limit
* ��    �ܣ�����������
* �������������ֵ������ṹ���ַ 
* �� �� ֵ��None
* ע    �⣺
********************************************/
void Servo_Output_Limit(ServoType *Servo)
{
		Servo->CurrentValue = Servo->CurrentValue  > Servo->MaxValue ? Servo->MaxValue : Servo->CurrentValue ;//�����޷�
		Servo->CurrentValue = Servo->CurrentValue  < Servo->MinValue ? Servo->MinValue : Servo->CurrentValue ;//�����޷�
	
}


/**
  * @brief  RoboticArm_Control(��е�ۿ���)
  * @param  ����ָ�� 0x00��������  0x01���ſ�  0x02���ر�
  * @retval None
  * @notice 
  */
void RoboticArm_Control(uint8 *action)
{
		switch(*action)
		{
				case 0x01:RoboticArm.CurrentValue += RoboticArm.Speed;
									if(RoboticArm.CurrentValue >= RoboticArm.MaxValue){device_hint_flag |= 0x01;}//��е�۵�ͷ��־
									else {device_hint_flag &= 0xFE;}; //�����е�۵�ͷ��־

									break;
				case 0x02:RoboticArm.CurrentValue -= RoboticArm.Speed;
									if(RoboticArm.CurrentValue <= RoboticArm.MinValue){device_hint_flag |= 0x01;}//��е�۵�ͷ��־
									else {device_hint_flag &= 0xFE;}; //�����е�۵�ͷ��־

									break;
				default:break;
		}
		Servo_Output_Limit(&RoboticArm);//��е�۶���޷�
		TIM_SetCompare3(TIM4,RoboticArm.CurrentValue);
		*action = 0x00; //���������
}


/**
  * @brief  YunTai_Control(��̨����)
  * @param  ����ָ�� 0x00��������  0x01������  0x02������
  * @retval None
  * @notice 
  */

void YunTai_Control(uint8 *action)
{		
		static int DirectionMode = 1;
		
		switch(*action)
		{
				case 0x01:DirectionMode++;
						  DirectionMode = DirectionMode<=DirectionMode_MAX?DirectionMode:1;	
						  Buzzer_Set(&Beep,1,1);			
						break;  
						
				case 0x02:DirectionProportion(DirectionMode);
						break;  

				case 0x03:DirectionMode = 0;break;   //����
				default: break;
		}
		Servo_Output_Limit(&YunTai);
		TIM_SetCompare4(TIM4,YunTai.CurrentValue); 
		*action = 0x00; //���������
}

void DirectionProportion(int Mode)
{
	switch(Mode)
	{
		case DirectionUp   :Direction.UP_P1 = Adjust1;
							Direction.UP_P2 = Adjust2;	
							break;
		case DirectionDown :Direction.DOWN_P1 = Adjust1;
						    Direction.DOWN_P2 = Adjust2;
							break;
		case DirectionLeft :Direction.LEFT_P = Adjust1;
		case DirectionRight:Direction.RIGHT_P = Adjust1;
		default: break;
	}
}

//void YunTai_Control(uint8 *action)
//{
//		switch(*action)
//		{
//				case 0x01:YunTai.CurrentValue += YunTai.Speed;  //����
//						if(YunTai.CurrentValue <= YunTai.MaxValue){device_hint_flag |= 0x02;}//��̨��ͷ��־
//						else {device_hint_flag &= 0xFD;}; //�����̨��ͷ��־

//						break;  
//						
//				case 0x02:YunTai.CurrentValue -= YunTai.Speed; //����
//						if(YunTai.CurrentValue >= YunTai.MinValue){device_hint_flag |= 0x02;}//��̨��ͷ��־
//						else {device_hint_flag &= 0xFD;}; //�����̨��ͷ��־

//						break;  

//				case 0x03:YunTai.CurrentValue = YunTai.MedValue;break;   //����
//				default: break;
//		}
//		Servo_Output_Limit(&YunTai);
//		TIM_SetCompare4(TIM4,YunTai.CurrentValue); 
//		*action = 0x00; //���������
//}



/**
  * @brief  servo_thread_entry(�����ʼ��������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void servo_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{
		TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  ������Ϊ500Hz��
		TIM4_PWM_Init(20000-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
		TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
		Propeller_Init();       //�ƽ�����ʼ��
	
		rt_thread_mdelay(100);
	
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

				log_i("Servo_init()");
			
				rt_thread_startup(servo_tid);
				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(servo_thread_init);











/*����е�ۡ���� �޸� �ٶ�ֵ */
static int robotic_arm_speed_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_Speed 10");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 255 && atoi(argv[1]) > 0){
				RoboticArm.Speed = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! RoboticArm.Speed:  %d",RoboticArm.Speed);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_speed_set,ag: robotic_arm_speed_set 10);


/*����е�ۡ���� �޸� ���������ֵ��MSH���� */
static int robotic_arm_maxValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_Maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000 && atoi(argv[1]) >= 1500){
				RoboticArm.MaxValue = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed!  RoboticArm.MaxValue:  %d",RoboticArm.MaxValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_maxValue_set,ag: robotic_arm_openvalue_set 2000);




/*����е�ۡ���� �޸� ���������ֵ�� MSH���� */
static int robotic_arm_minValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_minvalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000 &&  atoi(argv[1]) >= 500){
				RoboticArm.MinValue = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed!  RoboticArm.MinValue:  %d",RoboticArm.MinValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

		
		
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_minValue_set,ag: robotic_arm_closevalue_set 1500);


/*����е�ۡ���� �޸� �ٶ�ֵ */
static int yuntai_speed_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: yuntai_speed_set 5");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 255 && atoi(argv[1]) > 0){
				YunTai.Speed = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! YunTai.Speed:  %d",YunTai.Speed);
		}
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_speed_set,ag: yuntai_speed_set 5);

/*����̨����� �޸� ���������ֵ��MSH���� */
static int yuntai_maxValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000){
				YunTai.MaxValue = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! YunTai.MaxValue:  %d",YunTai.MaxValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_maxValue_set,ag: yuntai_maxvalue_set 2500);




/*����̨����� �޸� ���������ֵ�� MSH���� */
static int yuntai_minValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_minvalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				YunTai.MinValue = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! YunTai.MinValue:  %d",YunTai.MinValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_minValue_set,ag: yuntai_arm_closevalue_set 1500);

/*����̨����� �޸� ���������ֵ�� MSH���� */
static int yuntai_medValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_medvalue_set 2000");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				YunTai.MedValue = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! YunTai.MedValue):  %d",YunTai.MedValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_medValue_set,ag: yuntai_arm_medvalue_set 2000);



/*����̨����� �޸� ����ǰ�� MSH���� */
static int yuntai_currentValue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_medvalue_set 2000");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000 && atoi(argv[1]) >= 500){
				YunTai.CurrentValue = atoi(argv[1]);
				log_i("Write_Successed! Current YunTai.CurrentValue:  %d",YunTai.CurrentValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_currentValue_set,ag: yuntai_currentValue_set 1500);


/*����̨����� �޸� ����ǰ�� MSH���� */
static int yuntai(int argc, char **argv)
{
    int result = 0;
		ServoType servo;
    if (argc > 1){
        log_e("Error! Proper Usage: YunTai_medvalue_set 2000");
				result = -RT_ERROR;
        goto _exit;
    }
		rt_kprintf("Set Min Value:");
		scanf("%d",(int *)(&servo.MinValue));

		if(servo.MinValue <= 7000 ){		
				log_i("Write_Successed! servo.MinValue  %d",servo.MinValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai,ag: yuntai_currentValue_set 1500);





