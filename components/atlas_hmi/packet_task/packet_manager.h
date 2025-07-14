#ifndef PACKET_TASK_PACKET_MANAGER_H
#define PACKET_TASK_PACKET_MANAGER_H

#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "stm32l476xx.h"
#include "stm32l4xx_hal.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    GPIO_TypeDef* rob_packet_ready_gpio;
    uint16_t rob_packet_ready_pin;
} packet_config_t;

typedef struct {
    bool is_running;

    packet_config_t config;
} packet_manager_t;

atlas_err_t packet_manager_process(packet_manager_t* manager);
atlas_err_t packet_manager_initialize(packet_manager_t* manager, packet_config_t const* config);

#endif // PACKET_TASK_PACKET_MANAGER_H