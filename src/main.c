/*
 *
 * _______  ______          _    ____ _____    __        __         _    _                     _
 *|  ___\ \/ /  _ \        / \  | ___|___ /    \ \      / /__  _ __| | _| |__   ___ _ __   ___| |__
 *| |_   \  /| | | |_____ / _ \ |___ \ |_ \ ____\ \ /\ / / _ \| '__| |/ / '_ \ / _ \ '_ \ / __| '_ \
 *|  _|  /  \| |_| |_____/ ___ \ ___) |__) |_____\ V  V / (_) | |  |   <| |_) |  __/ | | | (__| | | |
 *|_|   /_/\_\____/     /_/   \_\____/____/       \_/\_/ \___/|_|  |_|\_\_.__/ \___|_| |_|\___|_| |_|
 *
 */

/*
 ============================================================================
 Name        : main.c
 Author      : Wang Chyng-chuan
 Version     :
 Copyright   : CRRC
 Description :
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <termios.h>
#include <dirent.h>
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
#include "signalfunction.h"
#include "gpio.h"
#include "can.h"
#include "eth.h"
#include "mcp3201.h"

int init_thread()
{
	int ret = 0;
	struct sched_param param;
	int policy, inher;

	int ret_pthread_can_sd_create = -1;
	int ret_pthread_can_rec_create = -1;
	int ret_pthread_eth_send_create = -1;
	int ret_pthread_eth_recv_create = -1;
	int ret_pthread_eth_send_for_6A_create = -1;
	int ret_pthread_eth_recv_for_6A_create = -1;
	int ret_pthread_eth_muticast_recv_for_create = -1;
	int ret_pthread_gpio_create = -1;
	int ret_pthread_rs485_create = -1;
	int ret_pthread_func_reboot_create = -1;
	int ret_pthread_dido_create = -1;
	int ret_pthread_ctrl_create = -1;
	int ret_pthread_ai_create = -1;
	int ret_pthread_test_create = -1;
	int ret_pthread_DC_power_supply_create = -1;
	int ret_pthread_fire_extinguishing_create = -1;
	int ret_pthread_fire_extinguishing_ack_create = -1;
	int ret_pthread_press_detect_create = -1;
	int ret_pthread_open_circult_detect_create = -1;
	int ret_pthread_smoke_can_sensor_detect_send_create = -1;
	int ret_pthread_can_smoke_sensor_detect_recv_create = -1;
	int ret_pthread_can_shortcircuit_recv_create = -1;
	int ret_pthread_can_opencircuit_recv_create = -1;
	int ret_pthread_can_opencircuit_send_create = -1;
	int ret_pthread_DIP_switch_create = -1;
	int ret_pthread_can_smoke_sensor_ctrl = -1;
	int ret_pthread_can_send = -1;

	pthread_t id_can_sd;
	pthread_t id_can_re;
	pthread_t id_eth_send;
	pthread_t id_eth_send_to_6A;
	pthread_t id_eth_recv;
	pthread_t id_eth_recv_from_6A;
	pthread_t id_eth_recv_from_by_muticast;
	pthread_t id_gpio;
	pthread_t id_test;
	pthread_t id_rs485;
	pthread_t id_ctrl;
	pthread_t id_reboot;
	pthread_t id_DC_power_supply;
	pthread_t id_extinguish;
	pthread_t id_extinguish_ack;
	pthread_t id_press_detect;
	pthread_t id_circuit_detect;
	pthread_t id_smoke_can_sensor_detect_send;
	pthread_t id_can_smoke_sensor_detect_recv;
	pthread_t id_can_shortcircuit_detect;
	pthread_t id_can_opencircuit_detect_recv;
	pthread_t id_can_opencircuit_detect_send;
	pthread_t id_DIP_switch;
	pthread_t id_can_smoke_senor_ctrl;
	pthread_t id_dido;
	pthread_t id_ai;
	pthread_t id_led;
	pthread_t id_can_send;

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_getinheritsched(&attr, &inher);
	if (inher == PTHREAD_INHERIT_SCHED)
	{
		inher = PTHREAD_EXPLICIT_SCHED;
	}
	pthread_attr_setinheritsched(&attr, inher);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);

	param.__sched_priority = PRIO_TASK_CAN_SEND;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_can_send = pthread_create(&id_can_send, &attr, (void *)thread_func_can_buff_send, NULL);
	if (ret_pthread_can_send != 0)
	{
		_CCU_ERROR_(ERROR_CAN_SEND, "thread_func_can_buff_send");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_ETH_RECV;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_eth_recv_create = pthread_create(&id_eth_recv, &attr, (void *)thread_eth_server, NULL);
	if (ret_pthread_eth_recv_create != 0)
	{
		_CCU_ERROR_(ERROR_ETH_RECV_INIT, "thread_eth_server");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_ETH_SEND;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_eth_send_create = pthread_create(&id_eth_send, &attr, (void *)thread_eth_client, NULL);
	if (ret_pthread_eth_send_create != 0)
	{
		_CCU_ERROR_(ERROR_ETH_SEND_to_HKPro_INIT, "thread_eth_client");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_ETH_for_RECV_MUTICAST;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_eth_muticast_recv_for_create = pthread_create(&id_eth_recv_from_by_muticast, &attr, (void *)thread_eth_for_muticast_recv, NULL);
	if (ret_pthread_eth_muticast_recv_for_create != 0)
	{
		_CCU_ERROR_(ERROR_ETH_MUTICAST_INIT, "thread_eth_for_muticast_recv");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_GPIO;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_gpio_create = pthread_create(&id_led, &attr, (void *)thread_func_gpio, NULL);
	if (ret_pthread_gpio_create != 0)
	{
		_CCU_ERROR_(ERROR_THREAD_GPIO, "thread_func_gpio");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_REBOOT;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_func_reboot_create = pthread_create(&id_reboot, &attr, (void *)thread_func_reboot, NULL);
	if (ret_pthread_func_reboot_create != 0)
	{
		_CCU_ERROR_(ERROR_THREAD_REBOOT, "thread_func_reboot");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_DC_POWER_SUPPLY;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_DC_power_supply_create = pthread_create(&id_DC_power_supply, &attr, (void *)thread_func_power_supply, NULL);
	if (ret_pthread_DC_power_supply_create != 0)
	{
		_CCU_ERROR_(ERROR_THREAD_POWER_SUPPLY, "thread_func_power_supply");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_PRESS_DETECT;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_press_detect_create = pthread_create(&id_press_detect, &attr, (void *)thread_func_AI_detect, NULL);
	if (ret_pthread_press_detect_create != 0)
	{
		_CCU_ERROR_(ERROR_THREAD_PRESS_DETECT, "thread_func_AI_detect");
		return EXIT_FAILURE;
	}

	param.__sched_priority = PRIO_TASK_CAN_SMOKE_SENSOR_DETECT_RECV;
	pthread_attr_setschedparam(&attr, &param);
	ret_pthread_can_smoke_sensor_detect_recv_create = pthread_create(&id_can_smoke_sensor_detect_recv, &attr, (void *)thread_func_can_smoke_sensor_detect_recv, NULL);
	if (ret_pthread_can_smoke_sensor_detect_recv_create != 0)
	{
		_CCU_ERROR_(ERROR_THREAD_CAN_SMOKE_SENSOR_DETECT_RECV, "thread_func_can_smoke_sensor_detect_recv");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if (-1 != watchdogfd)
		{
			ioctl(watchdogfd, WDIOC_KEEPALIVE); // keep watchdog alive
		}

		sleep(1);
	}

	pthread_attr_destroy(&attr);

	return 0;
}

int main(void)
{
	// Init log file path
	init_logfile();

	// Init data log file path
	init_data_logfile();

	// Init signal process
	_CCU_LOG_("Init signal process start.");
	if (init_sinal())
		_CCU_ERROR_(ERROR_SIGNAL_INIT, "Init signal register ERROR");

	// Read Configuration
	_CCU_LOG_("Init configuration start.");
	if (init_config(FILE_CONFIG))
		_CCU_ERROR_(ERROR_CONFIG_INIT, FILE_CONFIG);

	// Init gpio
	_CCU_LOG_("Init GPIO start.");
	if (init_gpio(GPIO_NAME))
		_CCU_ERROR_(ERROR_GPIO_INIT, GPIO_NAME);

	// Init can
	_CCU_LOG_("Init Can device start.");
#if TEST_MODE
	if (init_can(CAN1_NAME, CAN1_BITRATE_for_TEST))
		_CCU_ERROR_(ERROR_CAN_INIT, CAN1_NAME);

	if (init_can(CAN0_NAME, CAN0_BITRATE_for_TEST))
		_CCU_ERROR_(ERROR_CAN_INIT, CAN0_NAME);
#else
	if (init_can(CAN1_NAME, CAN1_BITRATE))
		_CCU_ERROR_(ERROR_CAN_INIT, CAN1_NAME);

	if (init_can(CAN0_NAME, CAN0_BITRATE))
		_CCU_ERROR_(ERROR_CAN_INIT, CAN0_NAME);
#endif

	// Init watchdog
	_CCU_LOG_("Init watchdog start.");
	if (init_watchdog(WATCHDOG_NAME, WATCHDOG_TIMEOUT))
		_CCU_ERROR_(ERROR_WATCHDOG_INIT, WATCHDOG_NAME);

	// Init IP set, must after gpio init
	_CCU_LOG_("Init IP set start.");
	if (init_DIP_switch())
		_CCU_ERROR_(ERRPR_DIP_INIT, "Init IP set ERROR");

	sleep(5); // wait for eth_converter_cabinet_No setting

	// Init pthread
	_CCU_LOG_("Init thread start.");
	if (init_thread())
		_CCU_ERROR_(ERROR_THREAD_INIT, "Init some or all threads failed");

	return 0;
}