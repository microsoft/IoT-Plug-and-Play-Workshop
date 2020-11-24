// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

#include "iothub_op.h"
#include "deviceTwin.h"
#include "deviceMethod.h"

#define PNP_ENABLE

//static const char g_pnp_model_id[] = "dtmi:iotpnpadt:DigitalTwins;SenseHat;1";
const char* g_pnp_model_id = NULL;

/*
** Receives callback when IoT Hub connection status change
**
*/
static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS connectionStatus, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;

    if (connectionStatus == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        LogInfo("Connected to iothub : %s", appContext->iothub_uri);
        appContext->isConnected = true;
    }
    else
    {
        LogError("The device client has been disconnected : Connection Status %s : Reason %s", 
                    MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS, connectionStatus),
                    MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));
        appContext->isConnected = false;
        IoTHub_Deinit();
    }
}

/*
** Opens handle using Connection String
**
** To Do : Add DPS support
*/
static IOTHUB_DEVICE_CLIENT_LL_HANDLE CreateDeviceClientLLHandle(APP_CONTEXT* appConext)
{
    const char* iothubCs;
    const char* scopeId;
    const char* deviceId;
    const char* deviceKey;
    const char* x509 = NULL;
    
    g_pnp_model_id = getenv(IOTPNP_MODEL_ID);

    if ((iothubCs = getenv(IOTHUB_CS)) != NULL)
    {
        return IoTHubDeviceClient_LL_CreateFromConnectionString(iothubCs, MQTT_Protocol);
    }
    else
    {
        if ((scopeId = getenv(DPS_IDSCOPE)) == NULL)
        {
            LogError("Cannot read environment variable=%s", DPS_IDSCOPE);
            return NULL;
        }
        else if ((x509 = getenv(DPS_X509)) != NULL)
        {
            return ProvisionDeviceX509(scopeId, g_pnp_model_id, appConext);
        }
        else
        {
            if ((deviceId = getenv(DPS_DEVICE_ID)) == NULL)
            {
                LogError("Cannot read environment variable=%s", DPS_DEVICE_ID);
                return NULL;
            }
            else if ((deviceKey = getenv(DPS_SYMMETRIC_KEY)) == NULL)
            {
                LogError("Cannot read environment variable=%s", DPS_SYMMETRIC_KEY);
                return NULL;
            }
            else
            {
                return ProvisionDevice(scopeId, deviceId, deviceKey, g_pnp_model_id, appConext);
            } 
        }
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
    else if ((deviceHandle = CreateDeviceClientLLHandle(appConext)) == NULL)
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