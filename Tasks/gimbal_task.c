#include "gimbal_task.h"
uint16_t gimbal_count;
void gimbal_task(void *pvParameters)
{
	
	gimbal_init(&Gimbal_TypedefStructure);
	while (1)
	{
		

		vTaskDelay(1);
	}
}
