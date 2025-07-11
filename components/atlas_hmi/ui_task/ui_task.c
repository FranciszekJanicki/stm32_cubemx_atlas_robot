#include "ui_task.h"
#include "button_task.h"
#include "display_task.h"
#include "ui_manager.h"

#define UI_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))
#define UI_TASK_PRIORITY (1U)
#define UI_TASK_NAME ("ui_task")
#define UI_TASK_ARGUMENT (NULL)

#define UI_QUEUE_ITEM_SIZE (sizeof(ui_event_t))
#define UI_QUEUE_ITEMS (10U)
#define UI_QUEUE_STORAGE_SIZE (UI_QUEUE_ITEM_SIZE * UI_QUEUE_ITEMS)

static void ui_task_func(void*)
{
    ui_manager_t manager;
    ATLAS_LOG_ON_ERR(UI_TASK_NAME, ui_manager_initialize(&manager));

    while (1) {
        ATLAS_LOG_ON_ERR(UI_TASK_NAME, ui_manager_process(&manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static TaskHandle_t ui_task_create_task(void)
{
    static StaticTask_t ui_task_buffer;
    static StackType_t ui_task_stack[UI_TASK_STACK_DEPTH];

    return xTaskCreateStatic(ui_task_func,
                             UI_TASK_NAME,
                             UI_TASK_STACK_DEPTH,
                             UI_TASK_ARGUMENT,
                             UI_TASK_PRIORITY,
                             ui_task_stack,
                             &ui_task_buffer);
}

static QueueHandle_t ui_task_create_queue(void)
{
    static StaticQueue_t ui_queue_buffer;
    static uint8_t ui_queue_storage[UI_QUEUE_STORAGE_SIZE];

    return xQueueCreateStatic(UI_QUEUE_ITEMS,
                              UI_QUEUE_ITEM_SIZE,
                              ui_queue_storage,
                              &ui_queue_buffer);
}

void ui_task_initialize(void)
{
    task_manager_set(TASK_TYPE_UI, ui_task_create_task());
    queue_manager_set(QUEUE_TYPE_UI, ui_task_create_queue());

    button_task_initialize();
    display_task_initialize();
}

#undef UI_TASK_STACK_DEPTH
#undef UI_TASK_PRIORITY
#undef UI_TASK_NAME
#undef UI_TASK_ARGUMENT

#undef UI_QUEUE_ITEM_SIZE
#undef UI_QUEUE_ITEMS
#undef UI_QUEUE_STORAGE_SIZE
