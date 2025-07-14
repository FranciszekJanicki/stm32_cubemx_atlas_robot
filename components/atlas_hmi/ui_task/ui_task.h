#ifndef UI_TASK_UI_TASK_H
#define UI_TASK_UI_TASK_H

#include "button_task.h"
#include "display_task.h"
#include "ui_manager.h"

typedef struct {
    display_task_ctx_t display_task_ctx;
    button_task_ctx_t button_task_ctx;
} ui_task_ctx_t;

void ui_task_initialize(ui_task_ctx_t* task_ctx);

#endif // UI_TASK_UI_TASK_H