#include "stm32f4xx.h"

void MAG_PWM_Configuration(void)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
	
	//初始化TIM5	//TIM5 CH4 I0
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5时钟使能   
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);                     //??GPIOI???
	
	GPIO_PinAFConfig( GPIOI,GPIO_PinSource0,GPIO_AF_TIM5 ); //GPIOH12复用为定时器5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;           //GPIOH12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOI,&GPIO_InitStructure);              //初始化GPIOH
	  
	TIM_TimeBaseStructure.TIM_Prescaler=8999;  //定时器分频
	TIM_TimeBaseStructure.TIM_Period = 200;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_OCInitStructure.TIM_Pulse = 25;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
	TIM_OC4Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 4OC2
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM15在CCR1上的预装载寄存器
  TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE使能 
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//初始化定时器5
                                                  //TIM8??
	TIM_CtrlPWMOutputs(TIM5, ENABLE);                                         //?????
}  
