#ifndef KINEMATICS_TASK_KINEMATICS_TASK_H
#define KINEMATICS_TASK_KINEMATICS_TASK_H

#include "kinematics_manager.h"

typedef struct {
    kinematics_config_t config;
} kinematics_task_ctx_t;

atlas_err_t kinematics_task_initialize(kinematics_task_ctx_t* task_ctx);

#endif // KINEMATICS_TASK_KINEMATICS_TASK_H