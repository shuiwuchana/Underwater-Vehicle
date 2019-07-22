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
#include "RC_Data.h"

#define Propeller_MedValue 1500

extern int16 PowerPercent;

uint8 Propeller_Init_Flag = 0;
PropellerParameter_Type PropellerParameter = {//��ʼ���ƽ�������ֵ
		 .PowerMax = 2000,//�������ֵ
		 .PowerMed = 1500,//��ֵ
		 .PowerMin = 1000,//������Сֵ�������������
	
	   .PowerDeadband = 10	//����ֵ
}; 

ActionType_Enum      Posture_Flag;                     //��������̬��־λ
PropellerDir_Type    PropellerDir = {1,1,1,1,1,1};     //�ƽ�������Ĭ��Ϊ1
PropellerPower_Type  PropellerPower = {0,0,0,0,0,0,0}; //�ƽ�������������
PropellerError_Type  PropellerError = {0,0,0,0,0,0};   //�ƽ���ƫ��ֵ


PropellerPower_Type power_test; //�����õı���

void PWM_Update(PropellerPower_Type* propeller)
{	
		power_test.rightUp     = Propeller_MedValue + propeller->rightUp;
		power_test.leftDown    = Propeller_MedValue + propeller->leftDown;
		power_test.leftUp      = Propeller_MedValue + propeller->leftUp;
		power_test.rightDown   = Propeller_MedValue + propeller->rightDown;
		
		power_test.leftMiddle  = Propeller_MedValue + propeller->leftMiddle;
		power_test.rightMiddle = Propeller_MedValue + propeller->rightMiddle;
	
		if(1 == Propeller_Init_Flag){
				
				TIM_SetCompare1(TIM1,power_test.rightUp);     //����	 E9	
				TIM_SetCompare2(TIM1,power_test.leftDown);    //����	 E11
				TIM_SetCompare3(TIM1,power_test.leftUp); 	    //����   E13
				TIM_SetCompare4(TIM1,power_test.rightDown);   //����   E14
			
				TIM_SetCompare1(TIM4,power_test.leftMiddle);  //����   D12
				TIM_SetCompare2(TIM4,power_test.rightMiddle); //����   D13
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

						power_test_msh.rightUp     = Propeller_MedValue + atoi(argv[1]);
						power_test_msh.leftDown    = Propeller_MedValue + atoi(argv[1]);
						power_test_msh.leftUp      = Propeller_MedValue + atoi(argv[1]);
						power_test_msh.rightDown   = Propeller_MedValue + atoi(argv[1]);
						
						power_test_msh.leftMiddle  = Propeller_MedValue + atoi(argv[1]);
						power_test_msh.rightMiddle = Propeller_MedValue + atoi(argv[1]);
					
						TIM_SetCompare1(TIM1,power_test_msh.rightUp);     //����	 E9	
						TIM_SetCompare2(TIM1,power_test_msh.leftDown);    //����	 E11
						TIM_SetCompare3(TIM1,power_test_msh.leftUp); 	    //����   E13
						TIM_SetCompare4(TIM1,power_test_msh.rightDown);   //����   E14
					
						TIM_SetCompare1(TIM4,power_test_msh.leftMiddle);  //����   D12
						TIM_SetCompare2(TIM4,power_test_msh.rightMiddle); //����   D13
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
		TIM_SetCompare1(TIM1, 2000);  		//���ת���ź�   	ˮƽ�ƽ���1��
		TIM_SetCompare2(TIM1, 2000);  		//���ת���ź�    ˮƽ�ƽ���2��
		TIM_SetCompare3(TIM1, 2000); 		  //���ת���ź�    ˮƽ�ƽ���3��
		TIM_SetCompare4(TIM1, 2000);  		//���ת���ź�    ˮƽ�ƽ���4��
	
		TIM_SetCompare1(TIM4, 2000); 	 	//���ת���ź�  	��ֱ�ƽ���1��
		TIM_SetCompare2(TIM4, 2000);	  //���ת���ź�  	��ֱ�ƽ���2��

		//TIM_SetCompare3(TIM4, 1900);		//��ֵ
		//TIM_SetCompare4(TIM4, 1700);		//����	

		rt_thread_mdelay(2000);  //2s


		TIM_SetCompare1(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare2(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare3(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare4(TIM1, 1500);			//ͣת�ź�
	
		TIM_SetCompare1(TIM4, 1500);		  //ͣת�ź�
		TIM_SetCompare2(TIM4, 1500);		  //ͣת�ź�

		TIM_SetCompare3(TIM4, 1900);		//��ֵ
		//TIM_SetCompare4(TIM4, 1000);		//����
		rt_thread_mdelay(1000);  //1s
		
	  log_i("Propeller_Init()");
		Propeller_Init_Flag = 1;
}


/*���ƽ����� ��Stop��MSH���� */
void Propeller_Stop(void)
{
		memset(&PropellerPower,0,sizeof(PropellerPower));

}
MSH_CMD_EXPORT(Propeller_Stop,ag: Propeller Stop);



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
        log_e("Error! Proper Usage: propeller_dir_set 1 1 1 1 1 1 ");
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
				
				Flash_Update();
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


