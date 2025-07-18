#include "hmi_task.h"
#include "button_task.h"
#include "display_task.h"
#include "hmi_manager.h"

#define HMI_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define HMI_TASK_PRIORITY (1U)
#define HMI_TASK_NAME ("hmi_task")
#define HMI_TASK_ARGUMENT (NULL)

#define HMI_QUEUE_ITEM_SIZE (sizeof(hmi_event_t))
#define HMI_QUEUE_ITEMS (10U)
#define HMI_QUEUE_STORAGE_SIZE (HMI_QUEUE_ITEM_SIZE * HMI_QUEUE_ITEMS)

static void hmi_task_func(void* ctx)
{
    hmi_task_ctx_t* task_ctx = (hmi_task_ctx_t*)ctx;

    hmi_manager_t manager;
    ATLAS_LOG_ON_ERR(HMI_TASK_NAME, hmi_manager_initialize(&manager));

    while (1) {
        ATLAS_LOG_ON_ERR(HMI_TASK_NAME, hmi_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t hmi_task_create_task(hmi_task_ctx_t* task_ctx)
{
    static StaticTask_t hmi_task_buffer;
    static StackType_t hmi_task_stack[HMI_TASK_STACK_DEPTH];

    return xTaskCreateStatic(hmi_task_func,
                             HMI_TASK_NAME,
                             HMI_TASK_STACK_DEPTH,
                             task_ctx,
                             HMI_TASK_PRIORITY,
                             hmi_task_stack,
                             &hmi_task_buffer);
}

static QueueHandle_t hmi_task_create_queue(void)
{
    static StaticQueue_t hmi_queue_buffer;
    static uint8_t hmi_queue_storage[HMI_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(HMI_QUEUE_ITEMS,
                              HMI_QUEUE_ITEM_SIZE,
                              hmi_queue_storage,
                              &hmi_queue_buffer);
}

atlas_err_t hmi_task_initialize(hmi_task_ctx_t* task_ctx)
{
    ATLAS_ASSERT(task_ctx);

    TaskHandle_t hmi_task = hmi_task_create_task(task_ctx);
    if (hmi_task == NULL) {
        return ATLAS_ERR_FAIL;
    }

    QueueHandle_t hmi_queue = hmi_task_create_queue();
    if (hmi_queue == NULL) {
        return ATLAS_ERR_FAIL;
    }

    task_manager_set(TASK_TYPE_HMI, hmi_task);
    queue_manager_set(QUEUE_TYPE_HMI, hmi_queue);

    ATLAS_RET_ON_ERR(button_task_initialize(&task_ctx->button_ctx));
    ATLAS_RET_ON_ERR(display_task_initialize(&task_ctx->display_ctx));

    return ATLAS_ERR_OK;
}

#undef HMI_TASK_STACK_DEPTH
#undef HMI_TASK_PRIORITY
#undef HMI_TASK_NAME
#undef HMI_TASK_ARGUMENT

#undef HMI_QUEUE_ITEM_SIZE
#undef HMI_QUEUE_ITEMS
#undef HMI_QUEUE_STORAGE_SIZE
