/*
 *  gpio.h
 *
 *  Created on: 2023.09.08
 *      Author: Wang Chyng-chuan
 */
#ifndef GPIO_H_
#define GPIO_H_

#define REBOOT_WAIT_TIME_MIN 0.5
#define REBOOT_WAIT_TIME_MAX 3
#define LOGIN_WAIT_TIME_MIN 5
#define COUNT_TIME 1

#define EXTINGUISH_TIME 15 // 15s
#define VIDEO_RECORD_COUNT 10
#define EXTINGUISH_TMPERATURE_TIME 3 // 3s
#define EXTINGUISH_PRESSED_TIME 2    // 2s
#define OPEN_CIRCUIT_TIME 2          // 2s

#define TIME_COUNT_for_POWER_RESET 9       // 9s
#define TIME_COUNT_for_POWER_OPEN_CIRCUIT 3 // 3s

int init_gpio(char *name);

void thread_func_reboot();

void thread_func_gpio();

void thread_func_power_supply();

int init_GPIO_DO();

int init_DIP_switch();

#endif