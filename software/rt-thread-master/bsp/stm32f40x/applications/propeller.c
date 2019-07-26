/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �ƽ����豸
 */
 #define LOG_TAG    "propeller"
 
 
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "propeller.h"
#include <elog.h>
#include <rtthread.h>

#include "flash.h"
#include "rc_data.h"
#include "drv_pwm.h"
#include "DeviceThread.h"

/*----------------------- Variable Declarations -----------------------------*/

uint8 Propeller_Init_Flag = 0;

PropellerParameter_Type PropellerParameter = {//��ʼ���ƽ�������ֵ
		 .PowerMax = 2000,//�������ֵ
		 .PowerMed = 1500,//��ֵ
		 .PowerMin = 1000,//������Сֵ�������������
	
	   .PowerDeadband = 10	//����ֵ
}; 

PropellerDir_Type    PropellerDir = {1,1,1,1,1,1};     //�ƽ�������Ĭ��Ϊ1
PropellerPower_Type  PropellerPower = {0,0,0,0,0,0,0}; //�ƽ�������������
PropellerError_Type  PropellerError = {0,0,0,0,0,0};   //�ƽ���ƫ��ֵ

PropellerPower_Type power_test; //�����õı���

extern int16 PowerPercent;

/*----------------------- Function Implement --------------------------------*/

int propeller_thread_init(void)
{
    rt_thread_t servo_tid;
		/*������̬�߳�*/
    servo_tid = rt_thread_create("propoller",//�߳�����
                    propeller_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (servo_tid != RT_NULL){

				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  ������Ϊ500Hz��
				TIM3_PWM_Init(20000-1,84-1);  //��ȡ��

				rt_thread_startup(servo_tid);

		}

		return 0;
}
INIT_APP_EXPORT(propeller_thread_init);


void PWM_Update(PropellerPower_Type* propeller)
{	
		power_test.rightUp     = PropellerPower_Med + propeller->rightUp;
		power_test.leftDown    = PropellerPower_Med + propeller->leftDown;
		power_test.leftUp      = PropellerPower_Med + propeller->leftUp;
		power_test.rightDown   = PropellerPower_Med + propeller->rightDown;
		
		power_test.leftMiddle  = PropellerPower_Med + propeller->leftMiddle;
		power_test.rightMiddle = PropellerPower_Med + propeller->rightMiddle;
	
		if(1 == Propeller_Init_Flag){
				
				TIM1_PWM_CH1_E9 (power_test.rightUp);     //����	 E9	
				TIM1_PWM_CH2_E11(power_test.leftDown);    //����	 E11
				TIM1_PWM_CH3_E13(power_test.leftUp); 	    //����   E13
				TIM1_PWM_CH4_E14(power_test.rightDown);   //����   E14
			
				TIM4_PWM_CH1_D12(power_test.leftMiddle);  //����   D12
				TIM4_PWM_CH2_D13(power_test.rightMiddle); //����   D13
		}

}


PropellerPower_Type power_test_msh; //�����õı���

/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int Propoller_Test(int argc, char **argv)
{

    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: Propoller_Test <0~100>");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 100){
				if(1 == Propeller_Init_Flag){

						power_test_msh.rightUp     = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.leftDown    = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.leftUp      = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.rightDown   = PropellerPower_Med + atoi(argv[1]);
						
						power_test_msh.leftMiddle  = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.rightMiddle = PropellerPower_Med + atoi(argv[1]);
					
						TIM1_PWM_CH1_E9 (power_test_msh.rightUp);      //����	 E9	
						TIM1_PWM_CH2_E11(power_test_msh.leftDown);    //����	 E11
						TIM1_PWM_CH3_E13(power_test_msh.leftUp); 	   //����   E13
						TIM1_PWM_CH4_E14(power_test_msh.rightDown);   //����   E14
					
						TIM4_PWM_CH1_D12(power_test_msh.leftMiddle);  //����   D12
						TIM4_PWM_CH2_D13(power_test_msh.rightMiddle); //����   D13
				}
				log_i("Current propeller power:  %d",atoi(argv[1]) );
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(Propoller_Test,ag: Propoller_Test <0~100>);



/*******************************************
* �� �� ����Propeller_Init
* ��    �ܣ��ƽ����ĳ�ʼ��
* ���������none
* �� �� ֵ��none
* ע    �⣺��ʼ�����̣�
*           1,����,�ϵ磬��-��-������,��ʾ��������
*           2,�����2ms��1ms���ת���ź�,��һ��
*           3,�����1.5msͣת�ź�,��һ��
*           4,��ʼ����ɣ����Կ�ʼ����
********************************************/
void Propeller_Init(void)//��߶���Ҫ�����޷��ڸ���  ԭ��Ϊ2000->1500
{
		rt_thread_mdelay(1000);//�ȴ��ⲿ�豸��ʼ���ɹ�
	
		TIM1_PWM_CH1_E9 (PropellerPower_Max);  		//���ת���ź�   	ˮƽ�ƽ���1��  ����	 E9	
		TIM1_PWM_CH2_E11(PropellerPower_Max);  		//���ת���ź�    ˮƽ�ƽ���2��  ����	 E11
		TIM1_PWM_CH3_E13(PropellerPower_Max); 	  //���ת���ź�    ˮƽ�ƽ���3��  ����   E13
		TIM1_PWM_CH4_E14(PropellerPower_Max);  		//���ת���ź�    ˮƽ�ƽ���4��  ����   E14
	
		TIM4_PWM_CH1_D12(PropellerPower_Max); 	 	//���ת���ź�  	��ֱ�ƽ���1��  ����   D12
		TIM4_PWM_CH2_D13(PropellerPower_Max);	    //���ת���ź�  	��ֱ�ƽ���2��  ����   D13

		rt_thread_mdelay(2000);  //2s

		TIM1_PWM_CH1_E9 (PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH2_E11(PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH3_E13(PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH4_E14(PropellerPower_Med);			//ͣת�ź�
	
		TIM4_PWM_CH1_D12(PropellerPower_Med);		  //ͣת�ź�
		TIM4_PWM_CH2_D13(PropellerPower_Med);		  //ͣת�ź�

		TIM4_PWM_CH3_D14(1500);		//��е����ֵ 1000~2000
		TIM4_PWM_CH4_D15(2000);		//��̨��ֵ
		
		rt_thread_mdelay(1000);  //1s
		
		Propeller_Init_Flag = 1;
		log_i("Propoller_init()");
}



/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_maxvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 2000){
				PropellerParameter.PowerMax = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller max_value_set:  %d",PropellerParameter.PowerMax);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_maxvalue_set,ag: propeller set 1600);


/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_medvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_medvalue_set 1500");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMed = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller med_value_set:  %d",PropellerParameter.PowerMed);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_medvalue_set,ag: propeller set 1500);

/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_minvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_minvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMin = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller min_value_set:  %d",PropellerParameter.PowerMin);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_minvalue_set,ag: propeller set 1200);



/*���ƽ����� �޸� ������MSH���� */
static int propeller_dir_set(int argc, char **argv) //ֻ���� -1 or 1
{
    int result = 0;
    if (argc != 7){ //6���ƽ���
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //���־ֻ���� 1  or  -1 
        log_e("Error! Proper Usage: propeller_dir_set 1 1 1 1 1 1 or propeller_dir_set -1 -1 -1 -1 -1 -1 ");
				result = -RT_ERROR;
        goto _exit;
    }
		
		if(abs(atoi(argv[1])) == 1 && abs(atoi(argv[2])) == 1  && abs(atoi(argv[3])) == 1  && \
			 abs(atoi(argv[4])) == 1  && abs(atoi(argv[5])) == 1  && abs(atoi(argv[6])) == 1  ) {
				 
				PropellerDir.rightUp     = atoi(argv[1]);
				PropellerDir.leftDown    = atoi(argv[2]);
				PropellerDir.leftUp      = atoi(argv[3]);
				PropellerDir.rightDown   = atoi(argv[4]);
				PropellerDir.leftMiddle  = atoi(argv[5]);
				PropellerDir.rightMiddle = atoi(argv[6]);
				
				Flash_Update();//FLASH����
				rt_kprintf("\n");
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //���־ֻ���� 1  or  -1 
				log_i("Propeller:    %d           %d          %d          %d            %d             %d",\
				 atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6]));
		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_dir_set,propeller <1 1 1 1 1 1>);


/*���ƽ����� �޸� ��������MSH���� */
static int propeller_power_set(int argc, char **argv) //ֻ���� 0~3.0f
{
    int result = 0;
    if (argc != 2){ //6���ƽ���
        log_e("Error! Proper Usage: propeller_power_set <0~300> % ");
				result = -RT_ERROR;
        goto _exit;
    }
		
	  if( atoi(argv[1]) >=0 && atoi(argv[1]) <=300  ) {
				 
				PowerPercent = atoi(argv[1]); //�ٷ���
				Flash_Update();

				log_i("Propeller_Power: %d %%",PowerPercent);
		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_power_set,propeller_power_set <0~300> %);


