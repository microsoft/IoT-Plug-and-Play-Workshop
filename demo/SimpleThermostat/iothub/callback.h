// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//
#ifndef _IOTHUB_CALLBACK
#define _IOTHUB_CALLBACK

#include <stdio.h>
#include "deviceTwin.h"
#include "deviceMethod.h"

typedef enum
{
    CB_DEVICE_TWIN,
    CB_DEVICE_METHOD
} CALLBACK_ENUM;


typedef int (*iothub_callback_fn)(const unsigned char* payload, size_t size, void* userContextCallback);
int set_callback(CALLBACK_ENUM cb, void* fn, void* appContext);

#endif