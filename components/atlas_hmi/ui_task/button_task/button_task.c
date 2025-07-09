#include "button_task.h"
#include "button_manager.h"
#include "button_task.h"
#include "common.h"
#include <stdint.h>

#define BUTTON_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define BUTTON_TASK_PRIORITY (1U)
#define BUTTON_TASK_NAME ("button_task")
#define BUTTON_TASK_ARGUMENT (NULL)

#define BUTTON_QUEUE_ITEMS (10U)
#define BUTTON_QUEUE_ITEM_SIZE (sizeof(button_event_t))
#define BUTTON_QUEUE_STORAGE_SIZE (BUTTON_QUEUE_ITEMS * BUTTON_QUEUE_ITEM_SIZE)

static void button_task_func(void*)
{
    button_manager_t button_manager;
    ATLAS_LOG_ON_ERR(BUTTON_TASK_NAME, button_manager_initialize(&button_manager));

    while (1) {
        ATLAS_LOG_ON_ERR(BUTTON_TASK_NAME, button_manager_process(&button_manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void button_task_initialize(void)
{
    static StaticTask_t button_task_buffer;
    static StackType_t button_task_stack[BUTTON_TASK_STACK_DEPTH];

    TaskHandle_t button_task = xTaskCreateStatic(button_task_func,
                                                 BUTTON_TASK_NAME,
                                                 BUTTON_TASK_STACK_DEPTH,
                                                 BUTTON_TASK_ARGUMENT,
                                                 BUTTON_TASK_PRIORITY,
                                                 button_task_stack,
                                                 &button_task_buffer);

    task_manager_set(TASK_TYPE_BUTTON, button_task);
}

void button_queue_initialize(void)
{
    static StaticQueue_t button_queue_buffer;
    static uint8_t button_queue_storage[BUTTON_QUEUE_STORAGE_SIZE];

    QueueHandle_t button_queue = xQueueCreateStatic(BUTTON_QUEUE_ITEMS,
                                                    BUTTON_QUEUE_ITEM_SIZE,
                                                    button_queue_storage,
                                                    &button_queue_buffer);

    queue_manager_set(QUEUE_TYPE_BUTTON, button_queue);
}

void button_press_callback(button_type_t type)
{
    BaseType_t task_woken = pdFALSE;

    xTaskNotifyFromISR(task_manager_get(TASK_TYPE_BUTTON), 1 << type, eSetBits, &task_woken);

    portYIELD_FROM_ISR(task_woken);
}

#undef BUTTON_TASK_STACK_DEPTH
#undef BUTTON_TASK_PRIORITY
#undef BUTTON_TASK_NAME
#undef BUTTON_TASK_ARGUMENT

#undef BUTTON_QUEUE_ITEMS
#undef BUTTON_QUEUE_ITEM_SIZE
#undef BUTTON_QUEUE_STORAGE_SIZE
