#include "main.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "adc.h"
#include "buzzer.h"
#include "can.h"
#include "delay.h"
#include "laser.h"
#include "led.h"
#include "power_ctrl.h"
#include "sys.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Remote_Control.h"
#include "start_task.h"
#include "usart2.h"
#include "uart_vofa.h"
#include "vofa_task.h"
#include "mpu6500driver.h"
#include "dac.h"
#include "icm20602.h"
#include "usart_judge.h"
#include "usart6_vision.h"
#include "mag_pwm.h"
#include "can_rec.h"
void BSP_init(void);
/*����6 ���� 0xab 0x11 0xdf ��ʾ��ݮ������
Ȼ�� ͨ������2������ͨ�� ��ʾ �Ӿ������Ѿ��ɹ�������
����״� ÿ�ε���ָ���ص� ������ͨ������2��󽮴���6���͵���ı�־
���յ���Ϣ�Ժ���������� 0xbb 0x01 0xdd ����ݮ�� ��ʾ ͬ��ʶ��
��ݮ���յ��Ժ� ������ʶ�������ݵ��� 0xab 0x��� 0x���� 0xdf
��a�� ԭ�ⲻ���ķ�������վ ����վͨ�������յ����ݺ��Զ����� ���洢������
���� ����3���� ����վͨ��ʹ�� ����վֻ���л�ģʽ��ʱ��Ż� ���󽮷���Ϣ
���� ��
��1��ģʽ�� ���������ݵĽ��� ���ᷢ���κ����ݵ� ��A����
��2��ģʽ�£��������� ���ߴ�A�� ��Ҫʵʱ��ʶ�� ���еĶ�ά��
��A��Ҫ���ߣ�0xab 0x22 0xdf�� ��ݮ�� ��Ҫʵʱ��ȥʶ������ ������ ��ʱ ��ݮ����ǰ��ʲô�ͻ�ȥʶ��ʲô ��
��ݮ�ɷ��ͻص����ݸ�ʽ��0xbf 0x���� 0xbb
�����յ������������Ժ� ��Ҫȥ���͸� ����վ
����վ �����ж���ʲôλ��
*/

/*
�����޸Ĳ��� ��
����6��������ݮ��ͨ��  115200
����3���������վͨ��   9600
����8�����������ɿ�ͨ��  115200





*/

int main(void)
{
	BSP_init();
	delay_ms(100);
	pm01_cmd_send(1, 1); // ��������ʹ��
	delay_ms(100);
	startTast();
	vTaskStartScheduler();
	while (1)
	{
		// ѭ����ֹ�˳�
	}
}

void BSP_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �ж��� 4

	delay_init(configTICK_RATE_HZ); // ��ʼ���δ�ʱ��

	led_configuration(); // ��ˮ�ƣ����̵Ƴ�ʼ��

	laser_configuration(); // stm32 �����¶ȴ�����ADC��ʼ��

	temperature_ADC_init(); // ��Դ���ʿ���DAC��ʼ��

	Dac1_Init();
	buzzer_init(30000, 90); // ң������ʼ��
	mpu6500_init();
	remote_control_init(); // CAN�ӿڳ�ʼ��

	//	CAN1_mode_init(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_6tq, 5, CAN_Mode_Normal);
	//	CAN2_mode_init(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_6tq, 5, CAN_Mode_Normal);
	SPI4_Init();
	// 24������ƿڳ�ʼ��
	power_ctrl_configuration(); // ӫ�����

	USART3_Judge_Config();				 // ����3 ���ڶ�ȡ����ϵͳ
	MAG_PWM_Configuration();			 // �����ʼ��,����
	USART6_vision_Configuration(115200); // �Ӿ�

	Init_Uart2(115200); // USART2��ʼ�� ����Vofa+��λ��
	Init_Dma_Tx_Config(USART2, (u32)(&USART2->DR), DMA1_Stream6, DMA_Channel_4, (u32)Vofa.message, sizeof(Vofa.message), DMA1_Stream6_IRQn);
}
