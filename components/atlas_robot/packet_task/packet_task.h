#ifndef PACKET_TASK_PACKET_TASK_H
#define PACKET_TASK_PACKET_TASK_H

#include "common.h"
#include "packet_manager.h"

typedef struct {
    packet_config_t config;
} packet_task_ctx_t;

atlas_err_t packet_task_initialize(packet_task_ctx_t* task_ctx);

void packet_task_robot_packet_ready_callback(void);

#endif // PACKET_TASK_PACKET_TASK_H