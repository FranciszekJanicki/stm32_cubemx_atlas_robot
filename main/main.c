#include "main.h"
#include "FreeRTOS.h"
#include "atlas_hmi.h"
#include "task.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    HAL_Delay(500);

    atlas_hmi_initialize();

    vTaskStartScheduler();
}