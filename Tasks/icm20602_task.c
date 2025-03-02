#include "icm20602_task.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rc_task.h"
#include "delay.h"

void icm20602_task(void *pvParameters)
{
		icm20602_cs_init();
		icm20602_init_spi();
		gyroOffsetInit() ;
	while(1)
	{


	
		vTaskDelay(100);
	}
}


