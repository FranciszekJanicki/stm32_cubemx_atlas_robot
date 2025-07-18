#include "common.h"
#include <stdio.h>
#include <usart.h>

__attribute__((used)) int _write(int file, char* ptr, int len)
{
    StreamBufferHandle_t uart_stream_buffer = stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART);
    SemaphoreHandle_t uart_mutex = semaphore_manager_get(SEMAPHORE_TYPE_UART);

    size_t written = 0U;

    if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(10)) == pdPASS) {
        written = xStreamBufferSend(uart_stream_buffer, ptr, len, pdMS_TO_TICKS(10));

        xSemaphoreGive(uart_mutex);
    }

    return written;
}