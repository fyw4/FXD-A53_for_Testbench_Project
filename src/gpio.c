/*
 * gpio.c
 *
 *  Created on: 2026.3.4
 *      Author: Wang Chyng-chuan
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <asm/ioctl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "gpio.h"
#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"

static int global_fire_extinguish_flag = READY;
static int global_fire_extinguish_ack_flag = READY;
static int global_video_record_flag = READY;

static int global_DO1_open_circuit_flag = READY;
static int global_DO4_open_circuit_flag = READY;
static int global_DO5_open_circuit_flag = READY;
static int global_underpressure_flag = READY;

static int global_count = 0;

timer_t global_fire_timer_id;
timer_t global_reboot_and_reset_timer_id;
timer_t global_video_record_timer_id;

int init_gpio(char *name)
{
    gpiofd = open(name, O_RDWR);
    if (gpiofd < 0)
    {
        _CCU_ERROR_(ERROR_GPIO_OPEN, name);
        return 1;
    }
    return 0;
}

int reset_LED()
{
    set_gpio_status(LED_SYSTEM_RUN, LED_OFF);
    set_gpio_status(LED_FAULT, LED_OFF);
    set_gpio_status(LED_FIRE_ARLAM, LED_OFF);
    set_gpio_status(LED_FIRE_EXTINGUISH, LED_OFF);
    set_gpio_status(LED_MAINTENANCE, LED_OFF);
    set_gpio_status(LED_RESERVE, LED_OFF);

    return 0;
}

int set_LED()
{
    set_gpio_status(LED_SYSTEM_RUN, LED_ON);
    set_gpio_status(LED_FAULT, LED_ON);
    set_gpio_status(LED_FIRE_ARLAM, LED_ON);
    set_gpio_status(LED_FIRE_EXTINGUISH, LED_ON);
    set_gpio_status(LED_MAINTENANCE, LED_ON);
    set_gpio_status(LED_RESERVE, LED_ON);

    return 0;
}

size_t getFileSize(const char *fileName)
{
    if (fileName == NULL)
    {
        return 0;
    }

    struct stat statbuf;

    stat(fileName, &statbuf);

    size_t filesize = statbuf.st_size;

    return filesize;
}

void set_bit(unsigned char *byte, int bit)
{
    *byte |= (1 << bit);
}

// ??3y???¡§???a0
void clear_bit(unsigned char *byte, int bit)
{
    *byte &= ~(1 << bit);
}

void thread_func_gpio()
{
    while (1)
    {
        if (extinguish_info_message_to_Workbench == NULL)
        {
            usleep(100000); // 100ms
            continue;
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 0 & 0x01)
        {
            set_gpio_status(FIRE_EXTINGUISH_A1, POWER_ON);
        }
        else
        {
            set_gpio_status(FIRE_EXTINGUISH_A1, POWER_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 1 & 0x01)
        {
            set_gpio_status(FIRE_EXTINGUISH_A2, POWER_ON);
        }
        else
        {
            set_gpio_status(FIRE_EXTINGUISH_A2, POWER_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 4 & 0x01)
        {
            set_gpio_status(GPIO2_IO07, POWER_ON);
        }
        else
        {
            set_gpio_status(GPIO2_IO07, POWER_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 5 & 0x01)
        {
            set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, POWER_ON);
        }
        else
        {
            set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, POWER_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 6 & 0x01)
        {
            set_gpio_status(DC24V_POWER_SUPPLY_1, POWER_ON);
        }
        else
        {
            set_gpio_status(DC24V_POWER_SUPPLY_1, POWER_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 7 & 0x01)
        {
            set_gpio_status(DC24V_POWER_SUPPLY_2, POWER_ON);
        }
        else
        {
            set_gpio_status(DC24V_POWER_SUPPLY_2, POWER_OFF);
        }

        // system run LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 0 & 0x01)
        {
            set_gpio_status(LED_SYSTEM_RUN, LED_ON);
        }
        else
        {
            set_gpio_status(LED_SYSTEM_RUN, LED_OFF);
        }

        // fault LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 1 & 0x01)
        {
            set_gpio_status(LED_FAULT, LED_ON);
        }
        else
        {
            set_gpio_status(LED_FAULT, LED_OFF);
        }

        // alarm LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 2 & 0x01)
        {
            set_gpio_status(LED_FIRE_ARLAM, LED_ON);
        }
        else
        {
            set_gpio_status(LED_FIRE_ARLAM, LED_OFF);
        }

        // extinguish LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 3 & 0x01)
        {
            set_gpio_status(LED_FIRE_EXTINGUISH, LED_ON);
        }
        else
        {
            set_gpio_status(LED_FIRE_EXTINGUISH, LED_OFF);
        }

        // maintance LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 4 & 0x01)
        {
            set_gpio_status(LED_MAINTENANCE, LED_ON);
        }
        else
        {
            set_gpio_status(LED_MAINTENANCE, LED_OFF);
        }

        // reserved LED
        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 >> 5 & 0x01)
        {
            set_gpio_status(LED_RESERVE, LED_ON);
        }
        else
        {
            set_gpio_status(LED_RESERVE, LED_OFF);
        }

        // extinguish_info_message_to_Workbench->self_status_byte_NO_1
        if (get_gpio_status(EXTINGUISH_ACK))
        {
            set_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_1, 0);
        }
        else
        {
            clear_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_1, 0);
        }

        if (get_gpio_status(OPEN_CIRCUIT))
        {
            set_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_1, 3);
        }
        else
        {
            clear_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_1, 3);
        }

         // extinguish_info_message_to_Workbench->self_status_byte_NO_2
        if (!get_gpio_status(BUTTON_REBOOT))
        {
            set_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 0);
        }
        else
        {
            clear_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 0);
        }

        if (get_gpio_status(DC24V_POWER_SUPPLY_1_FAULT))
        {
            set_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 1);
        }
        else
        {
            clear_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 1);
        }

        if (get_gpio_status(DC24V_POWER_SUPPLY_2_FAULT))
        {
            set_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 2);
        }
        else
        {
            clear_bit(&extinguish_info_message_to_Workbench->self_status_byte_NO_2, 2);
        }

        extinguish_info_message_to_Workbench->sw_choose_val = get_gpio_status(SW1) << 3 |
                                                              get_gpio_status(SW2) << 2 |
                                                              get_gpio_status(SW3) << 1 |
                                                              get_gpio_status(SW4) << 0;

        sleep(1);
    }

    return;
}

void thread_func_power_supply()
{
    sleep(1);

    int time_count = 0;
    int open_circuit_count = 0;
    int DC_24V_power_supply_short_circuit_flag = 0;
    int DC_24V_power_supply_1_open_circuit_flag = 0;
    int DC_24V_power_supply_2_open_circuit_flag = 0;

    int DC_24V_power_supply_1_set_flag = 0;
    int DC_24V_power_supply_2_set_flag = 0;

    // make sure the power supply is off at begining
    set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, POWER_OFF);
    set_gpio_status(DC24V_POWER_SUPPLY_1, POWER_OFF);
    set_gpio_status(DC24V_POWER_SUPPLY_2, POWER_OFF);

    sleep(3);

    set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, POWER_ON);
    _CCU_LOG_("Camera DC 24V power supply OK!");

    set_gpio_status(DC24V_POWER_SUPPLY_1, POWER_ON);
    _CCU_LOG_("DC 24V power NO.1 supply OK!");

    set_gpio_status(DC24V_POWER_SUPPLY_2, POWER_ON);
    _CCU_LOG_("DC 24V power NO.2 supply OK!");

    while (1)
    {
        sleep(1);
    }

    return;
}

int init_GPIO_DO()
{
    set_gpio_status(FIRE_EXTINGUISH_A1, POWER_OFF);
    set_gpio_status(FIRE_EXTINGUISH_A2, POWER_OFF);
    set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, POWER_OFF);
    set_gpio_status(DC24V_POWER_SUPPLY_1, POWER_OFF);
    set_gpio_status(DC24V_POWER_SUPPLY_2, POWER_OFF);
    set_gpio_status(LED_SYSTEM_RUN, LED_OFF);
    set_gpio_status(LED_FAULT, LED_OFF);
    set_gpio_status(LED_FIRE_ARLAM, LED_OFF);
    set_gpio_status(LED_FIRE_EXTINGUISH, LED_OFF);
    set_gpio_status(LED_MAINTENANCE, LED_OFF);
    set_gpio_status(LED_RESERVE, LED_OFF);
    return 0;
}

int init_DIP_switch()
{
    char exec_message[512] = {0};
    char dest_ip_file[128] = {0};
    char tmp_file[128] = {0};
    char tmp_file_create[256] = {0};
    char tmp_file_rm[256] = {0};
    char chmod_for_converter_cabinet_1_ip_file_for_A[128] = {0};
    char chmod_for_converter_cabinet_2_ip_file_for_A[128] = {0};
    char chmod_for_converter_cabinet_1_ip_file_for_B[128] = {0};
    char chmod_for_converter_cabinet_2_ip_file_for_B[128] = {0};
    char chmod_for_converter_cabinet_1_ip_file_for_C[128] = {0};
    char chmod_for_converter_cabinet_2_ip_file_for_C[128] = {0};
    char chmod_cabinet_1_message_for_A[512] = {0};
    char chmod_cabinet_2_message_for_A[512] = {0};
    char chmod_cabinet_1_message_for_B[512] = {0};
    char chmod_cabinet_2_message_for_B[512] = {0};
    char chmod_cabinet_1_message_for_C[512] = {0};
    char chmod_cabinet_2_message_for_C[512] = {0};

    sw_choose |= get_gpio_status(SW1) << 3;
    sw_choose |= get_gpio_status(SW2) << 2;
    sw_choose |= get_gpio_status(SW3) << 1;
    sw_choose |= get_gpio_status(SW4) << 0;

    sprintf(tmp_file, "%s", TEMP_FILE);
    sprintf(tmp_file_create, "touch %s", tmp_file);
    system(tmp_file_create);

    memcpy(chmod_for_converter_cabinet_1_ip_file_for_A, IP_FILE_for_CONVERTER_CABINET_1_for_A, strlen(IP_FILE_for_CONVERTER_CABINET_1_for_A));
    sprintf(chmod_cabinet_1_message_for_A, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_1_ip_file_for_A, tmp_file);
    system(chmod_cabinet_1_message_for_A);

    if (getFileSize(tmp_file) > 0)
    {
        set_ip_flag = 5;
    }

    memcpy(chmod_for_converter_cabinet_2_ip_file_for_A, IP_FILE_for_CONVERTER_CABINET_2_for_A, strlen(IP_FILE_for_CONVERTER_CABINET_2_for_A));
    sprintf(chmod_cabinet_2_message_for_A, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_2_ip_file_for_A, tmp_file);
    system(chmod_cabinet_2_message_for_A);

    if (getFileSize(tmp_file) > 0)
    {
        set_ip_flag = 5;
    }

    memcpy(chmod_for_converter_cabinet_1_ip_file_for_B, IP_FILE_for_CONVERTER_CABINET_1_for_B, strlen(IP_FILE_for_CONVERTER_CABINET_1_for_B));
    sprintf(chmod_cabinet_1_message_for_B, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_1_ip_file_for_B, tmp_file);
    system(chmod_cabinet_1_message_for_B);

    if (getFileSize(tmp_file) > 0)
    {
        set_ip_flag = 5;
    }

    memcpy(chmod_for_converter_cabinet_2_ip_file_for_B, IP_FILE_for_CONVERTER_CABINET_2_for_B, strlen(IP_FILE_for_CONVERTER_CABINET_2_for_B));
    sprintf(chmod_cabinet_2_message_for_B, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_2_ip_file_for_B, tmp_file);
    system(chmod_cabinet_2_message_for_B);

    if (getFileSize(tmp_file) > 0)
    {
        set_ip_flag = 5;
    }

    sprintf(tmp_file_rm, "rm -rf %s", tmp_file);
    system(tmp_file_rm);

    memcpy(dest_ip_file, IP_FILE_LOCATION, strlen(IP_FILE_LOCATION));

    if (0 == set_ip_flag)
    {
        if (sw_choose == SW2_CODE) // switch 2, 192.168.62.12, cab A-2
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_2_ip_file_for_A, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_2_FLAG;                         // A-2
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_2_FLAG_for_HKPro; // 0x02
            set_ip_flag = 2;
        }
        else if (sw_choose == SW3_CODE) // switch 3, 192.168.62.61, cab B-1
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_1_ip_file_for_B, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_1_FLAG;                         // B-1
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_3_FLAG_for_HKPro; // 0x03
            set_ip_flag = 1;
        }
        else if (sw_choose == SW4_CODE) // switch 4, 192.168.62.62, cab B-2
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_2_ip_file_for_B, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_2_FLAG;                         // B-2
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_4_FLAG_for_HKPro; // 0x04
            set_ip_flag = 2;
        }
        else // default switch 1, 192.168.62.11, cab A-1
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_1_ip_file_for_A, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_1_FLAG;                         // A-1
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_1_FLAG_for_HKPro; // 0x01
            set_ip_flag = 1;
        }

        system(exec_message);
        sleep(1);
        system("systemctl restart systemd-resolved.service");
        sleep(1);
        system("systemctl restart systemd-networkd.service");

        _CCU_LOG_(exec_message);
    }
    else
    {
        _CCU_LOG_("DIP switch failed");
        return -1;
    }

    _CCU_LOG_("DIP switch code is %02x, flag is %02x", sw_choose, set_ip_flag);

    return 0;
}