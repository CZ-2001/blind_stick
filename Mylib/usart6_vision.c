#include "usart6_vision.h"

u8 USART6_RX_BUF[BUFF_LEN];
uint16_t USART6_RX_STA = 0;
uint8_t USART6_TX_BUF;

uint8_t rrr;

typedef struct
{
    float Pitch;
    float Yaw;
    float Roll;
} MY_Angle;

MY_Angle Angel;


void USART6_vision_Configuration(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);

	USART_Cmd(USART6, ENABLE);

	USART_ClearFlag(USART6, USART_FLAG_TC);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t ishead6_f = 0;
uint8_t sta6 = 0;
int LEN6 = 4;
uint8_t rrr;
u8 start = 0;
/*

串口6 接受 0xab 0x11 0xdf 表示树莓派启动
然后 通过串口2与匿名通信 表示 视觉方面已经成功！！！
结合雷达 每次到达指定地点 凌霄就通过串口2与大疆串口6发送到达的标志
大疆收到消息以后就立即发送 0xbb 0x01 0xdd 到树莓派 表示 同意识别
树莓派收到以后 就立即识别发送数据到达 0xab 0x序号 0x内容 0xdf
大疆a板 原封不动的发至地面站 地面站通过串口收到数据后自动接受 并存储！！！
接着 串口3是与 地面站通信使用 地面站只有切换模式的时候才会 给大疆发消息
比如 ：（1）模式下 仅进行数据的接收 不会发送任何数据到 大疆A板子
（2）模式下：发送数据 告诉大疆A版 我要实时的识别 手中的二维码
大疆A板要告诉（0xab 0x22 0xdf） 树莓派 你要实时的去识别数据 （就是 此时 树莓派面前有什么就会去识别什么 ）
树莓派发送回的数据格式：0xbf 0x内容 0xbb
大疆在收到回来的数据以后 就要去发送给 地面站
地面站 根据判断在什么位置





*/






void updata_angle(u8 *rx_buf, MY_Angle *angle)
{
	angle->Roll = (((rx_buf[3] << 8) | rx_buf[2]) / 32768.0) * 180.0;				   // 32768;//*180.0;
	angle->Pitch = (((rx_buf[5] << 8) | rx_buf[4]) / 32768.0) * 180.0;				   // 32768*180.0;
	angle->Yaw = ((((rx_buf[7] << 8) | rx_buf[6]) / 32768.0) * 180.0) ; //*180.0;
}



void USART6_IRQHandler(void)
{
	if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		
		rrr = USART_ReceiveData(USART6);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		if (ishead6_f)
		{
			sta6++;
			USART6_RX_BUF[sta6] = rrr;
			if (sta6 == 10)
			{
				sta6 = 0;
				ishead6_f = 0;
				updata_angle(USART6_RX_BUF, &Angel);
			}
		}
		else
		{
			USART6_RX_BUF[sta6] = rrr;
			if (sta6 == 0)
			{
				if (USART6_RX_BUF[sta6] == 0x55)
				sta6 = 1;
			}
			else if (sta6 == 1)
			{
				if (USART6_RX_BUF[sta6] == 0x53)
				ishead6_f = 1;
			}
		}






	}
}

void USART6_SendChar(uint8_t ch)
{
	// while (USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET);
	USART_SendData(USART6, ch);
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET)
		;
}
// ����λ������
void vision_send(void)
{
	/*
	01234567
	4+5: (0:left 1:right 2:stay)
	6: aim mode (0: amor 1:buff)
	7:aimcolor(O:blue1:red)
	*/
	//	USART6_TX_BUF = 0x00; // ���λ
	if (Gimbal_Str->vision_forcast == vision_right)
	{
		USART6_TX_BUF &= 0xF3; // 1111 0011
		USART6_TX_BUF |= 0x04;
	}
	else if (Gimbal_Str->vision_forcast == vision_left)
	{
		USART6_TX_BUF &= 0xF3; // 0000 1100
	}
	else if (Gimbal_Str->vision_forcast == vision_stay)
	{
		USART6_TX_BUF &= 0xF3;	 // ���
		USART6_TX_BUF |= (0x0C); // 0000 1100
	}
	if (Gimbal_Str->Key_Mode == Gimbal_WindMill)
	{
		USART6_TX_BUF &= 0xFD; //   1111 1101 ��0���
		USART6_TX_BUF |= 0x02;
	}
	else
		USART6_TX_BUF &= 0xFD; //   0000 0010  ��0���
	//
	if (JudgeStructure.ext_game_robot_status.robot_id < 10)
		USART6_TX_BUF |= 0x01;
	else if (JudgeStructure.ext_game_robot_status.robot_id > 10)
		USART6_TX_BUF &= 0x0E;
	//	USART6_TX_BUF|=0xFF;

	USART6_SendChar(USART6_TX_BUF);
}
