/*
 * thread_can.c
 *
 *  Created on: 2023.11.1
 *      Author: Wang Chyng-chuan
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <signal.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/un.h>
#include <errno.h>
#include <linux/can/raw.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <termios.h>

#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"
#include "can.h"
#include "can_netlink.h"
#include "libsocketcan.h"

int init_can(char *name, char *bitrate)
{
    int result = 0;
    int flag = 0;
    int fd = 0;
    struct ifreq ifr;
    struct sockaddr_can can_addr;

    char cmd[1024] = {0};

    if ((strcmp(name, "can0") != 0) && (strcmp(name, "can1") != 0))
    {
        _CCU_ERROR_(ERROR_CAN_NAME_WRONG, name);
        return EXIT_FAILURE;
    }

    strcat(cmd, "ip link set ");
    strcat(cmd, name);
    strcat(cmd, " down");

    result = system(cmd);

    if ((-1 == result) || (0 == WIFEXITED(result)) || (0 != WEXITSTATUS(result)))
    {
        _CCU_ERROR_(ERROR_CAN_SET_DOWN, name);
        return EXIT_FAILURE;
    }

    memset(cmd, 0, sizeof(cmd));
    strcat(cmd, "ip link set ");
    strcat(cmd, name);
    strcat(cmd, " up type can bitrate ");
    strcat(cmd, bitrate);
    strcat(cmd, " sample-point 0.68 triple-sampling on");
    strcat(cmd, " restart-ms 100");

    result = system(cmd);
    if ((-1 == result) || (0 == WIFEXITED(result)) || (0 != WEXITSTATUS(result)))
    {
        _CCU_ERROR_(ERROR_CAN_SET_UP, name);
        return EXIT_FAILURE;
    }

    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (-1 == fd)
    {
        _CCU_ERROR_(ERROR_CAN_CREATE_SOCKET, name);
        return EXIT_FAILURE;
    }

    strcpy((char *)(ifr.ifr_name), name);

    result = ioctl(fd, SIOCGIFINDEX, &ifr);
    if (-1 == result)
    {
        _CCU_ERROR_(ERROR_CAN_IOCTL, name);
        return EXIT_FAILURE;
    }

    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;

    result = bind(fd, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if (-1 == result)
    {
        _CCU_ERROR_(ERROR_CAN_BIND, name);
        return EXIT_FAILURE;
    }

    flag = fcntl(fd, F_GETFL, 0);
    if (-1 == flag)
    {
        _CCU_ERROR_(ERROR_CAN_FCNTL_GET, name);
        return EXIT_FAILURE;
    }

    result = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (-1 == result)
    {
        _CCU_ERROR_(ERROR_CAN_FCNTL_SET, name);
        return EXIT_FAILURE;
    }

    if (strcmp(name, "can0") == 0)
    {
        canfd[CAN_0] = fd;
    }

    if (strcmp(name, "can1") == 0)
    {
        canfd[CAN_1] = fd;
    }

    set_gpio_status(CAN_EN, POWER_OFF);
    usleep(100000); /*100ms */
    set_gpio_status(CAN_EN, POWER_ON);

    set_gpio_status(CAN_STB, POWER_ON);

    _CCU_LOG_("Init CAN device %s init success. Bitrate:%s", name, bitrate);

    return 0;
}

int can_opencircuit_detect_send()
{
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame[2];

    frame[0].can_id = 0x11;
    frame[0].can_dlc = 1;
    frame[0].data[0] = 0xB0;

    frame[1].can_id = 0x22;
    frame[1].can_dlc = 1;
    frame[1].data[0] = 0xA0;

    while (1)
    {
        nbytes = write(canfd[CAN_0], &frame[0], sizeof(frame[0])); // send frame[0]

        nbytes = write(canfd[CAN_1], &frame[1], sizeof(frame[1])); // send frame[1]

        sleep(2);
    }

    return 0;
}

void thread_func_can_buff_send()
{
    while (1)
    {
        can_opencircuit_detect_send();
    }

    return;
}

void thread_func_can_smoke_sensor_detect_recv()
{
    int result[2] = {0};
    CAN_ID can_id[2] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
    struct can_frame recv_frame[2] = {{0, 0, 0, 0, 0, {0}}, {0, 0, 0, 0, 0, {0}}};

    while (1)
    {
        memset(result, 0, sizeof(result));
        memset(can_id, 0, sizeof(can_id));
        memset(recv_frame, 0, sizeof(recv_frame));

        result[CAN_0] = read(canfd[CAN_0], &recv_frame[CAN_0], sizeof(recv_frame[CAN_0]));
        result[CAN_1] = read(canfd[CAN_1], &recv_frame[CAN_1], sizeof(recv_frame[CAN_1]));

        if (result[CAN_0] > 0)
        {
            ///////////////////////////////////////////
            // printf("CAN_0 RECV:0x%02x %d ", recv_frame[CAN_0].can_id, recv_frame[CAN_0].can_dlc);
            // printf("\n");
            // for (int i = 0; i < recv_frame[CAN_0].can_dlc; i++)
            // {
            //     printf("%02X ", recv_frame[CAN_0].data[i]);
            // }
            // printf("\n");
            ///////////////////////////////////////////

            // CAN0 open circuit detect
            if (1 == recv_frame[CAN_0].can_dlc)
            {
                if (recv_frame[CAN_0].data[0] == 0xA0 && recv_frame[CAN_0].can_id == 0x22)
                {
                }
            }
        }

        if (result[CAN_1] > 0)
        {
            //////////////////////////////
            // printf("CAN_1 RECV:0x%02x %d ", recv_frame[CAN_1].can_id, recv_frame[CAN_1].can_dlc);
            // printf("\n");
            // for (int i = 0; i < recv_frame[CAN_1].can_dlc; i++)
            // {
            //     printf("%02X ", recv_frame[CAN_1].data[i]);
            // }
            // printf("\n");
            //////////////////////////////

            // CAN1 open circuit detect
            if (1 == recv_frame[CAN_1].can_dlc)
            {
                if (recv_frame[CAN_1].data[0] == 0xB0 && recv_frame[CAN_1].can_id == 0x11)
                {
                }
            }
        }

        usleep(100000); // 100ms
    }

    return;
}