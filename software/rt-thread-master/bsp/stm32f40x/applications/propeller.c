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

PropellerParameter_Type PropellerParameter = {//��ʼ���ƽ�������ֵ���޷���
		 .PowerMax = 1750,//�������ֵ
		 .PowerMed = 1500,//��ֵ
		 .PowerMin = 1250,//������Сֵ�������������
	
	   .PowerDeadband = 10	//����ֵ
}; 

PropellerDir_Type    PropellerDir = {1,1,1,1,1,1};     //�ƽ�������Ĭ��Ϊ1
PropellerPower_Type  PropellerPower = {0,0,0,0,0,0,0}; //�ƽ�������������
PropellerError_Type  PropellerError = {0,0,0,0,0,0};   //�ƽ���ƫ��ֵ

PropellerPower_Type power_test; //�����õı���

PropellerError_Type Forward   = {0,0,0,0,0,0};
PropellerError_Type Retreat   = {0,0,0,0,0,0};
PropellerError_Type TurnLeft  = {0,0,0,0,0,0};
PropellerError_Type TurnRight = {0,0,0,0,0,0};
Adjust_Parameter AdjustParameter = {1,1,1,1};

extern int16 PowerPercent;
extern int Extractor_Value;//��ȡ���ƽ�����ֵ
/*----------------------- Function Implement --------------------------------*/

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

	
		rt_thread_mdelay(3000);//�ȴ��ⲿ�豸��ʼ���ɹ�
	
		TIM1_PWM_CH1_E9 (PropellerPower_Max);  		//���ת���ź�   	ˮƽ�ƽ���1��  ����	 E9	
		TIM1_PWM_CH2_E11(PropellerPower_Max);  		//���ת���ź�    ˮƽ�ƽ���2��  ����	 E11
		TIM1_PWM_CH3_E13(PropellerPower_Max); 	  //���ת���ź�    ˮƽ�ƽ���3��  ����   E13
		TIM1_PWM_CH4_E14(PropellerPower_Max);  		//���ת���ź�    ˮƽ�ƽ���4��  ����   E14
	
		TIM4_PWM_CH1_D12(PropellerPower_Max); 	 	//���ת���ź�  	��ֱ�ƽ���1��  ����   D12
		TIM4_PWM_CH2_D13(PropellerPower_Max);	    //���ת���ź�  	��ֱ�ƽ���2��  ����   D13
	
		TIM3_PWM_CH3_B0(PropellerPower_Max);	    //���ת���ź�  	��ȡ��   B0
	
	/**/
		//TIM4_PWM_CH3_D14(PropellerPower_Max);	    //���ת���ź�  	��ȡ��   B0
	/**/
		rt_thread_mdelay(2000);  //2s

		TIM1_PWM_CH1_E9 (PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH2_E11(PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH3_E13(PropellerPower_Med);			//ͣת�ź�
		TIM1_PWM_CH4_E14(PropellerPower_Med);			//ͣת�ź�
	
		TIM4_PWM_CH1_D12(PropellerPower_Med);		  //ͣת�ź�
		TIM4_PWM_CH2_D13(PropellerPower_Med);		  //ͣת�ź�
		
		TIM3_PWM_CH3_B0(PropellerPower_Med);	    //ͣת�ź�  	��ȡ��   B0
		
		TIM4_PWM_CH3_D14(1500);		//��е����ֵ 1000~2000
		TIM4_PWM_CH4_D15(2000);		//��̨��ֵ
		
		rt_thread_mdelay(1000);  //1s
		
		Propeller_Init_Flag = 1;
		log_i("Propeller_init()");


}


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



int power_value = 1500;
/**
  * @brief  Extractor_Control(��ȡ������)
  * @param  ����ָ�� 0x00��������  0x01����ȡ  0x02���ر�
  * @retval None
  * @notice 
  */
void Extractor_Control(uint8 *action)
{

		switch(*action)
		{
				case 0x01:power_value = Extractor_Value; //�趨����ֵ
									break;
				case 0x02:power_value = PropellerPower_Med; //�ƽ�����ֵ ͣת
									break;
				default:break;
		}

		//TIM4_PWM_CH3_D14(power_value);
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
        log_e("Error! Proper Usage: propeller_dir_set <1 1 1 1 1 1>  ");
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
				 
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //���־ֻ���� 1  or  -1 
				log_i("Propeller:    %d           %d          %d          %d            %d             %d",\
				 PropellerDir.rightUp,PropellerDir.leftDown,PropellerDir.leftUp,PropellerDir.rightDown,PropellerDir.leftMiddle ,PropellerDir.rightMiddle);
				Flash_Update();//FLASH����
				rt_kprintf("\n");

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



/*����ȡ�����ƽ��� �޸� �������� MSH���� */
static int extractor_value_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: extractor_value_set <1000~2000>");
				result = -RT_ERROR;
        goto _exit;
    }

		if(atoi(argv[1]) <= 2000 ){		
				Extractor_Value = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! extractor_value  %d",Extractor_Value);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(extractor_value_set,ag: extractor_value_set <1000~2000>);



uint8 is_in_range(short value)
{
		return abs(value) < 100?1:0;
}

/*���ƽ����� �޸� ��ƫ��ֵ��MSH���� */
static int propeller_error_set(int argc, char **argv){ 
    int result = 0;
    if (argc != 7){ //6���ƽ���
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //���־ֻ���� 1  or  -1 
        log_e("Error! Proper Usage: propeller_dir_set <-100~+100>  ");
				result = -RT_ERROR;
        goto _exit;
    }
		
		if(is_in_range(atoi(argv[1])) && is_in_range(atoi(argv[2]))  && is_in_range(atoi(argv[3]))   && \
			 is_in_range(atoi(argv[4])) && is_in_range(atoi(argv[5])) && is_in_range(atoi(argv[6]))  ) {
				 
				PropellerError.rightUp     = atoi(argv[1]);
				PropellerError.leftDown    = atoi(argv[2]);
				PropellerError.leftUp      = atoi(argv[3]);
				PropellerError.rightDown   = atoi(argv[4]);
				PropellerError.leftMiddle  = atoi(argv[5]);
				PropellerError.rightMiddle = atoi(argv[6]);
				 
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");
				log_i("Propeller:    %d           %d          %d          %d            %d             %d",\
				 PropellerError.rightUp,PropellerError.leftDown,PropellerError.leftUp,PropellerError.rightDown,PropellerError.leftMiddle ,PropellerError.rightMiddle);
				Flash_Update();//FLASH����
				rt_kprintf("\n");

		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_error_set,propeller_error_set <-100~+100>);
