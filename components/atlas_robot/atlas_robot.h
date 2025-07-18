#ifndef ATLAS_ROBOT_ATLAS_ROBOT_H
#define ATLAS_ROBOT_ATLAS_ROBOT_H

#include "hmi_task.h"
#include "kinematics_task.h"
#include "packet_task.h"
#include "uart_task.h"

typedef struct {
    uart_task_ctx_t uart_ctx;
    packet_task_ctx_t packet_ctx;
    hmi_task_ctx_t hmi_ctx;
    kinematics_task_ctx_t kinematics_ctx;
} atlas_robot_config_t;

void atlas_robot_initialize(atlas_robot_config_t const* config);

#endif // ATLAS_ROBOT_ATLAS_ROBOT_H