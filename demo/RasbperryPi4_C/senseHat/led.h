#pragma once
#ifndef _SENSE_HAT_LED
#define _SENSE_HAT_LED

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include "util.h"

#define LED_I2C_ID   0x46;

typedef struct {
    bool isInitialized;
    int fd;
    FB_DEVICE* fb_dev;
} LED_CONFIG;

// typedef struct {
//     uint8_t asciiCHar;
//     uint8_t asciiData[0];
// } ASCII_DATA;

// typedef struct {
//     ASCII_DATA data[95];
// } ASCII_MAP;

EXTERNC LED_CONFIG* led_initialize(void);
EXTERNC void led_uninitialize(LED_CONFIG* config);
EXTERNC int led_clear(LED_CONFIG* config);
EXTERNC int led_set(LED_CONFIG* config, int x, int y, uint8_t r, uint8_t g, uint8_t b);
EXTERNC int led_putText(LED_CONFIG* config, const char *strText);
EXTERNC int led_fill(LED_CONFIG* config, uint8_t r, uint8_t g, uint8_t b);
#undef EXTERNC
#endif
