/*
 * focus.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �佹����ͷ�豸(����ͨ��)
 */

#include "focus.h"
#include <rtthread.h>
#include <elog.h>
#include "uart.h"
#include "ret_data.h"
#include "Control.h"
/*---------------------- Constant / Macro Definitions -----------------------*/


/*----------------------- Variable Declarations -----------------------------*/
uint8 focus_data_ok = 0;

uint8 addFocus_Data[6] = {0xAA,0x55,0x02,0x01,0x00,0x02}; //�Ŵ�
uint8 minFocus_Data[6] = {0xAA,0x55,0x02,0x02,0x00,0x03}; //��С

uint8 addZoom_Data[6]  = {0xAA,0x55,0x02,0x00,0x01,0x02}; //�۽�
uint8 minZoom_Data[6]  = {0xAA,0x55,0x02,0x00,0x02,0x03}; //�Ž�

uint8 Camera_Clear_Data[6] = {0xAA,0x55,0x02,0x88,0x88,0x11}; //�ָ���ʼֵ
uint8 Camera_Stop_Data[6]  = {0xAA,0x55,0x02,0x00,0x00,0x01}; //ֹͣ
/*----------------------- Function Implement --------------------------------*/
float Adjust1= 1,Adjust2 = 1;
uint8 focus_data[10] = {0};
/**
  * @brief  Focus_Zoom_Camera(����ͷ�佹���Ŵ�)
  * @param  �����ַ�����action [0x01�۽���0x02�Ž���0x11�Ŵ�0x12��С]
  * @retval None
  * @notice 
  */
void Focus_Zoom_Camera(uint8 *action)
{		
		switch(*action){
				case 0x01:Adjust1 += 0.01f;break;         //rt_device_write(focus_uart_device, 0,addFocus_Data,   6); break;
				case 0x02:Adjust1 -= 0.01f;break;         //rt_device_write(focus_uart_device, 0,minFocus_Data,   6); break;
				case 0x11:Adjust2 += 0.01f;break;         //rt_device_write(focus_uart_device, 0,addZoom_Data,    6); break;
				case 0x12:Adjust2 -= 0.01f;break;    	 //rt_device_write(focus_uart_device, 0,minZoom_Data,    6); break;
				case 0x88:Adjust1 = 1,Adjust2 = 1;break; //rt_device_write(focus_uart_device, 0,Camera_Clear_Data,    6); break; //�ָ���ʼֵ
				default  :break; //����Ϊ�������� ֹͣ����
		}
		*action = 0x00;
}


/**
  * @brief  Camera_Focus_Data_Analysis(�佹����ͷ�������ݽ���)
  * @param  �����ַ����� uint8 Data
  * @retval None
  * @notice �ӵ��ĸ��ֽڿ�ʼΪ�����ַ�
  */
void Camera_Focus_Data_Analysis(uint8 Data) //�������ݽ���
{
		static uint8 RxCheck = 0;	  //βУ����
		static uint8 RxCount = 0;	  //���ռ���
		static uint8 i = 0;	   		  //

		focus_data[RxCount++] = Data;	//���յ������ݴ��뻺������
	
		if(focus_data[0] == 0xAA){ //���յ���ͷ0xAA
				if(RxCount > 3){
						if(focus_data[1] == 0x55){ //���յ���ͷ0x55
								if(RxCount >= focus_data[2]+4){ //���յ����ݰ�����λ����ʼ�ж�ʲôʱ��ʼ����У��
										for(i = 0;i <= (RxCount-2);i++){ //�ۼӺ�У��
												RxCheck += focus_data[i];
										}
										if(RxCheck == focus_data[RxCount-1]){
												focus_data_ok = 1; //�������ݰ��ɹ�
										}
										else {focus_data_ok = 0;}
										
										RxCheck = 0; //�����������
										RxCount = 0;	
								}
						}
						else {focus_data_ok = 0;RxCount = 0;return;} //���ղ��ɹ�����
				}
		}
		else {focus_data_ok = 0;RxCount = 0;return;} //���ղ��ɹ�����
		
		if(focus_data_ok){ // ��ʾ�ַ� ��־����
			
				if(focus_data[3] == 0xFF ){
						device_hint_flag |= 0x04;
				}
				else {
						device_hint_flag &= 0xFB;
				}
				if(focus_data[4] == 0xFF ){
						device_hint_flag |= 0x08;
				}
				else {
						device_hint_flag &= 0xF7;
				}
		}

}

/* ���� �ָ���ʼֵ */
void focus_camera_clear(void)
{
		rt_device_write(focus_uart_device, 0,Camera_Clear_Data,    6);
		log_i("focus_camera_clear... ");
}
MSH_CMD_EXPORT(focus_camera_clear,focus camera clear);


/* ���� �Ŵ� */
void focus_camera_add(void)
{
		static int *p;
		*p = 0x0l;
		Focus_Zoom_Camera((uint8 *)p);
		log_i("focus_camera_add ");
}
MSH_CMD_EXPORT(focus_camera_add,focus camera add);

