#ifndef HMI_TASK_HMI_SCREEN_H
#define HMI_TASK_HMI_SCREEN_H

#include "common.h"

typedef enum {
    HMI_SCREEN_TYPE_MENU,
    HMI_SCREEN_TYPE_PROGRAM,
} hmi_screen_type_t;

typedef int hmi_screen_menu_t;

typedef int hmi_screen_program_t;

typedef union {
    hmi_screen_menu_t menu;
    hmi_screen_program_t program;
} hmi_screen_data_t;

typedef struct {
    hmi_screen_type_t type;
    hmi_screen_data_t data;
} hmi_screen_t;

atlas_err_t hmi_screen_initialize(hmi_screen_t* screen,
                                  hmi_screen_type_t type,
                                  hmi_screen_data_t const* data);
atlas_err_t hmi_screen_deinitialize(hmi_screen_t* screen);

#endif // HMI_TASK_HMI_SCREEN_H