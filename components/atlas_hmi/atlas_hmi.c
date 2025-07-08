#include "atlas_hmi.h"
#include "kinematics_task.h"
#include "packet_task.h"
#include "sd_task.h"
#include "system_task.h"
#include "ui_task.h"

void atlas_hmi_initialize(void)
{
    system_queue_initialize();
    ui_queue_intialize();
    packet_queue_initialize();
    kinematics_queue_initialize();

    ui_task_initialize();
    system_task_initialize();
    packet_task_initialize();
    kinematics_task_initialize();
}