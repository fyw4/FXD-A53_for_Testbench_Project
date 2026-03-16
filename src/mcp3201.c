/*
 * mcp3201.c
 *
 *  Created on: 2023.10.30
 *      Author: Wang Chyng-chuan
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <asm/ioctl.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mcp3201.h"
#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"

static int global_warning_limit_flag = WARNING_LIMIT_READY;
static int global_warning_unattached_flag = WARNING_LIMIT_READY;

ADC_DEV mcp3201adc;

static int file_data_read(char *filename, char *str)
{
    int ret = 0;
    FILE *data_stream;

    data_stream = fopen(filename, "r"); /* only read */
    if (data_stream == NULL)
    {
        printf("can't open file %s\r\n", filename);
        return -1;
    }

    ret = fscanf(data_stream, "%s", str);
    if (!ret)
    {
        printf("file read error!\r\n");
    }
    else if (ret == EOF)
    {
        fseek(data_stream, 0, SEEK_SET);
    }
    fclose(data_stream); /* close file */
    return 0;
}

static int adc_read(ADC_DEV *dev)
{
    int ret = 0;
    char str[50];

    SENSOR_FLOAT_DATA_GET(ret, IN_VOLTAGE_SCALE, str, scale);
    SENSOR_INT_DATA_GET(ret, IN_VOLTAGE_RAW, str, raw);

    dev->act = (dev->scale * dev->raw) / 1000.f;
    return ret;
}

void thread_func_AI_detect()
{
    int ret = 0;
    int time_count_low = 0;
    int time_count_unattached = 0;
    float sum_val = 0;
    int avrage_count = 0;

    while (1)
    {
        if (extinguish_info_message_to_Workbench == NULL)
        {
            usleep(100000); // 100ms
            continue;
        }

        ret = adc_read(&mcp3201adc);
        if (ret == 0)
        {
            sum_val += mcp3201adc.raw / 2.0f * 3.0f / 249.0f; // ADC_RAW
            if (avrage_count >= 10)
            {
                extinguish_info_message_to_Workbench->pressure_simulate_value_NO_1 = (unsigned char)(((unsigned char)sum_val / avrage_count) * 10);
                sum_val = 0;
                avrage_count = 0;
            }
        }

        avrage_count++;
        usleep(100000); /*100ms */
    }
    return;
}