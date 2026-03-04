/*
 * can.h
 *
 *  Created on: 2023.11.1
 *      Author: Wang Chyng-chuan
 */

#ifndef CAN_H_
#define CAN_H_

#define CAN_SEND_TIME 2
#define CAN_ALARM_TIME 2
#define CAN_CO_VALUE_LIMIT 60
#define CAN_SMOKE_VALUE_LIMIT 110
#define CAN_TEMPERATURE_PRECAUSION_LIMIT 75
#define CAN_TEMPERATURE_EXTINGUISH_LIMIT 90
#define CAN_REONLINE_SECONDS 2
#define CAN_ONLINE_to_OFFLINE_SECONDS 15
#define CAN_TIME_WAIT 5
#define CAN_TEMPERATURE_UPPER_TIME 60
#define CAN_TEMPERATURE_DIFF_VAL 3
#define CAN_TEMPERATURE_ADJUST_VAL 40
#define CAN_TEMPERATURE_THRESHOLD_VAL 125
#define CAN_OPEN_CIRCUIT_TIME 150 // 150 / 10 = 15s
#define CAN_OFFLINE_MAX_COUNT 3

typedef enum can_sensor_smoke_status
{
	INIT_STATUS = 0,
    SMOKE_but_NOT_TEMPERATURE_INCREASE_FAST_FLAG = 1,
	SMOKE_and_TEMPERATURE_INCREASE_FAST_FLAG = 2
} CAN_SENSOR_SMOKE_STATUS;

typedef enum can_online_status
{
	CAN_STATUS_OFFLINE = 0,
	CAN_STATUS_ONLINE,
	CAN_STATUS_REONLINE
} CAN_ONLINE_STATUS;

typedef enum can_id_identification
{
	CAN_ID_LOGIN_IDENTIFICATION = 1,
	CAN_ID_CONFIG_IDENTIFICATION,
	CAN_ID_SENSOR_DETECT_IDENTIFICATION
} CAN_ID_IDENTIFICATION;

typedef enum can_id_command
{
	CAN_ID_LOGIN_COMMAND = 0x11,
	CAN_ID_CONFIG_COMMAND = 0x21,
} CAN_ID_COMMAND;

typedef enum can_id_command_ack
{
	CAN_ID_LOGIN_COMMAND_ACK = 0x12,
	CAN_ID_CONFIG_COMMAND_ACK = 0x22,
	CAN_ID_SENSOR_STATUS_COMMAND_ACK = 0x31 // autonomous response, 1s per time
} CAN_ID_COMMAND_ACK;

typedef enum can_id_level
{
	CAN_ID_LEVEL_HIGHEST = 7
} CAN_ID_LEVEL;

typedef enum can_sensor_device_no
{
	CAN_DEVICE_to_ALL = 0,
	CAN_SENSOR_DEVICE_NO_1,
	CAN_SENSOR_DEVICE_NO_2,
	CAN_SENSOR_DEVICE_NO_3,
	CAN_SENSOR_DEVICE_NO_4,
	CAN_SENSOR_DEVICE_NO_5,
	CAN_SENSOR_DEVICE_NO_6,
	CAN_SENSOR_DEVICE_NO_7,
	CAN_SENSOR_DEVICE_NO_8,
	CAN_SENSOR_DEVICE_NO_9,
	CAN_SENSOR_DEVICE_NO_10,
	CAN_SENSOR_DEVICE_NO_11,
	CAN_SENSOR_DEVICE_NO_12,
	CAN_SENSOR_DEVICE_NO_13,
	CAN_SENSOR_DEVICE_NO_14,
	CAN_SENSOR_DEVICE_NO_15,
	CAN_SENSOR_DEVICE_NO_16,
	CAN_SENSOR_DEVICE_NO_17,
	CAN_SENSOR_DEVICE_NO_18,
	CAN_SENSOR_DEVICE_NO_19,
	CAN_SENSOR_DEVICE_NO_20,
	CAN_SENSOR_DEVICE_NO_21,
	CAN_SENSOR_DEVICE_NO_22,
	CAN_SENSOR_DEVICE_NO_23,
	CAN_SENSOR_DEVICE_NO_24,
	CAN_SENSOR_DEVICE_NO_25,
	CAN_SENSOR_DEVICE_NO_26,
	CAN_SENSOR_DEVICE_NO_27,
	CAN_SENSOR_DEVICE_NO_28,
	CAN_SENSOR_DEVICE_NO_29,
	CAN_SENSOR_DEVICE_NO_30,
	CAN_SENSOR_DEVICE_NO_31,
	CAN_SENSOR_DEVICE_NO_32,
	CAN_ADDRESS_NO_CAN0,
	CAN_ADDRESS_NO_CAN1
} CAN_SENSOR_DEVICE_NO;

typedef struct can_sensor_threshold_value
{
	unsigned char temperature_threshold; // 0~255 C
} CAN_SENSOR_THRESHOLD_VALUE;

typedef struct can_sensor_info
{
	unsigned char sensor_supplier;
	unsigned char sensor_type;
	unsigned char sensor_software_version;
	unsigned char sensor_hardware_version;
} CAN_SENSOR_INFO;

typedef struct can_sensor_config_msg
{
	CAN_SENSOR_INFO sensor_info;
	CAN_SENSOR_THRESHOLD_VALUE sensor_threshold;
} CAN_SENSOR_CONFIG_ACK_MSG;

typedef union sensor_alarm_status
{
	struct
	{
		unsigned char smoke_alarm : 1;
		unsigned char temp_alarm : 1;
		unsigned char temp_wire_alarm : 1;
		unsigned char flame_alarm : 1;
		unsigned char CO_alarm : 1;
		unsigned char : 3;
	};
	unsigned char all;
} SENSOR_ALARM_STATUS;

typedef union sensor_fault_status
{
	struct
	{
		unsigned char flame : 1;
		unsigned char temp_wire : 1;
		unsigned char smoke_dirty : 1;
		unsigned char smoke_part_fault : 1;
		unsigned char temp_part_fault : 1;
		unsigned char smoke_flu_part_fault : 1;
		unsigned char : 2;
	};
	unsigned char all;
} SENSOR_FAULT_STATUS;

typedef struct can_sensor_data
{
	unsigned char current_blue_smoke_or_CO_val;
	unsigned char current_red_smoke_val;
	int temperature_val;
	unsigned char pollute_val;
	SENSOR_ALARM_STATUS alarm_status;
	SENSOR_FAULT_STATUS fault_status;
} CAN_SENSOR_DATA;

typedef struct can_lifesignal
{
	int lifesignal;
	int lifesignal_old;
	int online_counter;
	int offline_counter;
} CAN_LIFESIGNAL;

typedef struct can_sensor_status
{
	SD_LIFESIGNAL lifesignal;
	int online;
} CAN_SENSOR_STATUS;

typedef struct can_sensor_login_ack_msg
{
	unsigned char supplier;
	unsigned char sensor_type;
	unsigned char software_version;
	unsigned char hardware_version;
	unsigned char temperature_threshold;
} CAN_SENSOR_LOGIN_ACK_MSG;

typedef struct can_sensor_state_meg
{
	CAN_SENSOR_DATA data;
	CAN_SENSOR_STATUS status;
} CAN_SENSOR_STATE_MSG;

typedef struct can_data
{
	CAN_SENSOR_CONFIG_ACK_MSG config;
	CAN_SENSOR_LOGIN_ACK_MSG login;
	CAN_SENSOR_STATE_MSG state;
} CAN_DATA;

int init_can(char *name, char *bitrate);

void thread_func_can_buff_send();

void thread_func_can_smoke_sensor_detect_recv();

#endif
