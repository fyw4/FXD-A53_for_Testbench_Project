/*
 * GlobalVariables.c
 *
 *  Created on: 2023.09.08
 *      Author: Wang Chyng-chuan
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"

char filepath_cculog[1024] = {0};
char filename_cculog[1024] = {0};

char filepath_datalog[1024] = {0};
char filename_datalog[1024] = {0};

char filepath_can_login_datalog[1024] = {0};
char filename_can_login_datalog[1024] = {0};

unsigned char loco_and_num_buff[12] = {0};

EXTINGUISH_INFO_MESSAGE_to_Workbench *extinguish_info_message_to_Workbench = NULL;
EXTINGUISH_INFO_MESSAGE_to_6A *detect_msg_to_6A_info = NULL;
CAN_ONLINE_AND_COUNT can_once_online_status[CAN_SENSOR_NUM] = {0};

int gpiofd = 0;
int comfd_ttymxc0 = 0;
int comfd_ttymxc1 = 0;
int comfd_ttymxc2 = 0;
int comfd_ttymxc3 = 0;
int canfd[3] = {0};
int watchdogfd = 0;
int ethsendfd2cc1 = 0;
int ethsendfd2cc2 = 0;
int ethrecvfd = 0;
volatile int can_temperature_rise_pressed = 0;
volatile int can_alarm_pressed = 0;
volatile int camera_alarm_pressed = 0;
volatile int camera_begin_record_video_flag = 0;
volatile int extinguish_ack = 0;
volatile int can_sensor_precaution_status = 0;
volatile int can_sensor_smoke_status = 0;
volatile int power_short_circuit_status = 0;
volatile int power_open_circuit_status = 0;
volatile int camera_precaution_status = 0;
volatile int fault_can_online_flag[CAN_SENSOR_NUM] = {0};
volatile int fault_open_circuit_LED_status = 0;
volatile int fault_underpressure_LED_status = 0;
volatile int fault_can_open_circuit_LED_status = 0;
volatile int fault_can_short_circuit_LED_status = 0;
volatile int fault_extinguish_ack_LED_status = 0;
volatile int fault_transmitter_status = 0;
volatile int alarm_LED_status = 0;
volatile int alarm_LED_status_for_precaution = 0;
volatile int extinguish_LED_status = 0;
volatile int maintenance_LED_status = 0;
volatile int can_login_flag = 0;
volatile int record_err = 0;
volatile float current_val = 0;
volatile int flag_for_extinguish_carbinet_NO_1 = 0;
volatile int flag_for_extinguish_carbinet_NO_2 = 0;
volatile char sw_choose = 0;

int set_ip_flag = 0;
unsigned char eth_converter_cabinet_No = 0;
unsigned char eth_converter_cabinet_No_send_to_HKPro = 0;

volatile int camera_offline_log[CAMERA_NUM] = {0};

SD sd[INSTALLED_SD_NO];

int sd_reset_command;
int sd_fault;
int sd_alarm;
int sd_bus_error;
int sd_bus_error_position[2];
int sd_can_normal;

int debug_can0_ok = 0;
int debug_can1_ok = 0;

ETH_6A_TIME struct_6A_time;

DATA_PACKAGE_FH data_buffer_fh;
DATA_PACKAGE_VERSION data_buffer_version;
DATA_PACKAGE_IO data_buffer_io_ctrl;

DATA_PACKAGE_PUBLIC data_buffer_public[4]; // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN
DATA_PACKAGE_TIME data_buffer_time[4];     // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN
DATA_PACKAGE_IO data_buffer_io[4];         // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN
