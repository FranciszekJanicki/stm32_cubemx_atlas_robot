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
    RTC_HandleTypeDef* timestamp_rtc;
#ifdef PACKET_TEST
    TIM_HandleTypeDef* packet_ready_timer;
#endif
    GPIO_TypeDef* delta_period_gpio;
    uint16_t delta_period_pin;
} system_config_t;

typedef struct {
    atlas_robot_state_t state;

    atlas_joints_path_t current_path;
    size_t current_path_index;

    atlas_timestamp_t startup_timestamp;
    atlas_timestamp_t current_timestamp;

    struct {
        bool is_ready;
        bool has_fault;
        float32_t meas_position;
        float32_t goal_position;
    } joint_ctxs[ATLAS_JOINT_NUM];

    system_config_t config;
} system_manager_t;

atlas_err_t system_manager_process(system_manager_t* manager);
atlas_err_t system_manager_initialize(system_manager_t* manager,
                                      system_config_t const* config);

#endif // SYSTEM_TASK_SYSTEM_MANAGER_H