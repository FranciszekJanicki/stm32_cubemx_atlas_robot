#include "sd_task.h"
#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "sd_manager.h"
#include "task.h"
#include <stdint.h>

#define SD_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define SD_TASK_NAME ("sd_task")
#define SD_TASK_PRIORITY (1U)
#define SD_TASK_ARGUMENT (NULL)

#define SD_QUEUE_ITEMS (10U)
#define SD_QUEUE_ITEM_SIZE (sizeof(sd_event_t))
#define SD_QUEUE_STORAGE_SIZE (SD_QUEUE_ITEMS * SD_QUEUE_ITEM_SIZE)

static void sd_task_func(void*)
{
    sd_manager_t manager;
    ATLAS_LOG_ON_ERR(SD_TASK_NAME, sd_manager_initialize(&manager));

    while (1) {
        ATLAS_LOG_ON_ERR(SD_TASK_NAME, sd_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t sd_task_create_task(void)
{
    static StaticTask_t sd_task_buffer;
    static StackType_t sd_task_stack[SD_TASK_STACK_DEPTH];

    return xTaskCreateStatic(sd_task_func,
                             SD_TASK_NAME,
                             SD_TASK_STACK_DEPTH,
                             SD_TASK_ARGUMENT,
                             SD_TASK_PRIORITY,
                             sd_task_stack,
                             &sd_task_buffer);
}

static QueueHandle_t sd_task_create_queue(void)
{
    static StaticQueue_t sd_queue_buffer;
    static uint8_t sd_queue_storage[SD_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(SD_QUEUE_ITEMS,
                              SD_QUEUE_ITEM_SIZE,
                              sd_queue_storage,
                              &sd_queue_buffer);
}

void sd_task_initialize(void)
{
    queue_manager_set(QUEUE_TYPE_SD, sd_task_create_queue());
    task_manager_set(TASK_TYPE_SD, sd_task_create_task());
}

#undef SD_TASK_STACK_DEPTH
#undef SD_TASK_PRIORITY
#undef SD_TASK_NAME
#undef SD_TASK_ARGUMENT

#undef SD_QUEUE_ITEMS
#undef SD_QUEUE_ITEM_SIZE
#undef SD_QUEUE_STORAGE_SIZE