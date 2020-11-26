#ifndef _IOTHUB_OP
#define _IOTHUB_OP

#include <stdio.h>
#include "main.h"

#include "dps.h"
#include "d2c.h"
#include <iothub.h>
#include <iothub_device_client_ll.h>
#include <iothub_client_options.h>
#include <azure_c_shared_utility/threadapi.h>
#include <azure_c_shared_utility/xlogging.h>
#include <iothubtransportmqtt.h>

#ifdef SET_TRUSTED_CERT_IN_SAMPLES
#include <azure_c_shared_utility/shared_util_options.h>
#include <certs.h>
#endif // SET_TRUSTED_CERT_IN_SAMPLES


static const char IOTHUB_CS[] = "IOTHUB_CS";
static const char DPS_SCOPEID[]   = "DPS_SCOPEID";
static const char DPS_REGISTRATIONID[] = "DPS_REGISTRATIONID=";
static const char DPS_DEVICEKEY[] = "DPS_DEVICEKEY";
static const char DPS_GLOBAL_ENDPOINT[] = "global.azure-devices-provisioning.net";

static bool g_hubClientTraceEnabled = false;

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubInitialize(APP_CONTEXT* appConext);

#endif // _IOTHUB_OP