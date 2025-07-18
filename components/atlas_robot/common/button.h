#ifndef COMMON_BUTTON_H
#define COMMON_BUTTON_H

#define BUTTON_LONG_PRESS_MS (1000U)

typedef enum {
    BUTTON_TYPE_OK,
    BUTTON_TYPE_BACK,
    BUTTON_TYPE_UP,
    BUTTON_TYPE_DOWN,
    BUTTON_TYPE_NUM,
} button_type_t;

typedef enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_NONE,
} button_state_t;

typedef enum {
    BUTTON_PRESS_SHORT,
    BUTTON_PRESS_LONG,
    BUTTON_PRESS_NONE,
} button_press_t;

typedef struct {
    button_type_t type;
    button_state_t state;
    button_press_t press;
} button_data_t;

char const* button_type_to_string(button_type_t type);
char const* button_state_to_string(button_state_t state);
char const* button_press_to_string(button_press_t press);

#endif // COMMON_BUTTON_H