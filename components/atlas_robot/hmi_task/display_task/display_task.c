#include "display_task.h"
#include "FreeRTOS.h"
#include "common.h"
#include "display_manager.h"
#include "queue.h"
#include "task.h"

#define DISPLAY_TASK_PRIORITY (1U)
#define DISPLAY_TASK_NAME ("display_task")
#define DISPLAY_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))

#define DISPLAY_QUEUE_ITEM_SIZE (sizeof(display_event_t))
#define DISPLAY_QUEUE_ITEMS (10U)
#define DISPLAY_QUEUE_STORAGE_SIZE \
    (DISPLAY_QUEUE_ITEM_SIZE * DISPLAY_QUEUE_ITEMS)

static void display_task_func(void* ctx)
{
    display_task_ctx_t* task_ctx = (display_task_ctx_t*)ctx;

    display_manager_t manager;
    ATLAS_LOG_ON_ERR(DISPLAY_TASK_NAME,
                     display_manager_initialize(&manager, &task_ctx->config));

    while (1) {
        ATLAS_LOG_ON_ERR(DISPLAY_TASK_NAME, display_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t display_task_create_task(display_task_ctx_t* task_ctx)
{
    static StaticTask_t display_task_buffer;
    static StackType_t display_task_stack[DISPLAY_TASK_STACK_DEPTH];

    return xTaskCreateStatic(display_task_func,
                             DISPLAY_TASK_NAME,
                             DISPLAY_TASK_STACK_DEPTH,
                             task_ctx,
                             DISPLAY_TASK_PRIORITY,
                             display_task_stack,
                             &display_task_buffer);
}

static QueueHandle_t display_task_create_queue(void)
{
    static StaticQueue_t display_queue_buffer;
    static uint8_t display_queue_storage[DISPLAY_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(DISPLAY_QUEUE_ITEMS,
                              DISPLAY_QUEUE_ITEM_SIZE,
                              display_queue_storage,
                              &display_queue_buffer);
}

atlas_err_t display_task_initialize(display_task_ctx_t* task_ctx)
{
    ATLAS_ASSERT(task_ctx);

    QueueHandle_t display_queue = display_task_create_queue();
    if (display_queue == NULL) {
        return ATLAS_ERR_FAIL;
    }

    TaskHandle_t display_task = display_task_create_task(task_ctx);
    if (display_task == NULL) {
        return ATLAS_ERR_FAIL;
    }

    queue_manager_set(QUEUE_TYPE_DISPLAY, display_queue);
    task_manager_set(TASK_TYPE_DISPLAY, display_task);

    return ATLAS_ERR_FAIL;
}

#undef DISPLAY_TASK_PRIORITY
#undef DISPLAY_TASK_NAME
#undef DISPLAY_TASK_STACK_DEPTH

#undef DISPLAY_QUEUE_ITEM_SIZE
#undef DISPLAY_QUEUE_ITEMS
#undef DISPLAY_QUEUE_STORAGE_SIZE
