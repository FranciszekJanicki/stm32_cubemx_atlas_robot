#include "kinematics_task.h"
#include "FreeRTOS.h"
#include "common.h"
#include "kinematics_manager.h"
#include "queue.h"
#include "task.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define KINEMATICS_TASK_STACK_DEPTH (4096U / sizeof(StackType_t))
#define KINEMATICS_TASK_PRIORITY (1U)
#define KINEMATICS_TASK_NAME ("kinematics_task")

#define KINEMATICS_QUEUE_ITEMS (10U)
#define KINEMATICS_QUEUE_ITEM_SIZE (sizeof(kinematics_event_t))
#define KINEMATICS_QUEUE_STORAGE_SIZE \
    (KINEMATICS_QUEUE_ITEMS * KINEMATICS_QUEUE_ITEM_SIZE)

static void kinematics_task_func(void* ctx)
{
    kinematics_task_ctx_t* task_ctx = (kinematics_task_ctx_t*)ctx;

    kinematics_manager_t manager;
    ATLAS_LOG_ON_ERR(
        KINEMATICS_TASK_NAME,
        kinematics_manager_initialize(&manager, &task_ctx->config));

    while (1) {
        ATLAS_LOG_ON_ERR(KINEMATICS_TASK_NAME,
                         kinematics_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t kinematics_task_create_task(kinematics_task_ctx_t* task_ctx)
{
    static StaticTask_t kinematics_task_buffer;
    static StackType_t kinematics_task_stack[KINEMATICS_TASK_STACK_DEPTH];

    return xTaskCreateStatic(kinematics_task_func,
                             KINEMATICS_TASK_NAME,
                             KINEMATICS_TASK_STACK_DEPTH,
                             task_ctx,
                             KINEMATICS_TASK_PRIORITY,
                             kinematics_task_stack,
                             &kinematics_task_buffer);
}

static QueueHandle_t kinematics_task_create_queue(void)
{
    static StaticQueue_t kinematics_queue_buffer;
    static uint8_t kinematics_queue_storage[KINEMATICS_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(KINEMATICS_QUEUE_ITEMS,
                              KINEMATICS_QUEUE_ITEM_SIZE,
                              kinematics_queue_storage,
                              &kinematics_queue_buffer);
}

atlas_err_t kinematics_task_initialize(kinematics_task_ctx_t* task_ctx)
{
    ATLAS_ASSERT(task_ctx);

    QueueHandle_t kinematics_queue = kinematics_task_create_queue();
    if (kinematics_queue == NULL) {
        return ATLAS_ERR_FAIL;
    }

    TaskHandle_t kinematics_task = kinematics_task_create_task(task_ctx);
    if (kinematics_task == NULL) {
        return ATLAS_ERR_FAIL;
    }

    queue_manager_set(QUEUE_TYPE_KINEMATICS, kinematics_queue);
    task_manager_set(TASK_TYPE_KINEMATICS, kinematics_task);

    return ATLAS_ERR_OK;
}

#undef KINEMATICS_TASK_STACK_DEPTH
#undef KINEMATICS_TASK_PRIORITY
#undef KINEMATICS_TASK_NAME

#undef KINEMATICS_QUEUE_ITEMS
#undef KINEMATICS_QUEUE_ITEM_SIZE
#undef KINEMATICS_QUEUE_STORAGE_SIZE