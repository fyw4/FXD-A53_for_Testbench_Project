/*
 * eth.c
 *
 *  Created on: 2026.3.5
 *      Author: Wang Chyng-chuan
 */
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>
#include <netinet/in.h>

#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"
#include "eth.h"

void thread_eth_server()
{
    int flag = 0;
    int i = 0, fd = 0, ret = 0, recv_len = 0;
    char buf[1200] = {0};
    struct sockaddr_in recv_addr = {0};

    DATA_DOUBLE_SEP accumulate_check;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_server:socket error");
        return;
    }
    flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK); // nonblock

    struct sockaddr_in myaddr = {0};
    socklen_t addr_len = sizeof(struct sockaddr);
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(SERVER_PORT);

    ret = bind(fd, (struct sockaddr *)&myaddr, addr_len);
    if (ret < 0)
    {
        perror("thread_eth_server:bind error");
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_server:bind error");
        close(fd);
        return;
    }

    while (1)
    {
        if(extinguish_info_message_to_Workbench == NULL)
        {
            usleep(100000); // 100ms
            continue;
        }

        if ((recv_len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&recv_addr, &addr_len)) > 0)
        {
            if (buf[0] == 0xAA && buf[1] == 0xAA)
            {
                memcpy(&accumulate_check, &buf[18], sizeof(DATA_DOUBLE_SEP));
                if (accumulate_check.val_dec == checksum((unsigned char *)buf, recv_len - 2))
                {
                    extinguish_info_message_to_Workbench->workbench_command_byte_NO_1 = buf[5];
                    extinguish_info_message_to_Workbench->workbench_command_byte_NO_2 = buf[6];
                }

                printf("recv: ");
                for (int i = 0; i < recv_len; i++)
                    printf("%02x ", buf[i]);
                printf("\n");
            }
        }
        sleep(1);
    }

    close(fd);
    return;
}

void thread_eth_client()
{
    ssize_t send_len = 0;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_client: fd socket create fail!");
        return;
    }

    struct sockaddr_in dstaddr;
    dstaddr.sin_family = AF_INET;
    dstaddr.sin_port = htons(CLIENT_PORT);
    dstaddr.sin_addr.s_addr = inet_addr(IP_6A_ADDRESS_for_A);

    unsigned char send_buff_to_Workbench[FROM_WORKBENCH_MESSAGE_LEN] = {0};
    extinguish_info_message_to_Workbench = (EXTINGUISH_INFO_MESSAGE_to_Workbench *)malloc(sizeof(EXTINGUISH_INFO_MESSAGE_to_Workbench));
    memset(extinguish_info_message_to_Workbench, 0, sizeof(EXTINGUISH_INFO_MESSAGE_to_Workbench));
    unsigned char lifecount = 0x00;
    DATA_DOUBLE_SEP check;

    extinguish_info_message_to_Workbench->header.header_l = 0xAA;
    extinguish_info_message_to_Workbench->header.header_h = 0xAA;
    extinguish_info_message_to_Workbench->len = 0x14;
    extinguish_info_message_to_Workbench->flag = 0x02;

    while (1)
    {
        extinguish_info_message_to_Workbench->life_count = lifecount++;
        if (lifecount >= 0xFF)
        {
            lifecount = 0x00;
        }

        memset(send_buff_to_Workbench, 0, sizeof(send_buff_to_Workbench));
        memcpy(send_buff_to_Workbench, extinguish_info_message_to_Workbench, sizeof(EXTINGUISH_INFO_MESSAGE_to_Workbench));
        check.val_dec = checksum((unsigned char *)send_buff_to_Workbench, FROM_WORKBENCH_MESSAGE_LEN - 2);
        send_buff_to_Workbench[FROM_WORKBENCH_MESSAGE_LEN - 2] = check.val_chr[0];
        send_buff_to_Workbench[FROM_WORKBENCH_MESSAGE_LEN - 1] = check.val_chr[1];

        send_len = sendto(fd, send_buff_to_Workbench, FROM_WORKBENCH_MESSAGE_LEN, 0, (struct sockaddr *)&dstaddr, sizeof(dstaddr));
        if (send_len < 0)
        {
            perror("thread_eth_client send fail!\r\n");
        }

        printf("send: ");
        for (int i = 0; i < send_len; i++)
            printf("%02x ", send_buff_to_Workbench[i]);
        printf("\n");

        sleep(1);
    }

    free(extinguish_info_message_to_Workbench);
    close(fd);
    return;
}

void thread_eth_for_muticast_recv()
{
    int i = 0;
    int ret = 0;
    int flag = 0;
    int count = 0;
    int count_for_extinguish_carbinet_NO_1 = 0;
    int count_for_extinguish_carbinet_NO_2 = 0;
    int sockfd = 0;
    int recv_len = 0;
    unsigned char buff[1024] = {0};
    unsigned char recv_compare_buf[1024] = {0};
    DATA_DOUBLE_SEP accumulate_check = {0};
    struct sockaddr_in recv_addr = {0};
    struct ip_mreq vmreq;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_for_muticast_recv:socket error!");
        return;
    }
    flag = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK); // nonblock

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(GROUP_PORT);

    ret = bind(sockfd, (struct sockaddr *)&local_addr, addr_len);
    if (ret < 0)
    {
        perror("thread_eth_for_muticast_recv:bind error!\n");
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_for_muticast_recv:bind error!");
        close(sockfd);
        return;
    }

    vmreq.imr_multiaddr.s_addr = inet_addr(GROUP_IP);
    vmreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &vmreq, sizeof(vmreq));
    if (ret < 0)
    {
        perror("thread_eth_for_muticast_recv:setsockopt error!\n");
        _CCU_ERROR_(ERROR_NET_COMMUNICATION, "thread_eth_for_muticast_recv:setsockopt error! MAYBE IPconfig file is not set gateway!");
        close(sockfd);
        return;
    }

    while (1)
    {

        usleep(20000); // 20ms
    }

    close(sockfd);
    return;
}
