/*
 * GlobalVariable.h
 *
 *  Created on: 2023.09.08
 *      Author: Wang Chyng-chuan
 */

#ifndef GLOBALVARIABLE_H_
#define GLOBALVARIABLE_H_

#pragma pack(1)
typedef union
{
	short val_dec;
	char val_chr[2];
} DATA_DOUBLE_SEP;

typedef union
{
	unsigned int val_dec;
	unsigned char val_chr[4];
} DATA_FOUR_SEP;

typedef union uint16_little_end
{
	struct
	{
		unsigned char uint16_l;
		unsigned char uint16_h;
	};
	unsigned short all;
} UINT16_LITTLE_END;

typedef enum after_extinguish_stat
{
	NOT_WORK = 0,
	ack_OK,
	ack_ERROR
} AFTER_EXTINGUISH_STAT;

typedef enum fire_extinguish_flag_stat
{
	READY = 0,
	OCCUIPIED,
	FINISHED
} FIRE_EXTINGUISH_FLAG_STAT;

typedef enum camera_begin_record_video_status
{
	RECORD_OFF = 0,
	RECORD_ON = 1
} CAMERA_BEGIN_RECORD_VIDEO_STATUS;

typedef enum fire_extinguish_pressed_stat
{
	SWITCH_OFF = 0,
	SWITCH_ON,
	SWITCH_ON_2
} FIRE_EXTINGUISH_PRESSWS_STAT;

typedef enum camera_status
{
	CAMERA_OFFLINE = 0,
	CAMERA_ON
} CAMERA_STATUS;

typedef enum sensor_type
{
	TYPE_UNINSTALL = 0,
	TYPE_TEMPERATURE_and_SMOKE,
	TYPE_TEMPERATURE,
	TYPE_FIRE,
	TYPE_WIRE_or_PIPE,
	TYPE_SMOKE
} SENSOR_TYPE;

typedef enum sensor_status
{
	SENSOR_NORMAL = 1,
	SENSOR_FAULT,
	SENSOR_POLLUTION,
	SENSOR_ALARM,
	SENSOR_QUARANTINE,
	SENSOR_OFFLINE
} SENSOR_STATUS;

typedef struct gpio_info
{
	unsigned int num;
	unsigned int val;
} GPIO_INFO;

typedef enum camera_stat
{
	UNINSTALL = 0,
	NORMAL,
	FAULT,
	POLLUTION,
	WARNING,
	PERCAUTION,
	OFFLINE
} CAMERA_STAT;

typedef struct package_sequence
{
	unsigned char sequence;
} PACKAGE_SEQUENCE;

typedef struct package_check
{
	unsigned char check_h;
	unsigned char check_l;
} PACKAGE_CHECK;

typedef struct eth_6A_time
{
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
} ETH_6A_TIME;

typedef union package_header
{
	struct
	{
		unsigned char header_l;
		unsigned char header_h;
	};
	unsigned short all;
} PACKAGE_HEADER;

typedef union package_length
{
	struct
	{
		unsigned char length_l;
		unsigned char length_h;
	};
	unsigned short all;
} PACKAGE_LENGTH;

typedef union package_flag
{
	struct
	{
		unsigned char flag_l;
		unsigned char flag_h;
	};
	unsigned short all;
} PACKAGE_FLAG;

typedef union package_life_count
{
	struct
	{
		unsigned char lief_count_l;
		unsigned char lief_count_h;
	};
	unsigned short all;
} PACKAGE_LIFE_COUNT;

typedef struct package_type
{
	unsigned char type;
} PACKAGE_TYPE;

typedef union train_speed
{
	struct
	{
		unsigned char train_speed_l;
		unsigned char train_speed_h;
	};
	unsigned short all;
} TRAIN_SPEED;

typedef union sensor_status_and_type
{
	struct
	{
		unsigned char sensor_type : 4;
		unsigned char sensor_status : 4;
	};
	unsigned char all;
} SENSOR_STATUS_and_TYPE;

typedef union detect_info_message
{
	struct
	{
		unsigned char open_circuit : 1;
		unsigned char short_circuit : 1;
		unsigned char record_err : 1;
		unsigned char reserve : 3;
		unsigned char early_warning : 1;
		unsigned char fire_alarm : 1;
	};
	unsigned char all;
} DETECT_INFO_MESSAGE;

typedef union extinguish_info_message
{
	struct
	{
		unsigned char extinguish_action : 1;		  // ¿ØÖÆÆ÷·¢ËÍÆô¶¯µç´Å·§Ö¸Áî
		unsigned char extinguish_ack : 1;			  // Ãð»ð¼ÁÒÑÕý³£Åç·Å
		unsigned char pressure_low : 1;				  // Ãð»ð¼ÁÑ¹Á¦Òì³£
		unsigned char state_liquid_spray_fault : 1;	  // Ãð»ð¼ÁÅç·Å×´Ì¬Òì³£
		unsigned char pressure_transmitter_fault : 1; // Ñ¹Á¦±äËÍÆ÷¹ÊÕÏ
		unsigned char : 3;
	};
	unsigned char all;
} EXTINGUISH_INFO_MESSAGE;

typedef union can_sensor_fault
{
	struct
	{
		unsigned char smoke_fault : 1;
		unsigned char temprature_fault : 1;
		unsigned char smoke_alarm : 1;
		unsigned char temprature_alarm : 1;
		unsigned char : 4;
	};
	unsigned char all;
} CAN_SENSOR_FAULT;

typedef union software_version_num
{
	struct
	{
		unsigned char software_version_l;
		unsigned char software_version_h;
	};
	unsigned short all;
} SOFTWARE_VERSION_NUM;

typedef struct software_version
{
	SOFTWARE_VERSION_NUM version_num;
	char version_ascii;
} SOFTWARE_VERSION;

typedef struct cansensor_date_message
{
	CAN_SENSOR_FAULT fault;
	unsigned char detect_supplier;
	SOFTWARE_VERSION can_software_version;
	unsigned char data[4];
	unsigned char temp_alarm_value;
	unsigned char reserved[6];
} CAN_SENSOR_DATE_MESSAGE;


typedef struct can_online_and_count
{
	unsigned char can_online_count;
	unsigned char can_once_online_flag;
} CAN_ONLINE_AND_COUNT;

typedef union image_detect_temperature
{
	struct
	{
		char max_temperature_l;
		char max_temperature_h;
	};
	short all;
} IMAGE_DETECT_TEMPERATURE;

typedef struct thermal_imaging_date_message
{
	unsigned char detect_supplier;
	SOFTWARE_VERSION software_version;
	IMAGE_DETECT_TEMPERATURE max_temperature;
	unsigned char reserved[2];
} THERMAL_IMAGING_DATE_MESSAGE;

typedef struct extinguish_info_message_to_6A
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH len;
	PACKAGE_FLAG flag;
	PACKAGE_LIFE_COUNT life_count;
	TRAIN_SPEED train_speed;
	unsigned char converter_cabinet_No;
	SENSOR_STATUS_and_TYPE sensor_status_and_type[CAN_SENSOR_NUM];
	unsigned char camera_status[CAMERA_NUM];
	DETECT_INFO_MESSAGE detect_info_message;
	EXTINGUISH_INFO_MESSAGE extinguish_info_message;
	unsigned char pressure_transmitter_value;
	unsigned char info_reserved[8];
	unsigned char extinguish_detect_supplier;
	SOFTWARE_VERSION software_version;
	CAN_SENSOR_DATE_MESSAGE can_sensor_buff[CAN_SENSOR_NUM];
	THERMAL_IMAGING_DATE_MESSAGE thermal_imaging_buff[CAMERA_NUM];
	unsigned char reserved[20];
} EXTINGUISH_INFO_MESSAGE_to_6A;

typedef union bus_error
{
	// Low to High
	struct
	{
		unsigned char bus_error : 1;
		unsigned char : 7;
	};
	unsigned char all;
} BUS_ERROR;

typedef union sd_data
{
	// Low to High
	struct
	{
		unsigned char sd_type : 4;
		unsigned char sd_status : 4;
	};
	unsigned char all;
} SD_DATA;

typedef struct data_package_fh
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH length;
	PACKAGE_TYPE type;
	PACKAGE_SEQUENCE sequence;
	BUS_ERROR bus;
	SD_DATA sd[MAX_SD_NO];
	unsigned char reserved[23 - MAX_SD_NO];
	PACKAGE_CHECK check;
} DATA_PACKAGE_FH;

typedef union ccu_valid
{
	// Low to High
	struct
	{
		unsigned char valid : 1;
		unsigned char : 7;
	};
	unsigned char all;
} CCU_VALID;

typedef struct data_package_public
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH length;
	PACKAGE_TYPE type;
	PACKAGE_SEQUENCE sequence;
	CCU_VALID ccu_vaild;
	unsigned char time[6];
	unsigned char speed[2];
	unsigned char kilometer[4];
	unsigned char trainnumber[15];
	unsigned char traincode[5];
	unsigned char cargonumber;
	unsigned char longitude[4];
	unsigned char latitude[4];
	unsigned char host;
	unsigned char temperature;
	unsigned char alex1wheel[2];
	unsigned char alex2wheel[2];
	unsigned char pressure1[2];
	unsigned char pressure2[2];
	unsigned char reserved[4];
	PACKAGE_CHECK check;
} DATA_PACKAGE_PUBLIC;

typedef struct data_package_version
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH length;
	PACKAGE_TYPE type;
	PACKAGE_SEQUENCE sequence;
	unsigned char devicecode;
	unsigned char software_version_number_h;
	unsigned char software_version_number_l;
	unsigned char software_version_letter;
	unsigned char hardware_version_number_h;
	unsigned char hardware_version_number_l;
	unsigned char hardwareversion_letter;
	unsigned char factorycode_h;
	unsigned char factorycode_l;
	unsigned char reserved[15];
	PACKAGE_CHECK check;
} DATA_PACKAGE_VERSION;

typedef struct data_package_time
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH length;
	PACKAGE_TYPE type;
	PACKAGE_SEQUENCE sequence;
	unsigned char null;
	unsigned char time[7];
	unsigned char reserved[13];
	PACKAGE_CHECK check;
} DATA_PACKAGE_TIME;

typedef union command_info
{
	// Low to High
	struct
	{
		unsigned char io_ctrl_device : 4;
		unsigned char io_ctrl_comm_type : 4;
	};
	unsigned char all;
} COMMAND_INFO;

typedef struct data_package_io
{
	PACKAGE_HEADER header;
	PACKAGE_LENGTH length;
	PACKAGE_TYPE type;
	PACKAGE_SEQUENCE sequence;
	COMMAND_INFO command;
	unsigned char ctrltype;
	unsigned char ctrlid;
	unsigned char ctrlstep;
	unsigned char reserved[20];
	PACKAGE_CHECK check;
	;
} DATA_PACKAGE_IO;

typedef union can_id
{
	// Low to High
	struct
	{
		unsigned int sequence : 6;
		unsigned int send_id : 6;
		unsigned int type : 2;
		unsigned int recv_id : 6;
		unsigned int ack : 2;
		unsigned int identification : 4;
		unsigned int level : 3;
		unsigned int : 3;
	};
	unsigned int all;
} CAN_ID;

typedef union can_sd_id
{
	// Low to High
	struct
	{
		unsigned int address : 8;
		unsigned int command : 3;
		unsigned int : 21;
	};
	unsigned int all;
} CAN_SD_ID;

typedef union sd_config_mode
{
	// Low to High
	struct
	{
		unsigned char cable_enable : 1;
		unsigned char temperature_enable : 1;
		unsigned char smoke_enable : 1;
		unsigned char : 5;
	};
	unsigned char all;
} SD_CONFIG_MODE;

typedef struct sd_config
{
	SD_CONFIG_MODE mode;
	unsigned char temperature_threshold;   // 69-85 C
	unsigned char temperature_report_time; // 0-30 s
	unsigned char smoke_threshold;		   // 80-120 % 108
	unsigned char smoke_report_time;	   // 0-30 s
} SD_CONFIG;

typedef struct sd_info
{
	unsigned char supplier;
	unsigned char sd_type;
	unsigned char version;
} SD_INFO;

typedef union sd_alarm_status
{
	// Low to High
	struct
	{
		unsigned char smoke : 1;	   // SD_DATA_REAL_TIME.smoke > (108%)
		unsigned char temperature : 1; // SD_DATA_REAL_TIME.temperature > SD_CONFIG.temperature_threshold
		unsigned char cable : 1;	   // 234(Ohms) >= SD_DATA_REAL_TIME.cable(h-l) >= 126(Ohms)
		unsigned char flame : 1;
		unsigned char : 4;
	};
	unsigned char all;
} SD_ALARM_STATUS;

typedef union sd_fault_status
{
	// Low to High
	struct
	{
		unsigned char cable_grounding_short : 1;  // Temperature sensing cable is shorted to ground	125(Ohms) >= SD_DATA_REAL_TIME.cable(h-l) >= 0(Ohms)
		unsigned char cable_resistance_error : 1; // Temperature sensing cable resistance error		399(Ohms) >= SD_DATA_REAL_TIME.cable(h-l) >= 235(Ohms)
		unsigned char cable_rv_short : 1;		  // Temperature sensing cable RV short circuit		584(Ohms) >= SD_DATA_REAL_TIME.cable(h-l) >= 400(Ohms)
		unsigned char cable_open : 1;			  // Temperature sensing cable open circuit			SD_DATA_REAL_TIME.cable(h-l) >= 715(Ohms)
		unsigned char dirty : 1;				  // Detector is contaminated                          SD_DATA_REAL_TIME.normal_background > 100(%)
		unsigned char component_smoke : 1;		  // Smoke sensor error
		unsigned char component_temperature : 1;  // Temperature sensor error
		unsigned char unknow_error : 1;			  // Other unknown types of errors
	};
	unsigned char all;
} SD_FAULT_STATUS;

typedef struct sd_data_real_time
{
	unsigned char smoke;
	unsigned char normal_background;
	unsigned char temperature; // Complement code.
	unsigned char cable_l;	   //((cable_h & 0x00FF) < 8) | (cable_l & 0x00FF) 0-750 Ohm(s)
	unsigned char cable_h;	   //((cable_h & 0x00FF) < 8) | (cable_l & 0x00FF) 0-750 Ohm(s)
	SD_ALARM_STATUS alarm;
	SD_FAULT_STATUS fault;
} SD_DATA_REAL_TIME;

typedef struct sd_data_alarm_time
{
	unsigned char smoke;
	unsigned char normal_background;
	unsigned char temperature; // Complement code.
	unsigned char cable_l;	   //((cable_h & 0x00FF) < 8) | (cable_l & 0x00FF) 0-750 Ohm(s)
	unsigned char cable_h;	   //((cable_h & 0x00FF) < 8) | (cable_l & 0x00FF) 0-750 Ohm(s)
} SD_DATA_ALARM_TIME;

typedef struct sd_lifesignal
{
	int lifesignal;
	int lifesignal_old;
	int online_counter;
	int offline_counter;
} SD_LIFESIGNAL;

typedef struct sd_data_ctrl
{
	SD_DATA info;
	SD_LIFESIGNAL lifesignal;
	SD_LIFESIGNAL lifesignal_can0;
	SD_LIFESIGNAL lifesignal_can1;
	int isolated;
	int online;
	int online_can0;
	int online_can1;
} SD_DATA_CTRL;

typedef struct sd
{
	SD_INFO info;
	SD_CONFIG config;
	SD_DATA_REAL_TIME realtime;
	SD_DATA_ALARM_TIME alarmtime;
	SD_DATA_CTRL ctrl;
} SD;

typedef struct extinguish_info_message_to_Workbench
{
	PACKAGE_HEADER header;
	unsigned char len;
	unsigned char flag;
	unsigned char life_count;
	unsigned char workbench_command_byte_NO_1;
	unsigned char workbench_command_byte_NO_2;
	unsigned char self_status_byte_NO_1;
	unsigned char self_status_byte_NO_2;
	unsigned char pressure_simulate_value_NO_1;
	unsigned char pressure_simulate_value_NO_2;
	unsigned char pressure_simulate_value_NO_3;
	unsigned char sw_choose_val;
	unsigned char reserved[5];
} EXTINGUISH_INFO_MESSAGE_to_Workbench;

#pragma pack()

extern char filepath_cculog[1024];
extern char filename_cculog[1024];

extern char filepath_datalog[1024];
extern char filename_datalog[1024];

extern char filepath_can_login_datalog[1024];
extern char filename_can_login_datalog[1024];

extern int gpiofd;
extern int comfd_ttymxc0;
extern int comfd_ttymxc1;
extern int comfd_ttymxc2;
extern int comfd_ttymxc3;
extern int canfd[3];
extern int watchdogfd;
extern int ethsendfd2cc1;
extern int ethsendfd2cc2;
extern int ethrecvfd;
extern volatile int can_temperature_rise_pressed;
extern volatile int can_alarm_pressed;
extern volatile int camera_alarm_pressed;
extern volatile int camera_begin_record_video_flag;
extern volatile int extinguish_ack;
extern volatile int can_sensor_precaution_status;
extern volatile int can_sensor_smoke_status;
extern volatile int power_short_circuit_status;
extern volatile int power_open_circuit_status;
extern volatile int camera_precaution_status;
extern CAN_ONLINE_AND_COUNT can_once_online_status[CAN_SENSOR_NUM];
extern volatile int fault_can_online_flag[CAN_SENSOR_NUM];
extern volatile int fault_open_circuit_LED_status;
extern volatile int fault_underpressure_LED_status;
extern volatile int fault_can_open_circuit_LED_status;
extern volatile int fault_can_short_circuit_LED_status;
extern volatile int fault_extinguish_ack_LED_status;
extern volatile int alarm_LED_status;
extern volatile int alarm_LED_status_for_precaution;
extern volatile int extinguish_LED_status;
extern volatile int maintenance_LED_status;
extern volatile int can_login_flag;
extern unsigned char loco_and_num_buff[12];
extern EXTINGUISH_INFO_MESSAGE_to_6A *detect_msg_to_6A_info;
extern volatile int record_err;
extern volatile float current_val;
extern volatile int fault_transmitter_status;
extern volatile int flag_for_extinguish_carbinet_NO_1;
extern volatile int flag_for_extinguish_carbinet_NO_2;
extern SD sd[INSTALLED_SD_NO];
extern EXTINGUISH_INFO_MESSAGE_to_Workbench *extinguish_info_message_to_Workbench;

extern int set_ip_flag;
extern unsigned char eth_converter_cabinet_No;
extern unsigned char eth_converter_cabinet_No_send_to_HKPro;

extern volatile int camera_offline_log[CAMERA_NUM];

extern int sd_reset_command;
extern int sd_fault;
extern int sd_alarm;
extern int sd_bus_error;
extern int sd_bus_error_position[2];
extern int sd_can_normal;

extern int debug_can0_ok;
extern int debug_can1_ok;

extern ETH_6A_TIME struct_6A_time;

extern DATA_PACKAGE_FH data_buffer_fh;
extern DATA_PACKAGE_VERSION data_buffer_version;
extern DATA_PACKAGE_IO data_buffer_io_ctrl;

extern DATA_PACKAGE_PUBLIC data_buffer_public[4]; // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN
extern DATA_PACKAGE_TIME data_buffer_time[4];	  // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN
extern DATA_PACKAGE_IO data_buffer_io[4];		  // 0:CCU1-ETH 1:CCU2-ETH 2:CCU1-CAN 3:CCU2-CAN

#endif /* GLOBALVARIABLE_H_ */