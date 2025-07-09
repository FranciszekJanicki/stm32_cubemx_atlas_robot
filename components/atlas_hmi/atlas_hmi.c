#include "atlas_hmi.h"
#include "kinematics_task.h"
#include "packet_task.h"
#include "sd_task.h"
#include "system_task.h"
#include "uart_task.h"
#include "ui_task.h"
#include "usart.h"

#define UART_TASK_NAME ("uart_task")
#define UART_TASK_PRIORITY (1U)
#define UART_TASK_STACK_DEPTH (5000U / sizeof(StackType_t))

#define UART_BUFFER_SIZE (1024U)

#define UART_STREAM_BUFFER_STORAGE_SIZE (1024U)
#define UART_STREAM_BUFFER_TRIGGER (1U)

void uart_stream_buffer_initialize(void)
{
    static StaticStreamBuffer_t uart_stream_buffer_buffer;
    static uint8_t uart_stream_buffer_storage[UART_STREAM_BUFFER_STORAGE_SIZE];

    StreamBufferHandle_t uart_stream_buffer =
        uart_task_create_stream_buffer(&uart_stream_buffer_buffer,
                                       UART_STREAM_BUFFER_TRIGGER,
                                       UART_STREAM_BUFFER_STORAGE_SIZE,
                                       uart_stream_buffer_storage);

    stream_buffer_manager_set(STREAM_BUFFER_TYPE_UART, uart_stream_buffer);
}

void uart_task_initialize(void)
{
    static StaticTask_t uart_task_buffer;
    static StackType_t uart_task_stack[UART_TASK_STACK_DEPTH];

    static uint8_t uart_buffer[UART_BUFFER_SIZE];

    static uart_task_ctx_t uart_task_ctx = {.uart = &huart2,
                                            .uart_buffer = uart_buffer,
                                            .uart_action = UART_ACTION_TRANSMIT,
                                            .uart_buffer_size = UART_BUFFER_SIZE};
    uart_task_ctx.stream_buffer = stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART);

    TaskHandle_t uart_task = uart_task_create_task(&uart_task_ctx,
                                                   UART_TASK_NAME,
                                                   &uart_task_buffer,
                                                   UART_TASK_PRIORITY,
                                                   uart_task_stack,
                                                   UART_TASK_STACK_DEPTH);
}

void atlas_hmi_initialize(void)
{
    uart_stream_buffer_initialize();
    system_queue_initialize();
    ui_queue_intialize();
    packet_queue_initialize();
    kinematics_queue_initialize();

    uart_task_initialize();
    ui_task_initialize();
    system_task_initialize();
    packet_task_initialize();
    kinematics_task_initialize();
}

#undef UART_TASK_NAME
#undef UART_TASK_PRIORITY
#undef UART_TASK_STACK_DEPTH

#undef UART_BUFFER_SIZE

#undef UART_STREAM_BUFFER_STORAGE_SIZE
#undef UART_STREAM_BUFFER_TRIGGER