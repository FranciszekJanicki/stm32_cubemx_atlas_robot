#include "display_task.h"
#include "FreeRTOS.h"
#include "common.h"
#include "display_manager.h"
#include "queue.h"
#include "task.h"

#define DISPLAY_TASK_PRIORITY (1U)
#define DISPLAY_TASK_NAME ("display_task")
#define DISPLAY_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define DISPLAY_TASK_ARGUMENT (NULL)

#define DISPLAY_QUEUE_ITEM_SIZE (sizeof(display_event_t))
#define DISPLAY_QUEUE_ITEMS (10U)
#define DISPLAY_QUEUE_STORAGE_SIZE (DISPLAY_QUEUE_ITEM_SIZE * DISPLAY_QUEUE_ITEMS)

static void display_task_func(void*)
{
    display_manager_t manager;
    ATLAS_LOG_ON_ERR(DISPLAY_TASK_NAME, display_manager_initialize(&manager));

    while (1) {
        ATLAS_LOG_ON_ERR(DISPLAY_TASK_NAME, display_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void display_task_initialize(void)
{
    static StaticTask_t display_task_buffer;
    static StackType_t display_task_stack[DISPLAY_TASK_STACK_DEPTH];

    TaskHandle_t display_task = xTaskCreateStatic(display_task_func,
                                                  DISPLAY_TASK_NAME,
                                                  DISPLAY_TASK_STACK_DEPTH,
                                                  DISPLAY_TASK_ARGUMENT,
                                                  DISPLAY_TASK_PRIORITY,
                                                  display_task_stack,
                                                  &display_task_buffer);

    task_manager_set(TASK_TYPE_DISPLAY, display_task);
}

void display_queue_initialize(void)
{
    static StaticQueue_t display_queue_buffer;
    static uint8_t display_queue_storage[DISPLAY_QUEUE_STORAGE_SIZE];

    QueueHandle_t display_queue = xQueueCreateStatic(DISPLAY_QUEUE_ITEMS,
                                                     DISPLAY_QUEUE_ITEM_SIZE,
                                                     display_queue_storage,
                                                     &display_queue_buffer);

    queue_manager_set(QUEUE_TYPE_DISPLAY, display_queue);
}

#undef DISPLAY_TASK_PRIORITY
#undef DISPLAY_TASK_NAME
#undef DISPLAY_TASK_STACK_DEPTH
#undef DISPLAY_TASK_ARGUMENT

#undef DISPLAY_QUEUE_ITEM_SIZE
#undef DISPLAY_QUEUE_ITEMS
#undef DISPLAY_QUEUE_STORAGE_SIZE
