#ifndef HMI_TASK_HMI_TASK_H
#define HMI_TASK_HMI_TASK_H

#include "button_task.h"
#include "display_task.h"
#include "hmi_manager.h"

typedef struct {
    display_task_ctx_t display_task_ctx;
    button_task_ctx_t button_task_ctx;
} hmi_task_ctx_t;

atlas_err_t hmi_task_initialize(hmi_task_ctx_t* task_ctx);

#endif // HMI_TASK_HMI_TASK_H