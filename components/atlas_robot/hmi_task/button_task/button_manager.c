#include "button_manager.h"
#include "button.h"
#include "common.h"
#include <string.h>

static char const* const TAG = "button_manager";

static inline bool button_manager_receive_button_notify(button_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0x00, BUTTON_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool button_manager_send_hmi_notify(hmi_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_HMI), notify, eSetBits) == pdPASS;
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

static inline bool button_manager_send_hmi_event(hmi_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_HMI), event, pdMS_TO_TICKS(1)) == pdPASS;
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

static atlas_err_t button_manager_event_button_data_handler(
    button_manager_t* manager,
    button_event_payload_button_data_t const* button_data)
{
    ATLAS_ASSERT(manager && button_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    hmi_event_t event = {.type = HMI_EVENT_TYPE_BUTTON_DATA};
    event.payload.button_data.type = button_data->type;
    event.payload.button_data.state = button_data->state;

    TickType_t current_tick = xTaskGetTickCount();

    if (button_data->state == BUTTON_STATE_PRESSED) {
        bool is_long_press = (current_tick - manager->button_ctxs[button_data->type].tick) >
                             pdMS_TO_TICKS(BUTTON_LONG_PRESS_MS);

        event.payload.button_data.press = is_long_press ? BUTTON_PRESS_LONG : BUTTON_PRESS_SHORT;
    } else {
        event.payload.button_data.press = BUTTON_PRESS_NONE;
    }

    manager->button_ctxs[button_data->type].tick = current_tick;

    if (!button_manager_send_hmi_event(&event)) {
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
        case BUTTON_EVENT_TYPE_BUTTON_DATA: {
            return button_manager_event_button_data_handler(manager, &event->payload.button_data);
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

atlas_err_t button_manager_initialize(button_manager_t* manager, button_config_t const* config)
{
    ATLAS_ASSERT(manager && config);

    manager->is_running = false;
    memset(manager->button_ctxs, 0, sizeof(manager->button_ctxs));

    if (!button_manager_send_hmi_notify(HMI_NOTIFY_BUTTON_READY)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}
