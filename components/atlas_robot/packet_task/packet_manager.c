#include "packet_manager.h"
#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

static char const* const TAG = "packet_manager";

static inline bool packet_manager_has_packet_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_PACKET));
}

static inline bool packet_manager_send_system_event(system_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SYSTEM), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool packet_manager_send_system_notify(system_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_SYSTEM), notify, eSetBits) == pdPASS;
}

static inline bool packet_manager_receive_packet_event(packet_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_PACKET), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool packet_manager_receive_packet_notify(packet_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0, PACKET_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool packet_manager_packet_spi_transmit(packet_manager_t const* manager,
                                                      uint8_t const* data,
                                                      size_t data_size)
{
    return HAL_SPI_Transmit(manager->config.packet_spi_bus, data, data_size, 100) == HAL_OK;
}

static inline bool packet_manager_packet_spi_receive(packet_manager_t const* manager,
                                                     uint8_t* data,
                                                     size_t data_size)
{
    return HAL_SPI_Receive(manager->config.packet_spi_bus, data, data_size, 100) == HAL_OK;
}

static inline void packet_manager_set_joint_packet_ready_pin(packet_manager_t const* manager,
                                                             atlas_joint_num_t num,
                                                             bool state)
{
    HAL_GPIO_WritePin(manager->config.packet_ctxs[num].joint_packet_ready_gpio,
                      manager->config.packet_ctxs[num].joint_packet_ready_pin,
                      GPIO_PIN_SET);
}

static inline void packet_manager_set_joint_chip_select_pin(packet_manager_t const* manager,
                                                            atlas_joint_num_t num,
                                                            bool state)
{
    HAL_GPIO_WritePin(manager->config.packet_ctxs[num].joint_chip_select_gpio,
                      manager->config.packet_ctxs[num].joint_chip_select_pin,
                      GPIO_PIN_SET);
}

static inline bool packet_manager_send_joint_packet(packet_manager_t* manager,
                                                    atlas_joint_num_t num,
                                                    atlas_joint_packet_t const* packet)
{
    ATLAS_ASSERT(manager && packet);

    packet_manager_set_joint_chip_select_pin(manager, num, false);
    bool result =
        packet_manager_packet_spi_transmit(manager, (uint8_t const*)packet, sizeof(*packet));
    packet_manager_set_joint_chip_select_pin(manager, num, true);

    if (result) {
        packet_manager_set_joint_packet_ready_pin(manager, num, false);
        vTaskDelay(pdMS_TO_TICKS(10));
        packet_manager_set_joint_packet_ready_pin(manager, num, true);
    }

    return result;
}

static inline bool packet_manager_receive_robot_packet(packet_manager_t* manager,
                                                       atlas_joint_num_t num,
                                                       atlas_robot_packet_t* packet)
{
    ATLAS_ASSERT(manager && packet);

    packet_manager_set_joint_chip_select_pin(manager, num, false);
    bool result = packet_manager_packet_spi_receive(manager, (uint8_t*)packet, sizeof(*packet));
    packet_manager_set_joint_chip_select_pin(manager, num, true);

    return result;
}

static atlas_err_t packet_manager_packet_joint_data_handler(
    packet_manager_t* manager,
    atlas_robot_packet_payload_joint_data_t const* joint_data)
{
    ATLAS_ASSERT(manager && joint_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    system_event_t event = {.type = SYSTEM_EVENT_TYPE_JOINT_DATA,
                            .origin = SYSTEM_EVENT_ORIGIN_PACKET};
    event.payload.joint_data = *joint_data;

    if (!packet_manager_send_system_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_robot_packet_handler(packet_manager_t* manager,
                                                       atlas_robot_packet_t const* packet)
{
    ATLAS_ASSERT(manager && packet);
    ATLAS_LOG_FUNC(TAG);

    switch (packet->type) {
        case ATLAS_ROBOT_PACKET_TYPE_JOINT_DATA: {
            return packet_manager_packet_joint_data_handler(manager, &packet->payload.joint_data);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_PACKET;
        }
    }
}

static atlas_err_t packet_manager_notify_robot_packet_ready_handler(packet_manager_t* manager,
                                                                    atlas_joint_num_t num)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    atlas_robot_packet_t packet;
    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        if (!packet_manager_receive_robot_packet(manager, num, &packet)) {
            ATLAS_RET_ON_ERR(packet_manager_robot_packet_handler(manager, &packet));
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_notify_handler(packet_manager_t* manager, packet_notify_t notify)
{
    ATLAS_ASSERT(manager);

    if (notify & PACKET_NOTIFY_ROBOT_PACKET_READY) {
        for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
            if (notify & (1 << num)) {
                ATLAS_RET_ON_ERR(packet_manager_notify_robot_packet_ready_handler(manager, num));
            }
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_start_handler(packet_manager_t* manager,
                                                      packet_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_stop_handler(packet_manager_t* manager,
                                                     packet_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_joints_start_handler(
    packet_manager_t* manager,
    packet_event_payload_joints_start_t const* joints_start)
{
    ATLAS_ASSERT(manager && joints_start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    atlas_joint_packet_t packet = {.type = ATLAS_JOINT_PACKET_TYPE_JOINT_START};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        packet.payload.joint_start;

        if (!packet_manager_send_joint_packet(manager, num, &packet)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_joints_stop_handler(
    packet_manager_t* manager,
    packet_event_payload_joints_stop_t const* joints_stop)
{
    ATLAS_ASSERT(manager && joints_stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    atlas_joint_packet_t packet = {.type = ATLAS_JOINT_PACKET_TYPE_JOINT_STOP};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        packet.payload.joint_start;

        if (!packet_manager_send_joint_packet(manager, num, &packet)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_joints_data_handler(
    packet_manager_t* manager,
    packet_event_payload_joints_data_t const* joints_data)
{
    ATLAS_ASSERT(manager && joints_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    atlas_joint_packet_t packet = {.type = ATLAS_JOINT_PACKET_TYPE_JOINT_DATA};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        packet.payload.joint_data.position = joints_data->positions[num];

        if (!packet_manager_send_joint_packet(manager, num, &packet)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t packet_manager_event_handler(packet_manager_t* manager,
                                                packet_event_t const* event)
{
    ATLAS_ASSERT(manager && event);

    switch (event->type) {
        case PACKET_EVENT_TYPE_START: {
            return packet_manager_event_start_handler(manager, &event->payload.start);
        }
        case PACKET_EVENT_TYPE_STOP: {
            return packet_manager_event_stop_handler(manager, &event->payload.stop);
        }
        case PACKET_EVENT_TYPE_JOINTS_START: {
            return packet_manager_event_joints_start_handler(manager, &event->payload.joints_start);
        }
        case PACKET_EVENT_TYPE_JOINTS_STOP: {
            return packet_manager_event_joints_stop_handler(manager, &event->payload.joints_stop);
        }
        case PACKET_EVENT_TYPE_JOINTS_DATA: {
            return packet_manager_event_joints_data_handler(manager, &event->payload.joints_data);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t packet_manager_process(packet_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    packet_notify_t notify;
    if (packet_manager_receive_packet_notify(&notify)) {
        ATLAS_RET_ON_ERR(packet_manager_notify_handler(manager, notify));
    }

    packet_event_t event;
    while (packet_manager_has_packet_event()) {
        if (packet_manager_receive_packet_event(&event)) {
            ATLAS_RET_ON_ERR(packet_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t packet_manager_initialize(packet_manager_t* manager, packet_config_t const* config)
{
    ATLAS_ASSERT(manager && config);

    manager->is_running = false;
    manager->config = *config;

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        packet_manager_set_joint_packet_ready_pin(manager, num, true);
        packet_manager_set_joint_chip_select_pin(manager, num, true);
    }

    if (!packet_manager_send_system_notify(SYSTEM_NOTIFY_PACKET_READY)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}