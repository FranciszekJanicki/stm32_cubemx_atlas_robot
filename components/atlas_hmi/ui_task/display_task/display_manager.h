#ifndef DISPLAY_TASK_DISPLAY_MANAGER_H
#define DISPLAY_TASK_DISPLAY_MANAGER_H

#include "common.h"
#include "sh1107.h"

typedef struct {
    bool is_running;
    sh1107_t sh1107;
} display_manager_t;

atlas_err_t display_manager_process(display_manager_t* manager);
atlas_err_t display_manager_initialize(display_manager_t* manager);

#endif // DISPLAY_TASK_DISPLAY_MANAGER_H