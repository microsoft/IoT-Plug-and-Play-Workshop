#ifndef _IOTHUB_CALLBACK
#define _IOTHUB_CALLBACK

#include <stdio.h>
#include "deviceTwin.h"
#include "deviceMethod.h"
#include "c2d.h"

typedef enum
{
    CB_DEVICE_TWIN,
    CB_DEVICE_METHOD,
    CB_C2D_MESSAGE
} CALLBACK_ENUM;


typedef int (*iothub_callback_fn)(const unsigned char* payload, size_t size, void* userContextCallback);

// iothub_callback_fn g_callbacks[2] = {NULL};

int set_callback(CALLBACK_ENUM cb, void* fn, void* appContext);

#endif