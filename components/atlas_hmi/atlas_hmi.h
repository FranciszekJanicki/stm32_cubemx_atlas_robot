#ifndef ATLAS_HMI_ATLAS_HMI_H
#define ATLAS_HMI_ATLAS_HMI_H

#include "kinematics_task.h"
#include "packet_task.h"
#include "uart_task.h"
#include "ui_task.h"

typedef struct {
    uart_task_ctx_t uart_task_ctx;
    packet_task_ctx_t packet_task_ctx;
    ui_task_ctx_t ui_task_ctx;
    kinematics_task_ctx_t kinematics_task_ctx;
} atlas_hmi_config_t;

void atlas_hmi_initialize(atlas_hmi_config_t const* config);

#endif // ATLAS_HMI_ATLAS_HMI_H