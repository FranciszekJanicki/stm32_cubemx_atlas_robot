#include "main.h"
#include "FreeRTOS.h"
#include "kinematics_task.h"
#include "packet_task.h"
#include "sd_task.h"
#include "system_task.h"
#include "task.h"
#include "ui_task.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    system_queue_initialize();
    ui_queue_intialize();
    packet_queue_initialize();
    kinematics_queue_initialize();

    ui_task_initialize();
    system_task_initialize();
    packet_task_initialize();
    kinematics_task_initialize();

    vTaskStartScheduler();
}