#include "display_manager.h"
#include <string.h>

static char const* const TAG = "display_manager";

static inline bool display_manager_receive_display_notify(display_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0x00, DISPLAY_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool display_manager_send_ui_notify(ui_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_UI), notify, eSetBits) == pdPASS;
}

static inline bool display_manager_receive_display_event(display_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_DISPLAY), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool display_manager_has_display_event(void)
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_DISPLAY)) == pdPASS;
}

static inline bool display_manager_send_ui_event(ui_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_UI), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t display_manager_notify_handler(display_manager_t* manager,
                                                  display_notify_t notify)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    return ATLAS_ERR_OK;
}

static atlas_err_t display_manager_event_start_handler(display_manager_t* manager,
                                                       display_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t display_manager_event_stop_handler(display_manager_t* manager,
                                                      display_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t display_manager_event_data_handler(display_manager_t* manager,
                                                      display_event_payload_data_t const* data)
{
    ATLAS_ASSERT(manager && data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    atlas_print_data(data);

    return ATLAS_ERR_OK;
}

static atlas_err_t display_manager_event_path_handler(display_manager_t* manager,
                                                      display_event_payload_path_t const* path)
{
    ATLAS_ASSERT(manager && path);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    atlas_print_path(path);

    return ATLAS_ERR_OK;
}

static atlas_err_t display_manager_event_handler(display_manager_t* manager,
                                                 display_event_t const* event)
{
    ATLAS_ASSERT(manager && event);
    ATLAS_LOG_FUNC(TAG);

    switch (event->type) {
        case DISPLAY_EVENT_TYPE_START: {
            return display_manager_event_start_handler(manager, &event->payload.start);
        }
        case DISPLAY_EVENT_TYPE_STOP: {
            return display_manager_event_stop_handler(manager, &event->payload.stop);
        }
        case DISPLAY_EVENT_TYPE_DATA: {
            return display_manager_event_data_handler(manager, &event->payload.data);
        }
        case DISPLAY_EVENT_TYPE_PATH: {
            return display_manager_event_path_handler(manager, &event->payload.path);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t display_manager_process(display_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    display_notify_t notify;
    if (display_manager_receive_display_notify(&notify)) {
        ATLAS_RET_ON_ERR(display_manager_notify_handler(manager, notify));
    }

    display_event_t event;
    while (display_manager_has_display_event()) {
        if (display_manager_receive_display_event(&event)) {
            ATLAS_RET_ON_ERR(display_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t display_manager_initialize(display_manager_t* manager, display_config_t const* config)
{
    ATLAS_ASSERT(manager && config);

    manager->is_running = false;

    if (!display_manager_send_ui_notify(UI_NOTIFY_DISPLAY_READY)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}
