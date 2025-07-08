#include "button_manager.h"
#include "button.h"
#include <string.h>

static char const* const TAG = "button_manager";

static inline bool button_manager_receive_button_notify(button_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0x00, BUTTON_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool button_manager_send_ui_notify(ui_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_UI), notify, eSetBits) == pdPASS;
}

static inline bool button_manager_receive_button_event(button_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_BUTTON), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool button_manager_has_button_event(void)
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_BUTTON)) == pdPASS;
}

static inline bool button_manager_send_ui_event(ui_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_UI), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t button_manager_notify_button_handler(button_manager_t* manager,
                                                        button_type_t type)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    ui_event_t event = {.type = UI_EVENT_TYPE_BUTTON};
    event.payload.button.type = type;

    if (!button_manager_send_ui_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t button_manager_notify_handler(button_manager_t* manager, button_notify_t notify)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    return ATLAS_ERR_OK;
}

static atlas_err_t button_manager_event_start_handler(button_manager_t* manager,
                                                      button_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    manager->is_running = true;
    memset(manager->button_ctxs, 0, sizeof(manager->button_ctxs));

    return ATLAS_ERR_OK;
}

static atlas_err_t button_manager_event_stop_handler(button_manager_t* manager,
                                                     button_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    manager->is_running = false;
    memset(manager->button_ctxs, 0, sizeof(manager->button_ctxs));

    return ATLAS_ERR_OK;
}

static atlas_err_t button_manager_event_button_handler(button_manager_t* manager,
                                                       button_event_payload_button_t const* button)
{
    ATLAS_ASSERT(manager && button);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    ui_event_t event = {.type = UI_EVENT_TYPE_BUTTON};
    event.payload.button.type = button->type;
    event.payload.button.state = button->state;

    TickType_t current_tick = xTaskGetTickCount();

    if (button->state == BUTTON_STATE_PRESSED) {
        bool is_long_press = (current_tick - manager->button_ctxs[button->type].tick) >
                             pdMS_TO_TICKS(BUTTON_LONG_PRESS_MS);

        event.payload.button.press = is_long_press ? BUTTON_PRESS_LONG : BUTTON_PRESS_SHORT;
    } else {
        event.payload.button.press = BUTTON_PRESS_NONE;
    }

    manager->button_ctxs[button->type].tick = current_tick;

    if (!button_manager_send_ui_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t button_manager_event_handler(button_manager_t* manager,
                                                button_event_t const* event)
{
    ATLAS_ASSERT(manager && event);
    ATLAS_LOG_FUNC(TAG);

    switch (event->type) {
        case BUTTON_EVENT_TYPE_START: {
            return button_manager_event_start_handler(manager, &event->payload.start);
        }
        case BUTTON_EVENT_TYPE_STOP: {
            return button_manager_event_stop_handler(manager, &event->payload.stop);
        }
        case BUTTON_EVENT_TYPE_PRESS: {
            return button_manager_event_button_handler(manager, &event->payload.button);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t button_manager_process(button_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    button_notify_t notify;
    if (button_manager_receive_button_notify(&notify)) {
        ATLAS_RET_ON_ERR(button_manager_notify_handler(manager, notify));
    }

    button_event_t event;
    while (button_manager_has_button_event()) {
        if (button_manager_receive_button_event(&event)) {
            ATLAS_RET_ON_ERR(button_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t button_manager_initialize(button_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_running = false;
    memset(manager->button_ctxs, 0, sizeof(manager->button_ctxs));

    if (!button_manager_send_ui_notify(UI_NOTIFY_BUTTON_READY)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}
