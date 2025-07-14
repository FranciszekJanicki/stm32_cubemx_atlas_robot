#ifndef PACKET_TASK_PACKET_TASK_H
#define PACKET_TASK_PACKET_TASK_H

#include "packet_manager.h"

typedef struct {
    packet_config_t config;
} packet_task_ctx_t;

void packet_task_initialize(packet_task_ctx_t * task_ctx);

#endif // PACKET_TASK_PACKET_TASK_H