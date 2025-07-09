#include "ui_screen.h"

static atlas_err_t ui_screen_menu_initialize(ui_screen_menu_t* screen_menu)
{
    ATLAS_ASSERT(screen_menu);
}

static atlas_err_t ui_screen_menu_deinitialize(ui_screen_menu_t* screen_menu)
{
    ATLAS_ASSERT(screen_menu);
}

static atlas_err_t ui_screen_program_initialize(ui_screen_program_t* screen_program)
{
    ATLAS_ASSERT(screen_program);
}

static atlas_err_t ui_screen_program_deinitialize(ui_screen_program_t* screen_program)
{
    ATLAS_ASSERT(screen_program);
}

atlas_err_t ui_screen_initialize(ui_screen_t* screen,
                                 ui_screen_type_t type,
                                 ui_screen_data_t const* data)
{
    ATLAS_ASSERT(screen && data);

    screen->type = type;
    screen->data = *data;

    switch (screen->type) {
        case UI_SCREEN_TYPE_MENU:
            return ui_screen_menu_initialize(&screen->data.menu);
        case UI_SCREEN_TYPE_PROGRAM:
            return ui_screen_program_initialize(&screen->data.program);
        default:
            return ATLAS_ERR_FAIL;
    }
}

atlas_err_t ui_screen_deinitialize(ui_screen_t* screen)
{
    ATLAS_ASSERT(screen);

    switch (screen->type) {
        case UI_SCREEN_TYPE_MENU:
            return ui_screen_menu_deinitialize(&screen->data.menu);
        case UI_SCREEN_TYPE_PROGRAM:
            return ui_screen_program_deinitialize(&screen->data.program);
        default:
            return ATLAS_ERR_FAIL;
    }
}