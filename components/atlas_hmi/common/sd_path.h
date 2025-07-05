#ifndef COMMON_SD_PATH_H
#define COMMON_SD_PATH_H

#define SD_PATH_PATH_MAX_LEN (100U)

typedef struct {
    char path[SD_PATH_PATH_MAX_LEN];
} sd_path_t;

#endif // COMMON_SD_PATH_H