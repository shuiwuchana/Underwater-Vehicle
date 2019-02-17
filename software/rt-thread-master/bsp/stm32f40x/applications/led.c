#include "init.h"

/*---------------------- Constant / Macro Definitions -----------------------*/
//RGB�����ź�
#define LED_Red 		68 
#define LED_Green 	69
#define LED_Blue 		70

//RGB�ƹ������ӵ�+3.3V����ƽ 0�� 1��
#define LED_ON(led_pin) 						rt_pin_write(led_pin ,PIN_LOW )
#define LED_OFF(led_pin) 						rt_pin_write(led_pin ,PIN_HIGH)
#define LED_Turn(led_pin,status) 		rt_pin_write(led_pin ,status =! status) //ȡ��

/*----------------------- Variable Declarations -----------------------------*/
/* ALL_init �¼����ƿ� */
extern struct rt_event init_event;
extern rt_uint8_t VehicleStatus;

/*----------------------- Function Implement --------------------------------*/
void led_thread_entry(void *parameter)
{
	  rt_int8_t i=0;/*��ɫ���ı�> ��   ��   ��   ��   ��   ��   ��   �� */
		rt_int8_t inputdata[8] = {0x00,0x04,0x02,0x01,0x03,0x05,0x06,0x07};
		
		while(i <= 7){
				system_init_led_blink(inputdata[i++]);}
		
		LED_OFF(LED_Red);			//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		LED_OFF(LED_Green);			
		LED_OFF(LED_Blue);
				
    while (1)
    {	

				led_blink_task();
				rt_thread_mdelay(500);
    }
}

/* led��˸����ϵͳ��������ָʾ�ơ� */
void led_blink_task(void)
{
		static rt_uint8_t status = 1;
		if(boma_value_get() == 1){
				LED_Turn(LED_Green,status);	//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		}
}
	

/* ϵͳ��ʼ��led��˸״̬����ʾ7����ɫ�� -->[��ɫ���ı�> ��  ��  ��  ��  ��  ��  ��  �� ] */
void system_init_led_blink(rt_uint8_t InputData)
{
    if(InputData & 0x04){	
						LED_ON(LED_Red); }
		else{ 	LED_OFF(LED_Red);}
		
	  if(InputData & 0x02){	
					LED_ON(LED_Green); }
		else{ LED_OFF(LED_Green);}
		
		if(InputData & 0x01){	
					 LED_ON(LED_Blue); }
		else{  LED_OFF(LED_Blue);}
		rt_thread_mdelay(300);//�ȴ�ϵͳ��ʼ��  ��ϣ�ϵͳ�ȶ����ڶ�ȡ����
}


int led_thread_init(void)
{
    rt_thread_t led_tid;
		/*������̬�߳�*/
    led_tid = rt_thread_create("led",//�߳�����
                    led_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    8,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (led_tid != RT_NULL){
				rt_pin_mode( LED_Red, 	PIN_MODE_OUTPUT);//�������ģʽ	
				rt_pin_mode( LED_Green, PIN_MODE_OUTPUT);	
				rt_pin_mode( LED_Blue, 	PIN_MODE_OUTPUT);	
				LOG_I("LED_Init()");
				rt_event_send(&init_event, LED_EVENT);
				rt_thread_startup(led_tid);
		}
		return 0;
}
INIT_APP_EXPORT(led_thread_init);


/* led on MSH���� */
static int led_on(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
        rt_kprintf("Usage: led_on 68\nSpecies:[red[68]/green[69]/blue[70] \n");
				result = -RT_ERROR;
        goto _exit;
    }
		LED_ON(atoi(argv[1]));
_exit:
    return result;
}
MSH_CMD_EXPORT(led_on,ag:led_on 68  Species:[red[68]/green[69]/blue[70]] );


/* led off MSH���� */
static int led_off(int argc, char **argv)
{

    int result = 0;

    if (argc != 2){
        rt_kprintf("Usage: led_off 68\nSpecies:[red[68]/green[69]/blue[70] \n");
				result = -RT_ERROR;
        goto _exit;
    }
		LED_OFF(atoi(argv[1]));
_exit:
    return result;
}
MSH_CMD_EXPORT(led_off,ag:led_off 68  Species:[red[68]/green[69]/blue[70]] );