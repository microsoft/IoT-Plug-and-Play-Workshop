#include "dps.h"

static const int g_dpsRegistrationMaxPolls = 60;
static const int g_dpsRegistrationPollSleep = 1000;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);

static void dpsRegistrationStatusCallback(PROV_DEVICE_REG_STATUS register_result, void* user_context)
{
    LogInfo("Provisioning Status: %s", MU_ENUM_TO_STRING(PROV_DEVICE_REG_STATUS, register_result));
}

static void dpsRegisterDeviceCallback(PROV_DEVICE_RESULT register_result, const char* iothub_uri, const char* device_id, void* user_context)
{
    if (user_context == NULL)
    {
        LogInfo("user_context is NULL");
    }
    else
    {
        DPS_CLIENT_CONTEXT* user_ctx = (DPS_CLIENT_CONTEXT*)user_context;
        if (register_result == PROV_DEVICE_RESULT_OK)
        {
            LogInfo("Registration Information received from service: %s!", iothub_uri);
            (void)mallocAndStrcpy_s(&user_ctx->iothub_uri, iothub_uri);
            (void)mallocAndStrcpy_s(&user_ctx->device_id, device_id);
            LogInfo("IoT Hub   %s", user_ctx->iothub_uri);
            LogInfo("Device Id %s", user_ctx->device_id);

            user_ctx->registration_complete = 1;
        }
        else
        {
            LogError("Failure encountered on registration %s", MU_ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result) );
            user_ctx->registration_complete = 0;
        }
    }
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE ProvisionDevice(const char* scopeId, const char* deviceId, const char* deviceKey, const char* modelId)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceHandle = NULL;
    PROV_DEVICE_LL_HANDLE provDeviceHandle = NULL;
    STRING_HANDLE modelIdPayload = NULL;
    PROV_DEVICE_RESULT provDeviceResult;
    DPS_CLIENT_CONTEXT dpsContext;

    memset(&dpsContext, 0, sizeof(DPS_CLIENT_CONTEXT));
    dpsContext.registration_complete = false;
    dpsContext.sleep_time = 100;

    LogInfo("%s ScopeID %s Device ID %s Device Key %s ModelId %s", __func__, scopeId, deviceId, deviceKey, modelIdPayload);

    if (modelId != NULL)
    {
        if ((modelIdPayload = STRING_construct_sprintf("{\"modelId\":\"%s\"}", modelId)) == NULL)
        {
            LogError("Cannot allocate DPS payload for modelId.");
            return NULL;
        }
    }

    if ((prov_dev_set_symmetric_key_info(deviceId, deviceKey) != 0))
    {
        LogError("prov_dev_set_symmetric_key_info failed.");
    }
    else if (prov_dev_security_init(SECURE_DEVICE_TYPE_SYMMETRIC_KEY) != 0)
    {
        LogError("prov_dev_security_init failed");
    }
    else if ((provDeviceHandle = Prov_Device_LL_Create(DPS_GLOBAL_ENDPOINT, scopeId, Prov_Device_MQTT_Protocol)) == NULL)
    {
        LogError("Failed calling Prov_Device_LL_Create");
    }
    else if ((provDeviceResult = Prov_Device_LL_SetOption(provDeviceHandle, PROV_OPTION_LOG_TRACE, &g_hubClientTraceEnabled)) != PROV_DEVICE_RESULT_OK)
    {
        LogError("Setting provisioning tracing on failed, error=%d", provDeviceResult);
    }
    else if (modelIdPayload != NULL && ((provDeviceResult = Prov_Device_LL_Set_Provisioning_Payload(provDeviceHandle, STRING_c_str(modelIdPayload))) != PROV_DEVICE_RESULT_OK))
    {
        LogError("Failed setting provisioning data, error=%d", provDeviceResult);
    }
    else if ((provDeviceResult = Prov_Device_LL_Register_Device(provDeviceHandle, dpsRegisterDeviceCallback, &dpsContext, dpsRegistrationStatusCallback, &dpsContext)) != PROV_DEVICE_RESULT_OK)
    {
        LogError("Prov_Device_LL_Register_Device failed, error=%d", provDeviceResult);
    }
    else
    {
        do
        {
            Prov_Device_LL_DoWork(provDeviceHandle);
            ThreadAPI_Sleep(dpsContext.sleep_time);
        }
        while (dpsContext.registration_complete == 0);
    }

    if (provDeviceHandle != NULL)
    {
        Prov_Device_LL_Destroy(provDeviceHandle);
    }

    if (dpsContext.registration_complete == 1)
    {
        if (iothub_security_init(IOTHUB_SECURITY_TYPE_SYMMETRIC_KEY) != 0)
        {
            LogError("iothub_security_init failed");
        }
        else if ((deviceHandle = IoTHubDeviceClient_LL_CreateFromDeviceAuth(dpsContext.iothub_uri, dpsContext.device_id, MQTT_Protocol)) == NULL)
        {
            LogError("IoTHubDeviceClient_LL_CreateFromDeviceAuth failed");
        }
        else{
            LogInfo("IoTHubDeviceClient_LL_CreateFromDeviceAuth Success");
        }
    }

    if (modelIdPayload != NULL)
    {
        STRING_delete(modelIdPayload);
    }

    return deviceHandle;
}