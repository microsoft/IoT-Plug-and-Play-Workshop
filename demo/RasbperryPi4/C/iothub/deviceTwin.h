#pragma once

#ifndef _IOTHUB_DEVICE_TWIN
#define _IOTHUB_DEVICE_TWIN

#include "iothub_op.h"

typedef void (*deviceTwin_callback_fn)(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback);

deviceTwin_callback_fn _deviceTwinCallback_fn;

void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback);
void updateReportedState(IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClientLL, char* buffer);

#endif // _IOTHUB_DEVICE_TWIN