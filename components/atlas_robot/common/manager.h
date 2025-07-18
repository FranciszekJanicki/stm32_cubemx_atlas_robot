#ifndef COMMON_MANAGER_H
#define COMMON_MANAGER_H

#include "FreeRTOS.h"
#include "handle_manager.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"

typedef enum {
    TASK_TYPE_SYSTEM,
    TASK_TYPE_PACKET,
    TASK_TYPE_KINEMATICS,
    TASK_TYPE_HMI,
    TASK_TYPE_BUTTON,
    TASK_TYPE_DISPLAY,
    TASK_TYPE_SD,
    TASK_TYPE_UART,
    TASK_TYPE_NUM,
} TaskType_t;

typedef enum {
    QUEUE_TYPE_SYSTEM,
    QUEUE_TYPE_PACKET,
    QUEUE_TYPE_KINEMATICS,
    QUEUE_TYPE_DISPLAY,
    QUEUE_TYPE_BUTTON,
    QUEUE_TYPE_HMI,
    QUEUE_TYPE_SD,
    QUEUE_TYPE_NUM,
} QueueType_t;

typedef enum {
    STREAM_BUFFER_TYPE_UART,
    STREAM_BUFFER_TYPE_NUM,
} StreamBufferType_t;

typedef enum {
    SEMAPHORE_TYPE_UART,
    SEMAPHORE_TYPE_NUM,
} SemaphoreType_t;

DECLARE_HANDLE_MANAGER(task, TaskType_t, TaskHandle_t, TASK_TYPE_NUM)
DECLARE_HANDLE_MANAGER(queue, QueueType_t, QueueHandle_t, QUEUE_TYPE_NUM)
DECLARE_HANDLE_MANAGER(stream_buffer,
                       StreamBufferType_t,
                       StreamBufferHandle_t,
                       STREAM_BUFFER_TYPE_NUM)
DECLARE_HANDLE_MANAGER(semaphore, SemaphoreType_t, SemaphoreHandle_t, SEMAPHORE_TYPE_NUM)

#endif // COMMON_MANAGER_H