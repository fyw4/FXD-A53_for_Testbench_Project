/*
 * globalfunction.h
 *
 *  Created on: 2023.09.08
 *      Author: Wang Chyng-chuan
 */

#ifndef GLOBALFUNCTION_H_
#define GLOBALFUNCTION_H_

int init_watchdog(char *name, int timeout);

int init_config(char *name);

int init_logfile();

int init_data_logfile();

int init_can_login_data_logfile();

void _CCU_LOG_(const char *ms, ...);

void _CCU_DATA_LOG_(const char *ms, ...);

void _CCU_CAN_LOGIN_DATA_LOG_(const char *ms, ...);

void _CCU_ERROR_(unsigned int error_code, const char *ms, ...);

void get_crc(unsigned char *CmmBuf, unsigned char Len);

unsigned short checksum(unsigned char *buf, int len);

unsigned short checksum2(unsigned char *buf, int len);

unsigned char sc_check(unsigned char *p1, unsigned short int len);

int get_gpio_status(int num);

int set_gpio_status(int num, int onoff);

unsigned char *hex_string_to_ACII_string(unsigned char *hex_buff, int hex_len);

#endif /* GLOBALFUNCTION_H_ */
