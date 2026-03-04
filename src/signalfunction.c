/*
 * signalfuntion.c
 *
 *  Created on: 2023.09.11
 *      Author: Wang Chyng-chuan
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "signalfunction.h"
#include "globaldefine.h"
#include "globalvariable.h"
#include "globalfunction.h"

void dump(void)
{
    int nptrs = 0;
    void *buffer[BACKTRACE_SIZE] = {0};
    char **strings;

    nptrs = backtrace(buffer, BACKTRACE_SIZE);

    _CCU_LOG_("backtrace() returned %d addresses.", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (NULL == strings)
    {
        _CCU_LOG_("backtrace_symbols.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nptrs; i++)
    {
        _CCU_LOG_("%s", strings[i]);
    }

    free(strings);

    return;
}

void watchdog_close()
{
    int options = WDIOS_DISABLECARD;

    if (ioctl(watchdogfd, WDIOC_SETOPTIONS, &options) < 0)
    {
        _CCU_LOG_("ioctl WDIOC_SETOPTIONS failed.");
    }

    if (close(watchdogfd) < 0)
    {
        _CCU_LOG_("close watchdogfd failed.");
    }
    return;
}

void signal_handler(int signo)
{
    if (SIGSEGV == signo)
    {
        _CCU_LOG_("SIGSEGV accur, now stop process.");
        dump();
        watchdog_close();
        signal(signo, SIG_DFL); // default signal handle
        raise(signo);           // send signal
    }
    else if (SIGINT == signo)
    {
        _CCU_LOG_("SIGINT accur, now stop process.");
        watchdog_close();
        signal(signo, SIG_DFL); // default signal handle
        raise(signo);           // send signal
    }
    else if (SIGRTMAX == signo)
    {
        _CCU_LOG_("SIGRTMAX accur, now system reboot.");
        watchdog_close();
        system("reboot");
    }
    else if (SIGTERM == signo)
    {
        _CCU_LOG_("SIGTERM accur, now stop process.");
        watchdog_close();
        signal(signo, SIG_DFL); // default signal handle
        raise(signo);           // send signal
    }

    return;
}

int init_sinal(void)
{
    signal(SIGSEGV, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGRTMAX, signal_handler);
    signal(SIGTERM, signal_handler);

    return 0;
}