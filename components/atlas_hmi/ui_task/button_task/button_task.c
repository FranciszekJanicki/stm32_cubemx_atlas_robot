#include "button_task.h"
#include "button_manager.h"

void button_press_callback(button_type_t type)
{
    BaseType_t task_woken = pdFALSE;

    xTaskNotifyFromISR(task_manager_get(TASK_TYPE_BUTTON), 1 << type, eSetBits, &task_woken);

    portYIELD_FROM_ISR(task_woken);
}