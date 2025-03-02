#include "shoot_task.h"

void shoot_task(void *pvParameters)
{
	shoot_init();
	while (1)
	{
	
		vTaskDelay(10);
	}
}
