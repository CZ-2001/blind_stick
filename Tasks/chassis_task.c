#include "chassis_task.h"

u8 flag_caplow = 0;
u8 flag_cap_use = 0;
void chassis_task(void *pvParameters)
{

	chassis_init();
	while (1)
	{

		vTaskDelay(100);
	}
}
