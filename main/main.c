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
/*串口6 接受 0xab 0x11 0xdf 表示树莓派启动
然后 通过串口2与匿名通信 表示 视觉方面已经成功！！！
结合雷达 每次到达指定地点 凌霄就通过串口2与大疆串口6发送到达的标志
大疆收到消息以后就立即发送 0xbb 0x01 0xdd 到树莓派 表示 同意识别
树莓派收到以后 就立即识别发送数据到达 0xab 0x序号 0x内容 0xdf
大疆a板 原封不动的发至地面站 地面站通过串口收到数据后自动接受 并存储！！！
接着 串口3是与 地面站通信使用 地面站只有切换模式的时候才会 给大疆发消息
比如 ：
（1）模式下 仅进行数据的接收 不会发送任何数据到 大疆A板子
（2）模式下：发送数据 告诉大疆A版 我要实时的识别 手中的二维码
大疆A板要告诉（0xab 0x22 0xdf） 树莓派 你要实时的去识别数据 （就是 此时 树莓派面前有什么就会去识别什么 ）
树莓派发送回的数据格式：0xbf 0x内容 0xbb
大疆在收到回来的数据以后 就要去发送给 地面站
地面站 根据判断在什么位置
*/

/*
毕设修改部分 ：
串口6用于与树莓派通信  115200
串口3用于与地面站通信   9600
串口8用于与匿名飞控通信  115200





*/

int main(void)
{
	BSP_init();
	delay_ms(100);
	pm01_cmd_send(1, 1); // 超级电容使能
	delay_ms(100);
	startTast();
	vTaskStartScheduler();
	while (1)
	{
		// 循环防止退出
	}
}

void BSP_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 中断组 4

	delay_init(configTICK_RATE_HZ); // 初始化滴答时钟

	led_configuration(); // 流水灯，红绿灯初始化

	laser_configuration(); // stm32 板载温度传感器ADC初始化

	temperature_ADC_init(); // 电源功率控制DAC初始化

	Dac1_Init();
	buzzer_init(30000, 90); // 遥控器初始化
	mpu6500_init();
	remote_control_init(); // CAN接口初始化

	//	CAN1_mode_init(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_6tq, 5, CAN_Mode_Normal);
	//	CAN2_mode_init(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_6tq, 5, CAN_Mode_Normal);
	SPI4_Init();
	// 24输出控制口初始化
	power_ctrl_configuration(); // 荧光充能

	USART3_Judge_Config();				 // 串口3 用于读取裁判系统
	MAG_PWM_Configuration();			 // 舵机初始化,弹仓
	USART6_vision_Configuration(115200); // 视觉

	Init_Uart2(115200); // USART2初始化 用于Vofa+上位机
	Init_Dma_Tx_Config(USART2, (u32)(&USART2->DR), DMA1_Stream6, DMA_Channel_4, (u32)Vofa.message, sizeof(Vofa.message), DMA1_Stream6_IRQn);
}
