#ifndef SD_TASK_SD_MANAGER_H
#define SD_TASK_SD_MANAGER_H

#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    bool is_running;
} sd_manager_t;

atlas_err_t sd_manager_initialize(sd_manager_t* manager);
atlas_err_t sd_manager_process(sd_manager_t* manager);

#endif // SD_TASK_SD_MANAGER_H