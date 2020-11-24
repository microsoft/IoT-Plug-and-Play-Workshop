// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

#include "dps.h"

static const int g_dpsRegistrationMaxPolls = 60;
static const int g_dpsRegistrationPollSleep = 1000;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);

DPS_CLIENT_CONTEXT dpsContext;
bool g_provisioned = false;

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

            LogInfo("Registration Information received from DPS");

            if (user_ctx->iothub_uri != NULL)
            {
                free(user_ctx->iothub_uri);
            }

            if (mallocAndStrcpy_s(&user_ctx->iothub_uri, iothub_uri) != 0)
            {
                LogError("Failed to copy IoT Hub Uri");
            }
            else{
                LogInfo("IoT Hub     : %s", user_ctx->iothub_uri);
            }

            if (user_ctx->device_id != NULL)
            {
                free(user_ctx->device_id);
            }

            if (mallocAndStrcpy_s(&user_ctx->device_id, device_id) != 0)
            {
                LogError("Failed to copy Device Id");
            }
            else{
                LogInfo("Device Id   : %s", user_ctx->device_id);
            }

            user_ctx->registration_complete = 1;
        }
        else
        {
            LogError("Failure encountered on registration %s", MU_ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result) );
            user_ctx->registration_complete = 0;
        }
    }
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE ProvisionDeviceX509(const char* scopeId, const char* modelId, APP_CONTEXT* appConext)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceHandle = NULL;
    PROV_DEVICE_LL_HANDLE provDeviceHandle = NULL;
    STRING_HANDLE modelIdPayload = NULL;
    PROV_DEVICE_RESULT provDeviceResult;

    printf("==================================================\r\nStart device provisioning with X.509\r\n==================================================\r\n");
    if (g_provisioned == false)
    {
        memset(&dpsContext, 0, sizeof(DPS_CLIENT_CONTEXT));
        dpsContext.registration_complete = false;
        dpsContext.sleep_time = 100;
    }

    if (dpsContext.registration_complete == false)
    {
        if (modelId != NULL)
        {
            if ((modelIdPayload = STRING_construct_sprintf("{\"modelId\":\"%s\"}", modelId)) == NULL)
            {
                LogError("Cannot allocate DPS payload for modelId.");
                return NULL;
            }
        }

        LogInfo("%s\r\nID Scope   : %s\r\nModel ID   : %s", __func__, scopeId, modelId);

        if (prov_dev_security_init(SECURE_DEVICE_TYPE_X509) != 0)
        {
            LogError("prov_dev_security_init failed");
        }
        else if ((provDeviceHandle = Prov_Device_LL_Create(DPS_GLOBAL_ENDPOINT, scopeId, Prov_Device_MQTT_Protocol)) == NULL)
        {
            LogError("failed calling Prov_Device_Create");
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

    }

    if (dpsContext.registration_complete == 1)
    {
        g_provisioned = true;

        LogInfo("Connecting to IoT Hub");

        if (iothub_security_init(IOTHUB_SECURITY_TYPE_X509) != 0)
        {
            LogError("iothub_security_init failed");
        }
        else if ((deviceHandle = IoTHubDeviceClient_LL_CreateFromDeviceAuth(dpsContext.iothub_uri, dpsContext.device_id, MQTT_Protocol)) == NULL)
        {
            LogError("IoTHubDeviceClient_LL_CreateFromDeviceAuth failed");
        }
        else{
            LogInfo("IoTHubDeviceClient_LL_CreateFromDeviceAuth Success");
            if (appConext->iothub_uri != NULL)
            {
                free(appConext->iothub_uri);
            }
            (void)mallocAndStrcpy_s(&appConext->iothub_uri, dpsContext.iothub_uri);
        }
    }

    if (modelIdPayload != NULL)
    {
        STRING_delete(modelIdPayload);
    }

    return deviceHandle;
}


IOTHUB_DEVICE_CLIENT_LL_HANDLE ProvisionDevice(const char* scopeId, const char* deviceId, const char* deviceKey, const char* modelId, APP_CONTEXT* appConext)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceHandle = NULL;
    PROV_DEVICE_LL_HANDLE provDeviceHandle = NULL;
    STRING_HANDLE modelIdPayload = NULL;
    PROV_DEVICE_RESULT provDeviceResult;
    DPS_CLIENT_CONTEXT dpsContext;

    memset(&dpsContext, 0, sizeof(DPS_CLIENT_CONTEXT));
    dpsContext.registration_complete = false;
    dpsContext.sleep_time = 100;

    printf("==================================================\r\nStart device provisioning with symmetric key\r\n==================================================\r\n");

    if (modelId != NULL)
    {
        if ((modelIdPayload = STRING_construct_sprintf("{\"modelId\":\"%s\"}", modelId)) == NULL)
        {
            LogError("Cannot allocate DPS payload for modelId.");
            return NULL;
        }
    }

    LogInfo("Provisioning Data\r\nID Scope   : %s\r\nDevice ID  : %s\r\nDevice Key : %s\r\nModel ID   : %s", scopeId, deviceId, deviceKey, modelId);

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

        printf("Provisioning completed\r\n");
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
            if (appConext->iothub_uri != NULL)
            {
                free(appConext->iothub_uri);
            }
            (void)mallocAndStrcpy_s(&appConext->iothub_uri, dpsContext.iothub_uri);
        }
    }

    if (modelIdPayload != NULL)
    {
        STRING_delete(modelIdPayload);
    }

    return deviceHandle;
}