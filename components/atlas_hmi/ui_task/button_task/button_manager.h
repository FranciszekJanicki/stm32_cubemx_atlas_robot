#ifndef BUTTON_TASK_BUTTON_MANAGER_H
#define BUTTON_TASK_BUTTON_MANAGER_H

#include "common.h"

typedef struct {
    bool is_running;

    struct {
        TickType_t tick;
    } button_ctxs[BUTTON_TYPE_NUM];
} button_manager_t;

atlas_err_t button_manager_process(button_manager_t* manager);
atlas_err_t button_manager_initialize(button_manager_t* manager);

#endif // BUTTON_TASK_BUTTON_MANAGER_H