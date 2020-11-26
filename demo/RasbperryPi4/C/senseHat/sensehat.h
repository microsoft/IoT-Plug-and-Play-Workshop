#pragma once
#ifndef _SENSE_HAT
#define _SENSE_HAT

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "hts221.h"
#include "lps25h.h"
#include "lsm9ds1.h"
#include "led.h"

typedef struct {
    I2C_DEVICE* i2c_dev;
    HTS221_CONFIG* hts221;
    LPS25H_CONFIG* lps25h;
#ifdef HAVE_RTIMULIB_H
    RTIMU_CONFIG* rtimu;
#endif
    LED_CONFIG* led;
} SENSEHAT_DATA;

SENSEHAT_DATA* initialize_senseHat(void);

#endif