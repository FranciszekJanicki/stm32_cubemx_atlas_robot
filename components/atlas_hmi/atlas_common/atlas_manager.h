#ifndef ATLAS_COMMON_ATLAS_MANAGER_H
#define ATLAS_COMMON_ATLAS_MANAGER_H

#include "FreeRTOS.h"
#include "handle_manager.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"

typedef enum {
    ATLAS_TASK_TYPE_SYSTEM,
    ATLAS_TASK_TYPE_PACKET,
    ATLAS_TASK_TYPE_HMI,
    ATLAS_TASK_TYPE_SD,
    ATLAS_TASK_TYPE_NUM,
} AtlasTaskType_t;

typedef enum {
    ATLAS_QUEUE_TYPE_SYSTEM,
    ATLAS_QUEUE_TYPE_PACKET,
    ATLAS_QUEUE_TYPE_HMI,
    ATLAS_QUEUE_TYPE_SD,
    ATLAS_QUEUE_TYPE_NUM,
} AtlasQueueType_t;

typedef enum {
    ATLAS_STREAM_BUF_TYPE_UART,
    ATLAS_STREAM_BUF_TYPE_NUM,
} AtlasStreamBufType_t;

typedef enum {
    ATLAS_SEMAPHORE_TYPE_UART,
    ATLAS_SEMAPHORE_TYPE_NUM,
} AtlasSemaphoreType_t;

DECLARE_HANDLE_MANAGER(atlas_hmi_task, AtlasTaskType_t, TaskHandle_t, ATLAS_TASK_TYPE_NUM)
DECLARE_HANDLE_MANAGER(atlas_hmi_queue, AtlasQueueType_t, QueueHandle_t, ATLAS_QUEUE_TYPE_NUM)
DECLARE_HANDLE_MANAGER(atlas_hmi_stream_buf,
                       AtlasStreamBufType_t,
                       StreamBufferHandle_t,
                       ATLAS_STREAM_BUF_TYPE_NUM)
DECLARE_HANDLE_MANAGER(atlas_hmi_semaphore,
                       AtlasSemaphoreType_t,
                       SemaphoreHandle_t,
                       ATLAS_SEMAPHORE_TYPE_NUM)

#endif // ATLAS_COMMON_ATLAS_MANAGER_H