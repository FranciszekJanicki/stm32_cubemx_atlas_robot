#ifndef COMMON_BUTTON_DATA_H
#define COMMON_BUTTON_DATA_H

typedef enum {
    BUTTON_TYPE_OK,
    BUTTON_TYPE_BACK,
    BUTTON_TYPE_UP,
    BUTTON_TYPE_DOWN,
    BUTTON_TYPE_NONE,
} button_type_t;

typedef enum {
    BUTTON_PRESS_SHORT,
    BUTTON_PRESS_LONG,
} button_press_t;

typedef struct {
    button_type_t type;
    button_press_t press;
} button_data_t;

#endif // COMMON_BUTTON_DATA_H