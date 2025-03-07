#include "vision_behaviour.h"

VisionTypedef VisionStructure;

void vision_updata(VisionTypedef *Structure, uint16_t *buff)
{
	static uint16_t xrr,yrr;
	static uint16_t Xrr,Yrr;
	xrr=(buff[1]<<8)| buff[2] ;
	yrr=(buff[3]<<8)| buff[4] ;
	if(buff[5]&0x02)   
		Xrr=(xrr*1.0)+360.0;  
	else if((buff[5]&0x02)==0)
		Xrr=-(xrr*1.0)+360.0; 
	if(buff[5]&0x01)
		Yrr=(yrr*1.0)+360;
	else if((buff[5]&0x01)==0)
		Yrr=-(yrr*1.0)+360;
	Structure->x = Xrr/4.0 ;
	Structure->y = Yrr/4.0 ;
}
