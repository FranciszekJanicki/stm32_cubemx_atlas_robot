#ifndef SYSTEM_TASK_SYSTEM_MANAGER_H
#define SYSTEM_TASK_SYSTEM_MANAGER_H

#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "stm32l476xx.h"
#include "stm32l4xx_hal.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    TIM_HandleTypeDef* delta_timer;
} system_config_t;

typedef struct {
    uint32_t timestamp;
    atlas_robot_state_t state;
    atlas_joints_data_t data;
    atlas_joints_path_t path;
    size_t path_index;

    struct {
        float32_t position;
        
    } joint_ctxs[ATLAS_JOINT_NUM];

    system_config_t config;
} system_manager_t;

atlas_err_t system_manager_process(system_manager_t* manager);
atlas_err_t system_manager_initialize(system_manager_t* manager,
                                      system_config_t const* config);

#endif // SYSTEM_TASK_SYSTEM_MANAGER_H