#pragma once
#ifndef _SENSE_HAT_LPS25H
#define _SENSE_HAT_LPS25H

#include "util.h"

// https://www.st.com/resource/en/datasheet/lps25hb.pdf

#define LPS25H_I2C_ID         0xbd
#define LPS25H_AUTO_ADVANCE   0x80

#define LPS25H_WHO_AM_I       0x0F
#define LPS25H_RES_CONF       0x10
#define LPS25H_CTRL_REG1      0x20
#define LPS25H_CTRL_REG2      0x21
#define LPS25H_STATUS         0x27
#define LPS25H_PRESS_OUT_XL   0x28
#define LPS25H_TEMP_OUT_L     0x2B
#define LPS25H_FIFO_CTRL      0x2E


typedef struct {
    bool isInitialized;
    int fd;
    unsigned char i2c_addr;
} LPS25H_CONFIG;

typedef struct {
    double temperature;
    double pressure;
    bool isTemperatureValid;
    bool isPressureValid;
} LPS25H_DATA;

LPS25H_CONFIG* lps25h_initialize(int fd);
void lps25h_uninitialize(LPS25H_CONFIG* config);
int lps25h_read(LPS25H_CONFIG* config, LPS25H_DATA* data);

#endif