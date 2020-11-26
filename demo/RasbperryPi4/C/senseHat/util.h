#ifndef _SENSE_HAT_UTIL
#define _SENSE_HAT_UTIL

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
// #include <linux/i2c.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

static const char SENSE_HAT_FB_NAME[] = "RPi-Sense FB";

typedef struct {
    int fd;
    char name[64];
} I2C_DEVICE;

typedef struct {
    int fd;
    char name[128];
} FB_DEVICE;

typedef struct {
    bool isValid;
    double x;
    double y;
    double z;
} ORIANTATION_DATA;

typedef struct {
    bool isValid;
    double pitch;
    double roll;
    double yaw;
} FUSION_DATA;

EXTERNC FB_DEVICE* scan_fb_devices();
I2C_DEVICE* scan_i2c_devices();

int search_i2c_device(int fd, unsigned char who_am_i, unsigned char i2c_id);

int read_i2c(int fd, 
             unsigned char i2c_addr,
             unsigned char regAddr,
             unsigned char length,
             unsigned char *buffer);

int write_i2c(int fd, 
              unsigned char i2c_addr,
              unsigned char regAddr,
              __u8          value);

void delay_ms(long ms);


int64_t seload(uint64_t measured, size_t size);
#endif // _SENSE_HAT_UTIL