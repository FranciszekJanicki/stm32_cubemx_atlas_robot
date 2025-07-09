#include "ui_manager.h"
#include "FreeRTOS.h"
#include "common.h"
#include "task.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char const* const TAG = "ui_manager";

static inline bool ui_manager_has_ui_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_UI));
}

static inline bool ui_manager_send_button_event(button_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_BUTTON), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool ui_manager_send_display_event(display_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_DISPLAY), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool ui_manager_send_system_event(system_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SYSTEM), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool ui_manager_send_system_notify(system_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_SYSTEM), notify, eSetBits) == pdPASS;
}

static inline bool ui_manager_receive_ui_event(ui_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_UI), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool ui_manager_receive_ui_notify(ui_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0, UI_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t ui_manager_event_start_handler(ui_manager_t* manager,
                                                  ui_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    if (manager->is_button_ready) {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!ui_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    if (manager->is_display_ready) {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!ui_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_event_stop_handler(ui_manager_t* manager,
                                                 ui_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!ui_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!ui_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_event_data_handler(ui_manager_t* manager,
                                                 ui_event_payload_data_t const* data)
{
    ATLAS_ASSERT(manager && data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    display_event_t event = {.type = DISPLAY_EVENT_TYPE_DATA};
    event.payload.data = *data;

    if (!ui_manager_send_display_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_event_button_handler(ui_manager_t* manager,
                                                   ui_event_payload_button_t const* button)
{
    ATLAS_ASSERT(manager && button);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    ATLAS_LOG(TAG,
              "button type: %s, press: %s, state: %s",
              button_type_to_string(button->type),
              button_press_to_string(button->press),
              button_state_to_string(button->press));

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_notify_display_ready_handler(ui_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_display_ready = true;

    if (manager->is_running) {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!ui_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_notify_button_ready_handler(ui_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_button_ready = true;

    if (manager->is_running) {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!ui_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_notify_handler(ui_manager_t* manager, ui_notify_t notify)
{
    ATLAS_ASSERT(manager);

    if (notify & UI_NOTIFY_BUTTON_READY) {
        ATLAS_RET_ON_ERR(ui_manager_notify_button_ready_handler(manager));
    }
    if (notify & UI_NOTIFY_DISPLAY_READY) {
        ATLAS_RET_ON_ERR(ui_manager_notify_display_ready_handler(manager));
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t ui_manager_event_handler(ui_manager_t* manager, ui_event_t const* event)
{
    ATLAS_ASSERT(manager && event);

    switch (event->type) {
        case UI_EVENT_TYPE_START: {
            return ui_manager_event_start_handler(manager, &event->payload.start);
        }
        case UI_EVENT_TYPE_STOP: {
            return ui_manager_event_stop_handler(manager, &event->payload.stop);
        }
        case UI_EVENT_TYPE_DATA: {
            return ui_manager_event_data_handler(manager, &event->payload.data);
        }
        case UI_EVENT_TYPE_BUTTON: {
            return ui_manager_event_button_handler(manager, &event->payload.button);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t ui_manager_process(ui_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    ui_notify_t notify;
    if (ui_manager_receive_ui_notify(&notify)) {
        ATLAS_RET_ON_ERR(ui_manager_notify_handler(manager, notify));
    }

    ui_event_t event;
    while (ui_manager_has_ui_event()) {
        if (ui_manager_receive_ui_event(&event)) {
            ATLAS_RET_ON_ERR(ui_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t ui_manager_initialize(ui_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_running = false;
    manager->is_button_ready = false;
    manager->is_display_ready = false;

    return ATLAS_ERR_OK;
}