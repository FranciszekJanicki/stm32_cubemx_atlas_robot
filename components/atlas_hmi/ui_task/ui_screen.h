#ifndef UI_TASK_UI_SCREEN_H
#define UI_TASK_UI_SCREEN_H

#include "common.h"

typedef enum {
    UI_SCREEN_TYPE_MENU,
    UI_SCREEN_TYPE_PROGRAM,
} ui_screen_type_t;

typedef struct {
} ui_screen_menu_t;

typedef struct {
} ui_screen_program_t;

typedef union {
    ui_screen_menu_t menu;
    ui_screen_program_t program;
} ui_screen_data_t;

typedef struct {
    ui_screen_type_t type;
    ui_screen_data_t data;
} ui_screen_t;

atlas_err_t ui_screen_initialize(ui_screen_t* screen,
                                 ui_screen_type_t type,
                                 ui_screen_data_t const* data);
atlas_err_t ui_screen_deinitialize(ui_screen_t* screen);

#endif // UI_TASK_UI_SCREEN_H