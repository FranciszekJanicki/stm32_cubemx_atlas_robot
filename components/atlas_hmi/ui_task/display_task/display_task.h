#ifndef DISPLAY_TASK_DISPLAY_TASK_H
#define DISPLAY_TASK_DISPLAY_TASK_H

#include "display_manager.h"

typedef struct {
    display_config_t config;
} display_task_ctx_t;

void display_task_initialize(display_task_ctx_t* task_ctx);

#endif // DISPLAY_TASK_DISPLAY_TASK_H