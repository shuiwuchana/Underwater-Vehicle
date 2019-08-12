/*
 * timer.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ��ʱ��
 */
#define LOG_TAG    "timer"

#include <string.h>
#include "timer.h"
#include "flash.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include "Control.h"
#include "gyroscope.h"
#include "debug.h"
#include "ret_data.h"
#include "PropellerControl.h"
/*---------------------- Constant / Macro Definitions -----------------------*/



/*----------------------- Variable Declarations -----------------------------*/




/*----------------------- Function Implement --------------------------------*/


/**
  * @brief  timer1_out(��ʱ���жϺ���)
  * @param  void* parameter
  * @retval None
  * @notice Attention! -> ��ʱ���в��ܴ�����ʱ�����ͷ��̵߳Ĳ���
  */
static void timer1_out(void* parameter)// ��ʱ��1��ʱ����  ����JY901ģ������ת��
{


		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
				

	
		get_speed(&Sensor.JY901.Acc.x,&Sensor.JY901.Speed.x);//�õ�x�ٶ�,���������⣬�ۻ�ЧӦ����
		get_zspeed(); 
		//Angle_Control(); //�Ƕȿ���


		/* ���������� */
		rt_exit_critical();

	
}



int timer1_init(void)
{
		/* ��ʱ���Ŀ��ƿ� */
		static rt_timer_t timer1;
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1",  /* ��ʱ�������� timer1 */
                        timer1_out, 		  /* ��ʱʱ�ص��Ĵ����� */
                        RT_NULL, 			  /* ��ʱ��������ڲ��� */
                        10,      			  /* ��ʱ���ȣ���OS TickΪ��λ����5��OS Tick   --> 50MS*/  
                        RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_HARD_TIMER); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL){ 
				
				rt_timer_start(timer1);
				
		}

    return 0;
}
//INIT_APP_EXPORT(timer1_init);









