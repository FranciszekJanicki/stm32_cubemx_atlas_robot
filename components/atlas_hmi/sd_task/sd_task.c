#include "sd_task.h"
#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "sd_manager.h"
#include "task.h"
#include <stdint.h>

static char const* const TAG = "sd_task";

#define SD_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define SD_TASK_PRIORITY (1U)

#define SD_QUEUE_ITEMS (10U)
#define SD_QUEUE_ITEM_SIZE (sizeof(sd_event_t))
#define SD_QUEUE_STORAGE_SIZE (SD_QUEUE_ITEMS * SD_QUEUE_ITEM_SIZE)

static void sd_task_func(void*)
{
    sd_manager_t sd_manager;
    ATLAS_LOG_ON_ERR(TAG, sd_manager_initialize(&sd_manager));

    while (1) {
        ATLAS_LOG_ON_ERR(TAG, sd_manager_process(&sd_manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void sd_task_initialize(void)
{
    static StaticTask_t sd_task_buffer;
    static StackType_t sd_task_stack[SD_TASK_STACK_DEPTH];

    TaskHandle_t sd_task = xTaskCreateStatic(sd_task_func,
                                             "sd_task",
                                             SD_TASK_STACK_DEPTH,
                                             NULL,
                                             SD_TASK_PRIORITY,
                                             sd_task_stack,
                                             &sd_task_buffer);

    task_manager_set(TASK_TYPE_SD, sd_task);
}

void sd_queue_initialize(void)
{
    static StaticQueue_t sd_queue_buffer;
    static uint8_t sd_queue_storage[SD_QUEUE_STORAGE_SIZE];

    QueueHandle_t sd_queue =
        xQueueCreateStatic(SD_QUEUE_ITEMS, SD_QUEUE_ITEM_SIZE, sd_queue_storage, &sd_queue_buffer);

    queue_manager_set(QUEUE_TYPE_SD, sd_queue);
}

#undef SD_TASK_STACK_DEPTH
#undef SD_TASK_PRIORITY

#undef SD_QUEUE_ITEMS
#undef SD_QUEUE_ITEM_SIZE
#undef SD_QUEUE_STORAGE_SIZE