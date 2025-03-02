#include "user_task.h"

#define user_is_error() toe_is_error(errorListLength)

uint16_t user_count;  // 计数
uint16_t reset_count; // 重启

void UserTask(void *pvParameters)
{
	laser_configuration();
	while (1)
	{
		
		
		vTaskDelay(10);
	}
}
