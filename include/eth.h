/*
 * eth.h
 *
 *  Created on: 2023.11.3
 *      Author: Wang Chyng-chuan
 */

#ifndef ETH_H_
#define ETH_H_

#define SERVER_PORT 8080
#define CLIENT_PORT 8000

#define IP_LOCAL_ADDRESS "127.0.0.1"
#define IP_CONVERTER_CABINET_NO_A_1_ADDRESS "192.168.62.11"
#define IP_CONVERTER_CABINET_NO_A_2_ADDRESS "192.168.62.12"
#define IP_CONVERTER_CABINET_NO_B_1_ADDRESS "192.168.62.61"
#define IP_CONVERTER_CABINET_NO_B_2_ADDRESS "192.168.62.62"

#define IP_6A_ADDRESS_for_A "192.168.60.11"
#define IP_6A_BACKUP_ADDRESS_for_A "192.168.60.12"
#define IP_6A_ADDRESS_for_B "192.168.60.31"
#define IP_6A_BACKUP_ADDRESS_for_B "192.168.60.32"
#define IP_6A_ADDRESS_for_test "192.168.62.123"
#define IP_6A_BACKUP_ADDRESS_for_test "192.168.62.124"
#define GROUP_IP "239.255.60.82"
#define GROUP_PORT 7300
#define GPOUP_MESSAGE_LEN 64
#define FROM_6A_MESSAGE_LEN 96
#define CLIENT_for_6A_PORT 7000

#define IP_6A_ADDRESS_for_PTU_TEST "192.168.61.124"
#define GROUP_IP_for_PTU_TEST "239.255.60.81"
#define GROUP_PORT_for_PTU_TEST 7200
#define CLIENT_for_6A_PORT_for_PTU_TEST 7000

#define CAMRA_PRECAUSION_VALUE_LIMIT 150
#define CAMRA_VALUE_LIMIT 180

#define CAMERA_TMPERATURE_TIME 2

#define EXTINGUISH_TIME_WAIT 1500

#define ONLINE_to_OFFLINE_COUNT 3

#pragma pack (1)
typedef enum all_eth_package_length
{
    LENGTH_SEND = 40,
    to_6A_LENGH_SEND = 256
} ALL_DETECT_PACKAGE_LENGTH;

typedef struct eth_send_package
{
    PACKAGE_HEADER header;
    unsigned char version;
    unsigned char converter_cabinet_No;
    unsigned char camera_begin_record_video_flag;
    PACKAGE_LENGTH loco_speed;
    unsigned char kilometers_sign[3];
    unsigned char train_num_letter[4];
    unsigned char train_num_number[4];
    unsigned char loco_num[14];
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
} ETH_SEND_PACKAGE;

#pragma pack ()

void thread_eth_server();

void thread_eth_client();

void thread_eth_for_muticast_recv();

#endif
