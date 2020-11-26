#include "iothub_op.h"
#include "deviceTwin.h"
#include "deviceMethod.h"
#include "c2d.h"

#define PNP_ENABLE

//static const char g_pnp_model_id[] = "dtmi:iotpnpadt:DigitalTwins;SenseHat;1";
const char* g_pnp_model_id = NULL;

/*
** Receives callback when IoT Hub connection status change
**
** To Do : Add DPS support
*/
static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS isConfigured, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;

    (void)reason;

    if (isConfigured == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        LogInfo("The device client is connected to iothub");
        appContext->isConnected = true;
    }
    else
    {
        LogInfo("The device client has been disconnected");
        appContext->isConnected = false;
    }
}

/*
** Opens handle using Connection String
**
** To Do : Add DPS support
*/
static IOTHUB_DEVICE_CLIENT_LL_HANDLE CreateDeviceClientLLHandle(void)
{
    const char* iothubCs;
    const char* scopeId;
    const char* deviceId;
    const char* deviceKey;
    
    g_pnp_model_id = getenv("PNP_MODEL_ID");

    if ((iothubCs = getenv(IOTHUB_CS)) != NULL)
    {
        return IoTHubDeviceClient_LL_CreateFromConnectionString(iothubCs, MQTT_Protocol);
    }
    else if ((scopeId = getenv("DPS_SCOPEID")) == NULL)
    {
        LogError("Cannot read environment variable=%s", DPS_SCOPEID);
        return NULL;
    }
    else if ((deviceId = getenv("DPS_REGISTRATIONID")) == NULL)
    {
        LogError("Cannot read environment variable=%s", DPS_REGISTRATIONID);
        return NULL;
    }
    else if ((deviceKey = getenv("DPS_DEVICEKEY")) == NULL)
    {
        LogError("Cannot read environment variable=%s", DPS_DEVICEKEY);
        return NULL;
    }
    else
    {
        LogInfo("Provisioning Start ===>");
        return ProvisionDevice(scopeId, deviceId, deviceKey, g_pnp_model_id);
        LogInfo("Provisioning End   <===");
    } 

    return NULL;
}

/*
** Initialize IoT Hub connection
**
** To Do : Add DPS support
*/
IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubInitialize(APP_CONTEXT* appConext)
{
    int iothubInitResult;
    bool isConfigured;
    IOTHUB_CLIENT_RESULT iothubResult;
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceHandle = NULL;
    bool urlAutoEncodeDecode = true;

    LogInfo("%s()", __func__);

    if (appConext == NULL)
    {
        LogError("AppContext is NULL");
    }

    if ((iothubInitResult = IoTHub_Init()) != 0)
    {
        LogError("Failure to initialize client.  Error=%d", iothubInitResult);
        isConfigured = false;
    }
    else if ((deviceHandle = CreateDeviceClientLLHandle()) == NULL)
    {
        LogError("Failure creating IotHub client.  Hint: Check your connection string or DPS configuration");
        isConfigured = false;
    }
    else if ((iothubResult = IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_LOG_TRACE, &g_hubClientTraceEnabled)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set logging option, error=%d", iothubResult);
        isConfigured = false;
    }
#ifdef PNP_ENABLE
    else if ((g_pnp_model_id) && ((iothubResult = IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_MODEL_ID, g_pnp_model_id)) != IOTHUB_CLIENT_OK))
    {
        LogError("Unable to set model ID, error=%d", iothubResult);
        isConfigured = false;
    }
#endif
    else if ((iothubResult = IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_AUTO_URL_ENCODE_DECODE, &urlAutoEncodeDecode)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set auto Url encode option, error=%d", iothubResult);
        isConfigured = false;
    }
    else if ((iothubResult = IoTHubDeviceClient_LL_SetConnectionStatusCallback(deviceHandle, connection_status_callback, (void*)appConext)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set Connection Status callback, error=%d", iothubResult);
        isConfigured = false;
    }
    else if ((iothubResult = IoTHubDeviceClient_LL_SetDeviceMethodCallback(deviceHandle, deviceMethodCallback, (void*)appConext)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set device method callback, error=%d", iothubResult);
        isConfigured = false;
    }
    else if ((iothubResult = IoTHubDeviceClient_LL_SetDeviceTwinCallback(deviceHandle, deviceTwinCallback, (void*)appConext)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set Device Twin callback, error=%d", iothubResult);
        isConfigured = false;
    }
    else if ((iothubResult = IoTHubDeviceClient_LL_SetMessageCallback(deviceHandle, cloudMessageCallback, (void*)appConext)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set Message callback, error=%d", iothubResult);
        isConfigured = false;
    }
#ifdef SET_TRUSTED_CERT_IN_SAMPLES
    // Setting the Trusted Certificate.  This is only necessary on systems without built in certificate stores.
    else if ((iothubResult = IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_TRUSTED_CERT, certificates)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to set the trusted cert, error=%d", iothubResult);
        isConfigured = false;
    }
#endif // SET_TRUSTED_CERT_IN_SAMPLES
    else
    {
        isConfigured = true;
    }

    if ((isConfigured == false) && (deviceHandle != NULL))
    {
        IoTHubDeviceClient_LL_Destroy(deviceHandle);
        deviceHandle = NULL;
    }

    if ((isConfigured == false) &&  (iothubInitResult == 0))
    {
        IoTHub_Deinit();
    }

    return deviceHandle;
}