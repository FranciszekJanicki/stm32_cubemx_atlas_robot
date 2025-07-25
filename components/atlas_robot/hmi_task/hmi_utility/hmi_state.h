#ifndef HMI_UTILITY_HMI_STATE_H
#define HMI_UTILITY_HMI_STATE_H

#include "common.h"

typedef struct {
    atlas_timestamp_t timestamp;
    atlas_joints_data_t joints;
    atlas_cartesian_data_t cartesian;
} hmi_state_update_t;

typedef enum {
    HMI_STATE_TRANSITION_NEXT,
    HMI_STATE_TRANSITION_PREV,
} hmi_state_transition_t;

typedef enum {
    HMI_STATE_TYPE_MENU,
    HMI_STATE_TYPE_PROGRAM,
    HMI_STATE_TYPE_SETTINGS,
    HMI_STATE_TYPE_NUM_STATES,
} hmi_state_type_t;

typedef struct {
} hmi_state_data_menu_t;

typedef struct {
} hmi_state_data_program_t;

typedef struct {
} hmi_state_data_settings_t;

typedef union {
    hmi_state_data_menu_t menu;
    hmi_state_data_program_t program;
    hmi_state_data_settings_t settings;
} hmi_state_data_t;

typedef struct {
    void (*transition_next_callback)(void*);
    void (*transition_prev_callback)(void*);
} hmi_state_interface_t;

typedef struct {
    hmi_state_type_t type;
    hmi_state_data_t data;
    hmi_state_interface_t interface;
} hmi_state_t;

atlas_err_t hmi_state_transition(hmi_state_t* state,
                                 hmi_state_transition_t transition);
atlas_err_t hmi_state_update(hmi_state_t* state,
                             hmi_state_update_t const* update);

atlas_err_t hmi_state_initialize(hmi_state_t* state, hmi_state_type_t type);
atlas_err_t hmi_state_deinitialize(hmi_state_t* state);

#endif // HMI_UTILITY_HMI_STATE_H