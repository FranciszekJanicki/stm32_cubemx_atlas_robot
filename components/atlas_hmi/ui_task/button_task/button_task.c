#include "button_task.h"
#include "button_manager.h"

TaskHandle_t button_task;

void button_press_callback(button_type_t type)
{
    BaseType_t task_woken = pdFALSE;

    xTaskNotifyFromISR(button_task, 1 << type, eSetBits, &task_woken);

    portYIELD_FROM_ISR(task_woken);
}