#ifndef COMMON_SD_H
#define COMMON_SD_H

#include "atlas_robot.h"

#define SD_PATH_MAX_LEN (100U)

typedef struct {
    char sd_path[SD_PATH_MAX_LEN];
    atlas_robot_path_t robot_path;
} sd_save_path_t;

typedef struct {
    char sd_path[SD_PATH_MAX_LEN];
} sd_load_path_t;

#endif // COMMON_SD_H