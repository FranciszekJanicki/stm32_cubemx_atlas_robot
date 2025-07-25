#ifndef HMI_TASK_HMI_MANAGER_H
#define HMI_TASK_HMI_MANAGER_H

#include "common.h"
#include "hmi_state.h"

typedef struct {
} hmi_config_t;

typedef struct {
    bool is_running;
    bool is_button_ready;
    bool is_display_ready;

    hmi_state_t state;
} hmi_manager_t;

atlas_err_t hmi_manager_initialize(hmi_manager_t* manager);
atlas_err_t hmi_manager_process(hmi_manager_t* manager);

#endif // HMI_TASK_HMI_MANAGER_H