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
            set_gpio_status(FIRE_EXTINGUISH_A1, LED_ON);
        }
        else
        {
            set_gpio_status(FIRE_EXTINGUISH_A1, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 1 & 0x01)
        {
            set_gpio_status(FIRE_ALARM_to_TCU, LED_ON);
        }
        else
        {
            set_gpio_status(FIRE_ALARM_to_TCU, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 2 & 0x01)
        {
            set_gpio_status(FIRE_EXTINGUISH_REVERSED, LED_ON);
        }
        else
        {
            set_gpio_status(FIRE_EXTINGUISH_REVERSED, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 3 & 0x01)
        {
            set_gpio_status(NORMALLY_OPEN_EXTINGUISH_A4, LED_ON);
        }
        else
        {
            set_gpio_status(NORMALLY_OPEN_EXTINGUISH_A4, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 4 & 0x01)
        {
            set_gpio_status(NORMALLY_CLOSED_EXTINGUISH_A5, LED_ON);
        }
        else
        {
            set_gpio_status(NORMALLY_CLOSED_EXTINGUISH_A5, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 5 & 0x01)
        {
            set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, LED_ON);
        }
        else
        {
            set_gpio_status(CAMERA_DC24V_POWER_SUPPLY, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 6 & 0x01)
        {
            set_gpio_status(DC24V_POWER_SUPPLY_1, LED_ON);
        }
        else
        {
            set_gpio_status(DC24V_POWER_SUPPLY_1, LED_OFF);
        }

        if (extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 >> 7 & 0x01)
        {
            set_gpio_status(DC24V_POWER_SUPPLY_2, LED_ON);
        }
        else
        {
            set_gpio_status(DC24V_POWER_SUPPLY_2, LED_OFF);
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

        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DI1) & 0x01) << 0;
        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DI2) & 0x01) << 1;
        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DI3) & 0x01) << 2;
        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DO1_OPEN_CIRCUIT) & 0x01) << 3;
        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DO4_OPEN_CIRCUIT) & 0x01) << 4;
        extinguish_info_message_to_Workbench->self_status_byte_NO_1 |= (get_gpio_status(DO5_OPEN_CIRCUIT) & 0x01) << 5;

        extinguish_info_message_to_Workbench->self_status_byte_NO_2 |= (get_gpio_status(BUTTON_REBOOT) & 0x01) << 0;
        extinguish_info_message_to_Workbench->self_status_byte_NO_2 |= (get_gpio_status(DC24V_POWER_SUPPLY_1_FAULT) & 0x01) << 1;
        extinguish_info_message_to_Workbench->self_status_byte_NO_2 |= (get_gpio_status(DC24V_POWER_SUPPLY_2_FAULT) & 0x01) << 2;

        sw_choose |= get_gpio_status(SW1) << 3;
        sw_choose |= get_gpio_status(SW2) << 2;
        sw_choose |= get_gpio_status(SW3) << 1;
        sw_choose |= get_gpio_status(SW4) << 0;
        extinguish_info_message_to_Workbench->sw_choose_val = sw_choose;

        usleep(500000);
    }

    return;
}

void fire_reboot_and_reset_handle()
{
    global_count++;
    if (global_count > 255)
    {
        global_count = LOGIN_WAIT_TIME_MIN;
    }

    return;
}

void fire_reboot_and_reset_timer_init()
{
    struct sigevent sig_event;
    struct sigaction sig_act;

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    sig_act.sa_handler = fire_reboot_and_reset_handle;
    if (sigaction(SIGRTMAX - 2, &sig_act, NULL) < 0)
    {
        return;
    }

    sig_event.sigev_notify = SIGEV_SIGNAL;
    sig_event.sigev_signo = SIGRTMAX - 2;
    if (timer_create(CLOCK_REALTIME, &sig_event, &global_reboot_and_reset_timer_id) < 0)
    {
        return;
    }

    return;
}

void fire_interval_timer_set(timer_t *timer_id, int time)
{
    struct itimerspec timer_setting;
    timer_setting.it_value.tv_sec = time;
    timer_setting.it_value.tv_nsec = 0;
    timer_setting.it_interval.tv_sec = time;
    timer_setting.it_interval.tv_nsec = 0;
    if (timer_settime(*timer_id, 0, &timer_setting, NULL) < 0)
    {
        return;
    }

    return;
}

void thread_func_reboot()
{
    int count_flag = 0;
    int reboot_flag = 0;

    fire_reboot_and_reset_timer_init();
    while (1)
    {
        if (!get_gpio_status(BUTTON_REBOOT))
        {
            if (0 == count_flag)
            {
                fire_interval_timer_set(&global_reboot_and_reset_timer_id, COUNT_TIME);
                count_flag = 1;
            }
        }
        else
        {
            count_flag = 0;
            fire_interval_timer_set(&global_reboot_and_reset_timer_id, 0);

            if (REBOOT_WAIT_TIME_MIN <= global_count && global_count < LOGIN_WAIT_TIME_MIN) // reboot time effective value: 0.5s~5s
            {
                if (0 == reboot_flag)
                {
                    _CCU_LOG_("Reboot button pressed!");
                    reboot_flag = 1;
                    raise(SIGRTMAX);
                }
            }
            else if (LOGIN_WAIT_TIME_MIN <= global_count) // login time effective value: >= 5s
            {
                if (0 == reboot_flag)
                {
                    can_login_flag = 1;
                    _CCU_LOG_("Login detect and reboot !");

                    set_LED();

                    reboot_flag = 1;
                }
            }
            else
            {
                global_count = 0;
            }
        }

        usleep(100000); // 100ms
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

    memcpy(chmod_for_converter_cabinet_1_ip_file_for_C, IP_FILE_for_CONVERTER_CABINET_1_for_C, strlen(IP_FILE_for_CONVERTER_CABINET_1_for_C));
    sprintf(chmod_cabinet_1_message_for_C, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_1_ip_file_for_C, tmp_file);
    system(chmod_cabinet_1_message_for_C);

    if (getFileSize(tmp_file) > 0)
    {
        set_ip_flag = 5;
    }

    memcpy(chmod_for_converter_cabinet_2_ip_file_for_C, IP_FILE_for_CONVERTER_CABINET_2_for_C, strlen(IP_FILE_for_CONVERTER_CABINET_2_for_C));
    sprintf(chmod_cabinet_2_message_for_C, "chmod 777 %s 2> %s", chmod_for_converter_cabinet_2_ip_file_for_C, tmp_file);
    system(chmod_cabinet_2_message_for_C);

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
        else if (sw_choose == SW5_CODE) // switch 5, 192.168.62.111, cab C-1
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_1_ip_file_for_C, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_2_FLAG;                         // C-1
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_5_FLAG_for_HKPro; // 0x05
            set_ip_flag = 1;
        }
        else if (sw_choose == SW6_CODE) // switch 6, 192.168.62.112, cab C-2
        {
            sprintf(exec_message, "cp -f %s %s", chmod_for_converter_cabinet_2_ip_file_for_C, dest_ip_file);
            eth_converter_cabinet_No = CONVERTER_CABINET_NO_2_FLAG;                         // C-2
            eth_converter_cabinet_No_send_to_HKPro = CONVERTER_CABINET_NO_6_FLAG_for_HKPro; // 0x06
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