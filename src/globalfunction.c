/*
 * globalfunction.c
 *
 *  Created on: 2023.09.08
 *      Author: Wang Chyng-chuan
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <asm/ioctl.h>
#include <sys/ioctl.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/can/raw.h>
#include <linux/can.h>
#include <net/if.h>
#include <linux/watchdog.h>

#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"

int init_watchdog(char *name, int timeout)
{
#if ENABLE_WATCHDOG
	watchdogfd = open(name, O_WRONLY);
	int options = WDIOS_ENABLECARD;

	if (-1 == watchdogfd)
	{
		_CCU_ERROR_(ERROR_WATCHDOG_INIT, name);
	}
	else
	{
		ioctl(watchdogfd, WDIOC_SETTIMEOUT, &timeout);
		ioctl(watchdogfd, WDIOC_SETOPTIONS, &options);

		_CCU_LOG_("Init watchdog success.Feed time is %d.", timeout);
	}
#else
	_CCU_LOG_("Init watchdog exit due to set disabled by define ENABLE_WATCHDOG 0.");
#endif
	return 0;
}

int init_config(char *name)
{
	//	_CCU_LOG_("Init config success");
	return 0;
}

int init_logfile()
{
	int i = 0;
	int ret = 0;
	char buffer[1024] = {0};
	time_t now;
	time(&now);
	struct tm *local;
	local = gmtime(&now);

	int file_count = 0;
	char filename[4096][1024] = {0};

	DIR *dir_cculog = NULL;
	struct dirent *entry;
	dir_cculog = opendir(FILE_PATH_LOG);
	if (dir_cculog == NULL)// no necessary to check num of file
	{
		ret = mkdir(FILE_PATH_LOG, 0755);
		if (ret != 0)
		{
			record_err = 1;
			printf("mkdir %s error!\n", FILE_PATH_LOG);
			return -1;
		}
	}
	else
	{
		while ((entry = readdir(dir_cculog)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			{
				continue;
			}
			file_count++;
		}
		closedir(dir_cculog);

		if (file_count >= FILE_LOG_MAX)
		{
			dir_cculog = opendir(FILE_PATH_LOG);
			while ((entry = readdir(dir_cculog)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				{
					continue;
				}
				sprintf(filename[i++], "%s/%s", FILE_PATH_LOG, entry->d_name);
			}
			closedir(dir_cculog);

			for (i = 0; i < file_count; i++) // bubble sort
			{
				for (int j = i + 1; j < file_count; j++)
				{
					if (strcmp(filename[i], filename[j]) > 0)
					{
						strcpy(buffer, filename[i]);
						strcpy(filename[i], filename[j]);
						strcpy(filename[j], buffer);
					}
				}
			}

			for (i = 0; i < file_count - FILE_LOG_MAX; i++)
			{
				remove(filename[i]);
			}
		}
	}

	sprintf(filepath_cculog, FILE_PATH_LOG);
	sprintf(filename_cculog, "%s/%04d%02d%02d-%02d%02d%02d.log", filepath_cculog, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	creat(filename_cculog, 0755);

	_CCU_LOG_("------FXD-A53_for_Testbench(FXD) application start------");
	_CCU_LOG_("------ 6 nework interface -----");
	_CCU_LOG_("Software Version %d.%d.%d", INNER_VERSION_NUMBER_X, INNER_VERSION_NUMBER_Y, INNER_VERSION_NUMBER_Z);
	_CCU_LOG_("Init log file:%s create success.", filename_cculog);

#if DATA_RECORD_MODE
	_CCU_LOG_("DATA_RECORD_MODE mode is set, so create datefile and record data!");
#else
	_CCU_LOG_("DATA_RECORD_MODE mode is NOT set, so do not create datefile and record data!");
#endif

	return 0;
}

int init_data_logfile()
{
	int ret = 0;
	int i = 0;
	char buffer[1024] = {0};
	time_t now;
	time(&now);
	struct tm *local;
	local = gmtime(&now);

	int file_count = 0;
	char filename[4096][1024] = {0};

	DIR *dir_cculog = NULL;
	struct dirent *entry;
	dir_cculog = opendir(FILE_PATH_DATA_LOG);
	if (dir_cculog == NULL)// no necessary to check num of file
	{
		ret = mkdir(FILE_PATH_DATA_LOG, 0755);
		if (ret != 0)
		{
			record_err = 1;
			printf("mkdir %s error!\n", FILE_PATH_DATA_LOG);
			return -1;
		}
	}
	else
	{
		while ((entry = readdir(dir_cculog)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			{
				continue;
			}
			file_count++;
		}
		closedir(dir_cculog);

		if (file_count >= FILE_DATA_LOG_MAX)
		{
			dir_cculog = opendir(FILE_PATH_DATA_LOG);
			while ((entry = readdir(dir_cculog)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				{
					continue;
				}
				sprintf(filename[i++], "%s/%s", FILE_PATH_DATA_LOG, entry->d_name);
			}
			closedir(dir_cculog);

			for (i = 0; i < file_count; i++) // bubble sort
			{
				for (int j = i + 1; j < file_count; j++)
				{
					if (strcmp(filename[i], filename[j]) > 0)
					{
						strcpy(buffer, filename[i]);
						strcpy(filename[i], filename[j]);
						strcpy(filename[j], buffer);
					}
				}
			}

			for (i = 0; i < file_count - FILE_DATA_LOG_MAX; i++)
			{
				remove(filename[i]);
			}
		}
	}

#if DATA_RECORD_MODE
	sprintf(filepath_datalog, FILE_PATH_DATA_LOG);
	sprintf(filename_datalog, "%s/%04d%02d%02d-%02d%02d%02d.csv", filepath_datalog, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	creat(filename_datalog, 0755);

	FILE *file = fopen(filename_datalog, "a+");
	if (NULL != file)
	{
		sprintf(buffer, "time, sensor_type, SENSOR_NUM, CAMRA_TEMPERATURE_VALUE, CAN_TEMPERATURE_VALUE, SMOKE_VALUE_BLUE, SMOKE_VALUE_RED\n");
		fwrite(buffer, 1, strlen(buffer), file);
		fclose(file);
		_CCU_LOG_("Init data log file:%s create success.", filename_datalog);
	}
	else
	{
		_CCU_ERROR_(ERROR_DATA_LOG_INIT, filename_datalog);
	}

#else
	_CCU_LOG_("Init data log file:%s NOT success, cause DATA_RECORD_MODE is not set.", filename_datalog);
#endif

	return 0;
}

int init_can_login_data_logfile()
{
	int ret = 0;
	int i = 0;
	char buffer[1024] = {0};
	time_t now;
	time(&now);
	struct tm *local;
	local = gmtime(&now);

	int file_count = 0;
	char filename[4096][1024] = {0};

	DIR *dir_cculog = NULL;
	struct dirent *entry;
	dir_cculog = opendir(FILE_PATH_CAN_LOGIN_DATA_LOG);
	if (dir_cculog == NULL) // no necessary to check num of file
	{
		mkdir(FILE_PATH_CAN_LOGIN_DATA_LOG, 0755);
		if (ret != 0)
		{
			record_err = 1;
			printf("mkdir %s error!\n", FILE_PATH_CAN_LOGIN_DATA_LOG);
			return -1;
		}
	}
	else
	{
		while ((entry = readdir(dir_cculog)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			{
				continue;
			}
			file_count++;
		}
		closedir(dir_cculog);

		if (file_count >= FILE_CAN_LOGIN_DATA_LOG_MAX)
		{
			dir_cculog = opendir(FILE_PATH_CAN_LOGIN_DATA_LOG);
			while ((entry = readdir(dir_cculog)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				{
					continue;
				}
				sprintf(filename[i++], "%s/%s", FILE_PATH_CAN_LOGIN_DATA_LOG, entry->d_name);
			}
			closedir(dir_cculog);

			for (i = 0; i < file_count; i++) // bubble sort
			{
				for (int j = i + 1; j < file_count; j++)
				{
					if (strcmp(filename[i], filename[j]) > 0)
					{
						strcpy(buffer, filename[i]);
						strcpy(filename[i], filename[j]);
						strcpy(filename[j], buffer);
					}
				}
			}

			for (i = 0; i < file_count - FILE_CAN_LOGIN_DATA_LOG_MAX; i++)
			{
				remove(filename[i]);
			}
		}
	}
#if DATA_RECORD_MODE
	sprintf(filepath_can_login_datalog, FILE_PATH_CAN_LOGIN_DATA_LOG);
	sprintf(filename_can_login_datalog, "%s/%04d%02d%02d-%02d%02d%02d.csv", filepath_can_login_datalog, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	creat(filename_can_login_datalog, 0755);

	FILE *file = fopen(filename_can_login_datalog, "a+");
	if (NULL != file)
	{
		sprintf(buffer, "time, sensor_type, SENSOR_NUM, supplier, software_version, hardware_version, temperature_threshold\n");
		fwrite(buffer, 1, strlen(buffer), file);
		fclose(file);
		_CCU_LOG_("Init can login log file:%s create success.", filename_can_login_datalog);
	}
	else
	{
		_CCU_ERROR_(ERROR_CAN_LOGIN_LOG_INIT, filename_can_login_datalog);
	}

#else
	_CCU_LOG_("Init can login log file:%s NOT create, cause DATA_RECORD_MODE is not set.", filename_can_login_datalog);
#endif

	return 0;
}

void _CCU_CAN_LOGIN_DATA_LOG_(const char *ms, ...)
{
	if (0 == record_err)
	{
		size_t ret = 0;
		char wzLog[1024] = {0};
		char buffer[1024] = {0};
		va_list args;
		va_start(args, ms);
		vsprintf(wzLog, ms, args);
		va_end(args);

		time_t now;
		time(&now);
		struct tm *local;
		local = gmtime(&now);

		sprintf(buffer, "%04d-%02d-%02d %02d_%02d_%02d, %s\n",
				local->tm_year + 1900,
				local->tm_mon + 1,
				local->tm_mday,
				local->tm_hour,
				local->tm_min,
				local->tm_sec,
				wzLog);
#if DATA_RECORD_MODE
		FILE *file = fopen(filename_can_login_datalog, "a+");
		if (NULL != file)
		{
			ret = fwrite(buffer, 1, strlen(buffer), file);
			fclose(file);
		}
#endif
	}
}

void _CCU_DATA_LOG_(const char *ms, ...)
{
	if (0 == record_err)
	{
		int ret = 0;
		char wzLog[1024] = {0};
		char buffer[1024] = {0};
		va_list args;
		va_start(args, ms);
		vsprintf(wzLog, ms, args);
		va_end(args);

		time_t now;
		time(&now);
		struct tm *local;
		local = gmtime(&now);

		sprintf(buffer, "%04d-%02d-%02d %02d_%02d_%02d, %s\n",
				local->tm_year + 1900,
				local->tm_mon + 1,
				local->tm_mday,
				local->tm_hour,
				local->tm_min,
				local->tm_sec,
				wzLog);
#if DATA_RECORD_MODE
		FILE *file = fopen(filename_datalog, "a+");
		if (NULL != file)
		{
			ret = fwrite(buffer, 1, strlen(buffer), file);
			fclose(file);
		}
#endif
	}
}

void _CCU_LOG_(const char *ms, ...)
{
	int ret = 0;
	char wzLog[1024] = {0};
	char buffer[1024] = {0};
	va_list args;
	va_start(args, ms);
	vsprintf(wzLog, ms, args);
	va_end(args);

	time_t now;
	time(&now);
	struct tm *local;
	local = gmtime(&now);
#if DEBUG_MODE
	printf("[%04d-%02d-%02d %02d:%02d:%02d][LOG] %s\n",
		   local->tm_year + 1900,
		   local->tm_mon + 1,
		   local->tm_mday,
		   local->tm_hour,
		   local->tm_min,
		   local->tm_sec,
		   wzLog);
#endif
	if (0 == record_err)
	{
		sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d][LOG] %s\n",
				local->tm_year + 1900,
				local->tm_mon + 1,
				local->tm_mday,
				local->tm_hour,
				local->tm_min,
				local->tm_sec,
				wzLog);
		FILE *file = fopen(filename_cculog, "a+");
		if (NULL != file)
		{
			ret = fwrite(buffer, 1, strlen(buffer), file);
			fclose(file);
		}
	}
}

void _CCU_ERROR_(unsigned int error_code, const char *ms, ...)
{
	char wzLog[1024] = {0};
	char errLog[1024] = {0};
	char buffer[1024] = {0};

	switch (error_code)
	{
	case ERROR:
		strcat(wzLog, "UNKNOWN TYPE ERROR");
		break;
	case ERROR_CONFIG_INIT:
		strcat(wzLog, "ERROR_CONFIG_INIT");
		break;
	case ERROR_CAN_NAME_WRONG:
		strcat(wzLog, "ERROR_CAN_NAME_WRONG");
		break;
	case ERROR_CAN_SET_UP:
		strcat(wzLog, "ERROR_CAN_SET_UP");
		break;
	case ERROR_CAN_SET_DOWN:
		strcat(wzLog, "ERROR_CAN_SET_DOWN");
		break;
	case ERROR_CAN_CREATE_SOCKET:
		strcat(wzLog, "ERROR_CAN_CREATE_SOCKET");
		break;
	case ERROR_CAN_IOCTL:
		strcat(wzLog, "ERROR_CAN_IOCTL");
		break;
	case ERROR_CAN_BIND:
		strcat(wzLog, "ERROR_CAN_BIND");
		break;
	case ERROR_CAN_FCNTL_GET:
		strcat(wzLog, "ERROR_CAN_FCNTL_GET");
		break;
	case ERROR_CAN_FCNTL_SET:
		strcat(wzLog, "ERROR_CAN_FCNTL_SET");
		break;
	case ERROR_CAN_INIT:
		strcat(wzLog, "ERROR_CAN_INIT");
		break;
	case ERROR_ETH_RECV_INIT:
		strcat(wzLog, "ERROR_ETH_RECV_INIT");
		break;
	case ERROR_ETH_SEND_to_HKPro_INIT:
		strcat(wzLog, "ERROR_ETH_SEND_to_HKPro_INIT");
		break;
	case ERROR_ETH_SEND_to_6A_INIT:
		strcat(wzLog, "ERROR_ETH_SEND_to_6A_INIT");
		break;
	case ERROR_ETH_RECV_from_6A_INIT:
		strcat(wzLog, "ERROR_ETH_RECV_from_6A_INIT");
		break;
	case ERROR_ETH_MUTICAST_INIT:
		strcat(wzLog, "ERROR_ETH_MUTICAST_INIT");
		break;
	case ERROR_WATCHDOG_INIT:
		strcat(wzLog, "ERROR_WATCHDOG_INIT");
		break;
	case ERROR_THREAD_CREAT:
		strcat(wzLog, "ERROR_THREAD_CREAT");
		break;
	case ERROR_THREAD_INIT:
		strcat(wzLog, "ERROR_THREAD_INIT");
		break;
	case ERROR_THREAD_TEST_CREAT:
		strcat(wzLog, "ERROR_THREAD_TEST_CREAT");
		break;
	case ERROR_SRIALCOMM_INIT:
		strcat(wzLog, "ERROR_SRIALCOMM_INIT");
		break;
	case ERROR_SRIALCOMM_OPEN:
		strcat(wzLog, "ERROR_SRIALCOMM_OPEN");
		break;
	case ERROR_SIGNAL_INIT:
		strcat(wzLog, "ERROR_SIGNAL_INIT");
		break;
	case ERROR_AI_INIT:
		strcat(wzLog, "ERROR_AI_INIT");
		break;
	case ERROR_THREAD_TEST:
		strcat(wzLog, "ERROR_THREAD_TEST");
		break;
	case ERROR_THREAD_REBOOT:
		strcat(wzLog, "ERROR_THREAD_REBOOT");
		break;
	case ERROR_THREAD_GPIO:
		strcat(wzLog, "ERROR_THREAD_GPIO");
		break;
	case ERROR_THREAD_POWER_SUPPLY:
		strcat(wzLog, "ERROR_THREAD_POWER_SUPPLY");
		break;
	case ERROR_THREAD_FIRE_EXTINGUISHING:
		strcat(wzLog, "ERROR_THREAD_FIRE_EXTINGUISHING");
		break;
	case ERROR_THREAD_FIRE_EXTINGUISHING_ACK:
		strcat(wzLog, "ERROR_THREAD_FIRE_EXTINGUISHING_ACK");
		break;
	case ERROR_THREAD_PRESS_DETECT:
		strcat(wzLog, "ERROR_THREAD_PRESS_DETECT");
		break;
	case ERROR_THREAD_OPEN_CIRCUIT_DETECT:
		strcat(wzLog, "ERROR_THREAD_OPEN_CIRCUIT_DETECT");
		break;
	case ERROR_THREAD_SMOKE_CAN_SENSOR_DETECT_SEND:
		strcat(wzLog, "ERROR_THREAD_SMOKE_CAN_SENSOR_DETECT_SEND");
		break;
	case ERROR_THREAD_CAN_SMOKE_SENSOR_DETECT_RECV:
		strcat(wzLog, "ERROR_THREAD_CAN_SMOKE_SENSOR_DETECT_RECV");
		break;
	case ERROR_THREAD_DIP_SWITCH:
		strcat(wzLog, "ERROR_THREAD_DIP_SWITCH");
		break;
	case ERROR_THREAD_CAN_SMOKE_SENSOR_CTRL:
		strcat(wzLog, "ERROR_THREAD_CAN_SMOKE_SENSOR_CTRL");
		break;
	case ERROR_THREAD_CAN_OPENCIRCUIT_DETECT_RECV:
		strcat(wzLog, "ERROR_THREAD_CAN_OPENCIRCUIT_DETECT_RECV");
		break;
	case ERROR_THREAD_CAN_OPENCIRCUIT_DETECT_SEND:
		strcat(wzLog, "ERROR_THREAD_CAN_OPENCIRCUIT_DETECT_SEND");
		break;
	case ERROR_THREAD_CAN_SHORTCIRCUIT_DETECT:
		strcat(wzLog, "ERROR_THREAD_CAN_SHORTCIRCUIT_DETECT");
		break;
	case ERROR_DATA_LOG_INIT:
		strcat(wzLog, "ERROR_DATA_LOG_INIT");
		break;
	case ERROR_CAN_LOGIN_LOG_INIT:
		strcat(wzLog, "ERROR_CAN_LOGIN_LOG_INIT");
		break;
	case ERROR_NET_COMMUNICATION:
		strcat(wzLog, "ERROR_NET_COMMUNICATION");
		break;
	case ERROR_MEMORY_ALLOCATION:
		strcat(wzLog, "ERROR_MEMORY_ALLOCATION");
		break;
	default:
		strcat(wzLog, "UNKNOWN TYPE ERROR");
		break;
	}

	va_list args;
	va_start(args, ms);
	vsprintf(errLog, ms, args);
	va_end(args);

	strcat(wzLog, " ");
	strcat(wzLog, errLog);

	time_t now;
	time(&now);
	struct tm *local;
	local = gmtime(&now);
#if DEBUG_MODE
	printf("[%04d-%02d-%02d %02d:%02d:%02d][ERROR] %s\n",
		   local->tm_year + 1900,
		   local->tm_mon + 1,
		   local->tm_mday,
		   local->tm_hour,
		   local->tm_min,
		   local->tm_sec,
		   wzLog);
#endif
	if (0 == record_err)
	{
		sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d][ERROR] %s\n",
				local->tm_year + 1900,
				local->tm_mon + 1,
				local->tm_mday,
				local->tm_hour,
				local->tm_min,
				local->tm_sec,
				wzLog);
		FILE *file = fopen(filename_cculog, "a+");
		if (NULL != file)
		{
			fwrite(buffer, 1, strlen(buffer), file);
			fclose(file);
		}
	}
}

void get_crc(unsigned char *CmmBuf, unsigned char Len)
{
	unsigned short crc = 0xFFFF;
	unsigned char i;
	unsigned char j;
	// if (Len > 78)
	// 	return;
	j = 0;
	while (j < Len)
	{
		crc ^= CmmBuf[j];
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
		j++;
	}
	CmmBuf[j] = crc % 0x100;
	CmmBuf[j + 1] = crc / 0x100;
}

unsigned short checksum(unsigned char *buf, int len) // for 6A
{
	int i = 0;
	unsigned short check = 0;
	for (i = 0; i < len; i++)
	{
		check += (*(buf + i));
	}
	return check;
}

unsigned short checksum2(unsigned char *buf, int len) // for camera
{
	int i = 0;
	unsigned short ret = 0;

	for (i = 2; i < len - 2; i++)
	{
		ret += (*(buf + i));
	}
	ret += 1;

	return ret;
}

unsigned char sc_check(unsigned char *p1, unsigned short int len) // for can
{
	unsigned char sum = 0;
	for (; len > 0; len--)
	{
		sum += p1[len - 1];
	}
	return sum;
}

int get_gpio_status(int num)
{
	int ret = 0;
	GPIO_INFO info = {0};
	info.num = num;
	info.val = 0; /*out value 0*/

	ret = ioctl(gpiofd, EIM_IOC_RD, &info);
	return info.val;
}

int set_gpio_status(int num, int onoff)
{
	int ret = 0;
	GPIO_INFO info = {0};

	info.num = num;
	info.val = onoff;

	ret = ioctl(gpiofd, EIM_IOC_WR, &info);

	return ret;
}

unsigned char *hex_string_to_ACII_string(unsigned char *hex_buff, int hex_len)
{
	int i = 0;

	for (int i = hex_len - 1; i >= 0; i--)
	{
		if (0x00 == hex_buff[i])
		{
			hex_buff[i] = '\0';
		}
		else
		{
			break;
		}
	}

	return hex_buff;
}
