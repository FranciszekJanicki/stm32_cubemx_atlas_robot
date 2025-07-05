#include "atlas_manager.h"

DEFINE_HANDLE_MANAGER(atlas_task, AtlasTaskType_t, TaskHandle_t, ATLAS_TASK_TYPE_NUM)
DEFINE_HANDLE_MANAGER(atlas_queue, AtlasQueueType_t, QueueHandle_t, ATLAS_QUEUE_TYPE_NUM)
DEFINE_HANDLE_MANAGER(atlas_stream_buffer,
                      StreamBufferType_t,
                      StreamBufferHandle_t,
                      ATLAS_STREAM_BUF_TYPE_NUM)
DEFINE_HANDLE_MANAGER(atlas_semaphore,
                      AtlasSemaphoreType_t,
                      SemaphoreHandle_t,
                      ATLAS_SEMAPHORE_TYPE_NUM)