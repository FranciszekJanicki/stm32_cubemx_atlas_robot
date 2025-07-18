#ifndef BUTTON_TASK_BUTTON_TASK_H
#define BUTTON_TASK_BUTTON_TASK_H

#include "FreeRTOS.h"
#include "button_manager.h"
#include "queue.h"
#include "task.h"

typedef struct {
    button_config_t config;
} button_task_ctx_t;

atlas_err_t button_task_initialize(button_task_ctx_t* task_ctx);

void button_task_press_callback(button_type_t type);

#endif // BUTTON_TASK_BUTTON_TASK_H