#ifndef UI_TASK_UI_MANAGER_H
#define UI_TASK_UI_MANAGER_H

#include "common.h"

typedef struct {
    bool is_running;
    bool is_button_ready;
    bool is_display_ready;
} ui_manager_t;

atlas_err_t ui_manager_initialize(ui_manager_t* manager);
atlas_err_t ui_manager_process(ui_manager_t* manager);

#endif // UI_TASK_UI_MANAGER_H