#pragma once
#ifndef _SENSE_HAT_HTS221
#define _SENSE_HAT_HTS221

#include "util.h"

// https://www.st.com/resource/en/datasheet/hts221.pdf

#define HTS221_I2C_ID         0xBC
#define HTS221_AUTO_ADVANCE   0x80

// From Table 6 Page 20
#define HTS221_WHO_AM_I       0x0F
#define HTS221_AV_CONF        0x10
#define HTS221_CTRL_REG1      0x20
#define HTS221_CTRL_REG2      0x21
#define HTS221_CTRL_REG3      0x22
#define HTS221_STATUS         0x27
#define HTS221_HUMIDITY_OUT_L 0x28
#define HTS221_HUMIDITY_OUT_H 0x29
#define HTS221_TEMP_OUT_L     0x2A
#define HTS221_TEMP_OUT_H     0x2B
#define HTS221_CALIB_0        0x30

#define HTS221_H0_H_2         0x30
#define HTS221_H1_H_2         0x31
#define HTS221_T0_DEGC_X8     0x32
#define HTS221_T1_DEGC_X8     0x33

#define HTS221_T1_T0_MSB      0x35
#define HTS221_H0_T0_OUT      0x36
#define HTS221_H1_T0_OUT      0x3A

#define HTS221_T0_OUT         0x3C
#define HTS221_T1_OUT         0x3E

typedef struct {
    bool isInitialized;
    int fd;
    unsigned char i2c_addr;
    double temperature_m;
    double temperature_c;
    double humidity_m;
    double humidity_c;
} HTS221_CONFIG;

typedef struct {
    double temperature;
    double humidity;
    bool isTemperatureValid;
    bool isHumidityValid;
} HTS221_DATA;

HTS221_CONFIG* hts221_initialize(int fd);
void hts221_uninitialize(HTS221_CONFIG* config);
int hts221_read(HTS221_CONFIG* config, HTS221_DATA* data);

#endif