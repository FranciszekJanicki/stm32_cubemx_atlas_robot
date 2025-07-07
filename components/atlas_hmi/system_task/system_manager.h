#ifndef SYSTEM_TASK_SYSTEM_MANAGER_H
#define SYSTEM_TASK_SYSTEM_MANAGER_H

#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    atlas_status_t status;
} system_manager_t;

atlas_err_t system_manager_initialize(system_manager_t* manager);
atlas_err_t system_manager_process(system_manager_t* manager);

#endif // SYSTEM_TASK_SYSTEM_MANAGER_H