#include "hmi_state.h"

static atlas_err_t hmi_state_data_menu_initialize(hmi_state_data_menu_t* menu)
{
    ATLAS_ASSERT(menu != NULL);
}

static atlas_err_t hmi_state_data_menu_deinitialize(hmi_state_data_menu_t* menu)
{
    ATLAS_ASSERT(menu != NULL);
}

static atlas_err_t hmi_state_data_program_initialize(
    hmi_state_data_program_t* program)
{
    ATLAS_ASSERT(program != NULL);
}

static atlas_err_t hmi_state_data_program_deinitialize(
    hmi_state_data_program_t* program)
{
    ATLAS_ASSERT(program != NULL);
}

static atlas_err_t hmi_state_data_settings_initialize(
    hmi_state_data_settings_t* settings)
{
    ATLAS_ASSERT(settings != NULL);
}

static atlas_err_t hmi_state_data_settings_deinitialize(
    hmi_state_data_settings_t* settings)
{
    ATLAS_ASSERT(settings != NULL);
}

static atlas_err_t hmi_state_data_initialize(hmi_state_data_t* data,
                                             hmi_state_type_t type)
{
    ATLAS_ASSERT(data != NULL);

    switch (type) {
        case HMI_STATE_TYPE_MENU: {
            return hmi_state_data_menu_initialize(&data->menu);
        }
        case HMI_STATE_TYPE_PROGRAM: {
            return hmi_state_data_program_initialize(&data->program);
        }
        case HMI_STATE_TYPE_SETTINGS: {
            return hmi_state_data_settings_initialize(&data->settings);
        }
        default: {
            return ATLAS_ERR_FAIL;
        }
    }
}

static atlas_err_t hmi_state_data_deinitialize(hmi_state_data_t* data,
                                               hmi_state_type_t type)
{
    ATLAS_ASSERT(data != NULL);

    switch (type) {
        case HMI_STATE_TYPE_MENU: {
            return hmi_state_data_menu_deinitialize(&data->menu);
        }
        case HMI_STATE_TYPE_PROGRAM: {
            return hmi_state_data_program_deinitialize(&data->program);
        }
        case HMI_STATE_TYPE_SETTINGS: {
            return hmi_state_data_settings_deinitialize(&data->settings);
        }
        default: {
            return ATLAS_ERR_FAIL;
        }
    }
}

static atlas_err_t hmi_state_transition_next(hmi_state_t* state)
{
    ATLAS_ASSERT(state != NULL);

    state->type = (state->type + 1U) % HMI_STATE_TYPE_NUM_STATES;
}

static atlas_err_t hmi_state_transition_prev(hmi_state_t* state)
{
    ATLAS_ASSERT(state != NULL);

    state->type = (state->type - 1U) % HMI_STATE_TYPE_NUM_STATES;
}

atlas_err_t hmi_state_transition(hmi_state_t* state,
                                 hmi_state_transition_t transition)
{
    ATLAS_ASSERT(state != NULL);

    if (transition == HMI_STATE_TRANSITION_NEXT) {
        return hmi_state_transition_next(state);
    }
    if (transition == HMI_STATE_TRANSITION_PREV) {
        return hmi_state_transition_prev(state);
    }

    return ATLAS_ERR_FAIL;
}

atlas_err_t hmi_state_update(hmi_state_t* state,
                             hmi_state_update_t const* update)
{
    ATLAS_ASSERT(state != NULL);
    ATLAS_ASSERT(update != NULL);

    switch (state->type) {
        case HMI_STATE_TYPE_MENU: {
            break;
        }
        case HMI_STATE_TYPE_PROGRAM: {
            break;
        }
        case HMI_STATE_TYPE_SETTINGS: {
            break;
        }
        default: {
            break;
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t hmi_state_initialize(hmi_state_t* state, hmi_state_type_t type)
{
    ATLAS_ASSERT(state != NULL);

    state->type = type;

    return hmi_state_data_initialize(&state->data, type);
}

atlas_err_t hmi_state_deinitialize(hmi_state_t* state)
{
    ATLAS_ASSERT(state != NULL);

    return hmi_state_data_deinitialize(&state->data, state->type);
}