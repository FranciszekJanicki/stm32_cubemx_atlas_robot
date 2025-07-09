#include "button.h"

char const* button_type_to_string(button_type_t type)
{
    switch (type) {
        case BUTTON_TYPE_OK:
            return "BUTTON_TYPE_OK";
        case BUTTON_TYPE_BACK:
            return "BUTTON_TYPE_BACK";
        case BUTTON_TYPE_UP:
            return "BUTTON_TYPE_UP";
        case BUTTON_TYPE_DOWN:
            return "BUTTON_TYPE_DOWN";
        default:
            return "";
    }
}

char const* button_state_to_string(button_state_t state)
{
    switch (state) {
        case BUTTON_STATE_PRESSED:
            return "BUTTON_STATE_PRESSED";
        case BUTTON_STATE_RELEASED:
            return "BUTTON_STATE_RELEASED";
        case BUTTON_STATE_NONE:
            return "BUTTON_STATE_NONE";
        default:
            return "";
    }
}

char const* button_press_to_string(button_press_t press)
{
    switch (press) {
        case BUTTON_PRESS_LONG:
            return "BUTTON_PRESS_LONG";
        case BUTTON_PRESS_SHORT:
            return "BUTTON_PRESS_SHORT";
        case BUTTON_PRESS_NONE:
            return "BUTTON_PRESS_NONE";
        default:
            return "";
    }
}