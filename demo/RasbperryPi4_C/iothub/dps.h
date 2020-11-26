#ifndef _DPS
#define _DPS

#include "iothub_op.h"
#include "iothub_device_client_ll.h"
#include "azure_prov_client/iothub_security_factory.h"
#include "azure_prov_client/prov_device_client.h"
#include "azure_prov_client/prov_transport_mqtt_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_c_shared_utility/strings.h"

typedef struct DPS_CLIENT_CONTEXT_TAG
{
    unsigned int sleep_time;
    char* iothub_uri;
    char* access_key_name;
    char* device_key;
    char* device_id;
    bool registration_complete;
} DPS_CLIENT_CONTEXT;

IOTHUB_DEVICE_CLIENT_LL_HANDLE ProvisionDevice(const char* scopeId, const char* deviceId, const char* deviceKey, const char* modelId);

#endif /* _DPS */