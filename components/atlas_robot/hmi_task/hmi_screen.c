#include "hmi_screen.h"

static atlas_err_t hmi_screen_menu_initialize(hmi_screen_menu_t* screen_menu)
{
    ATLAS_ASSERT(screen_menu);
}

static atlas_err_t hmi_screen_menu_deinitialize(hmi_screen_menu_t* screen_menu)
{
    ATLAS_ASSERT(screen_menu);
}

static atlas_err_t hmi_screen_program_initialize(hmi_screen_program_t* screen_program)
{
    ATLAS_ASSERT(screen_program);
}

static atlas_err_t hmi_screen_program_deinitialize(hmi_screen_program_t* screen_program)
{
    ATLAS_ASSERT(screen_program);
}

atlas_err_t hmi_screen_initialize(hmi_screen_t* screen,
                                  hmi_screen_type_t type,
                                  hmi_screen_data_t const* data)
{
    ATLAS_ASSERT(screen && data);

    screen->type = type;
    screen->data = *data;

    switch (screen->type) {
        case HMI_SCREEN_TYPE_MENU:
            return hmi_screen_menu_initialize(&screen->data.menu);
        case HMI_SCREEN_TYPE_PROGRAM:
            return hmi_screen_program_initialize(&screen->data.program);
        default:
            return ATLAS_ERR_FAIL;
    }
}

atlas_err_t hmi_screen_deinitialize(hmi_screen_t* screen)
{
    ATLAS_ASSERT(screen);

    switch (screen->type) {
        case HMI_SCREEN_TYPE_MENU:
            return hmi_screen_menu_deinitialize(&screen->data.menu);
        case HMI_SCREEN_TYPE_PROGRAM:
            return hmi_screen_program_deinitialize(&screen->data.program);
        default:
            return ATLAS_ERR_FAIL;
    }
}