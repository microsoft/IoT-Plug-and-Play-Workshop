// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

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

#define DEBUG_TELEMETRY
static const char IOTHUB_CS[] = "IOTHUB_CS";
static const char DPS_IDSCOPE[]   = "DPS_IDSCOPE";
static const char DPS_X509[]   = "DPS_X509";
static const char DPS_DEVICE_ID[] = "DPS_DEVICE_ID";
static const char DPS_SYMMETRIC_KEY[] = "DPS_SYMMETRIC_KEY";
static const char DPS_GLOBAL_ENDPOINT[] = "global.azure-devices-provisioning.net";
static const char IOTPNP_MODEL_ID[] = "PNP_MODEL_ID";

static bool g_hubClientTraceEnabled = false;

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubInitialize(APP_CONTEXT* appConext);

#endif // _IOTHUB_OP