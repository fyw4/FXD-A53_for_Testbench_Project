/*
 *  mcp3201.h
 *
 *  Created on: 2023.10.30
 *      Author: Wang Qingchuan
 */
#ifndef MCP3201_H_
#define MCP3201_H_

#define SENSOR_FLOAT_DATA_GET(ret, index, str, member) \
    ret = file_data_read(file_path[index], str);       \
    dev->member = atof(str);

#define SENSOR_INT_DATA_GET(ret, index, str, member) \
    ret = file_data_read(file_path[index], str);     \
    dev->member = atoi(str);

static char *file_path[] = {
    "/sys/bus/iio/devices/iio:device0/in_voltage_scale",
    "/sys/bus/iio/devices/iio:device0/in_voltage0_raw",
};

#define AI_PORT 0
#define AI_TEST_PORT 1
#define PRESS_LIMIT_VAL 5.6 // 5.6 * 249 / 3 * 2
#define PRESS_OPEN_VAL 2.5 // 4 *249 / 3 * 2
#define WARNING_WAIT_TIME 10

typedef enum warning_limit_flag_stat
{
    WARNING_LIMIT_READY = 0,
    WARNING_LIMIT_OCCUPIED
} WARNING_LIMIT_FLAG_STAT;

typedef enum warning_unattached_flag_stat
{
    WARNING_ATTACHED_READY = 0,
    WARNING_ATTACHED_OCCUPIED
} WARNING_UNATTACHED_FLAG_STAT;

typedef enum path_index
{
    IN_VOLTAGE_SCALE = 0,
    IN_VOLTAGE_RAW,
} PATH_INDEX;

typedef struct adc_dev
{
    int raw;
    float scale;
    float act;
} ADC_DEV;

void thread_func_AI_detect();

#endif