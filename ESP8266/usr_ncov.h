#ifndef __NCOV_DATA_H__
#define __NCOV_DATA_H__

#include "wifista.h"
#include "malloc.h"
#include "drv_usart2.h"
#include "delay.h"
#include "cJSON.h"

#include "string.h"
#include "stdio.h"


#define API_NCOV     "https://lab.isaaclin.cn/nCoV/api/overall"

struct ncov_data{
    long currentConfirmedCount;
    long currentConfirmedIncr;
    long confirmedCount;
    long confirmedIncr;
    long curedCount;
    long curedIncr;
    long seriousCount;
    long seriousIncr;
    long deadCount;
    long deadIncr;
    char updateTime[20];
};

extern struct ncov_data dataChina;
extern struct ncov_data dataGlobal;

uint8_t build_ssl_connect(char *type, char *ip, char *port);
uint8_t get_ncov_api(char *api_addr, uint8_t (*parse_fun)(void));

uint8_t parse_ncov_data(void);


#endif


