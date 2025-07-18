#include "button_task.h"
#include "button_manager.h"
#include "button_task.h"
#include "common.h"
#include <stdint.h>

#define BUTTON_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define BUTTON_TASK_PRIORITY (1U)
#define BUTTON_TASK_NAME ("button_task")

#define BUTTON_QUEUE_ITEMS (10U)
#define BUTTON_QUEUE_ITEM_SIZE (sizeof(button_event_t))
#define BUTTON_QUEUE_STORAGE_SIZE (BUTTON_QUEUE_ITEMS * BUTTON_QUEUE_ITEM_SIZE)

static void button_task_func(void* ctx)
{
    button_task_ctx_t* task_ctx = (button_task_ctx_t*)ctx;

    button_manager_t manager;
    ATLAS_LOG_ON_ERR(BUTTON_TASK_NAME, button_manager_initialize(&manager, &task_ctx->config));

    while (1) {
        ATLAS_LOG_ON_ERR(BUTTON_TASK_NAME, button_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t button_task_create_task(button_task_ctx_t* task_ctx)
{
    static StaticTask_t button_task_buffer;
    static StackType_t button_task_stack[BUTTON_TASK_STACK_DEPTH];

    return xTaskCreateStatic(button_task_func,
                             BUTTON_TASK_NAME,
                             BUTTON_TASK_STACK_DEPTH,
                             task_ctx,
                             BUTTON_TASK_PRIORITY,
                             button_task_stack,
                             &button_task_buffer);
}

static QueueHandle_t button_task_create_queue(void)
{
    static StaticQueue_t button_queue_buffer;
    static uint8_t button_queue_storage[BUTTON_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(BUTTON_QUEUE_ITEMS,
                              BUTTON_QUEUE_ITEM_SIZE,
                              button_queue_storage,
                              &button_queue_buffer);
}

atlas_err_t button_task_initialize(button_task_ctx_t* task_ctx)
{
    ATLAS_ASSERT(task_ctx);

    QueueHandle_t button_queue = button_task_create_queue();
    if (button_queue == NULL) {
        return ATLAS_ERR_FAIL;
    }

    TaskHandle_t button_task = button_task_create_task(task_ctx);
    if (button_task == NULL) {
        return ATLAS_ERR_FAIL;
    }

    task_manager_set(TASK_TYPE_BUTTON, button_task);
    queue_manager_set(QUEUE_TYPE_BUTTON, button_queue);

    return ATLAS_ERR_OK;
}

void button_task_press_callback(button_type_t type)
{
    BaseType_t task_woken = pdFALSE;
    xTaskNotifyFromISR(task_manager_get(TASK_TYPE_BUTTON), 1 << type, eSetBits, &task_woken);

    portYIELD_FROM_ISR(task_woken);
}

#undef BUTTON_TASK_STACK_DEPTH
#undef BUTTON_TASK_PRIORITY
#undef BUTTON_TASK_NAME

#undef BUTTON_QUEUE_ITEMS
#undef BUTTON_QUEUE_ITEM_SIZE
#undef BUTTON_QUEUE_STORAGE_SIZE
