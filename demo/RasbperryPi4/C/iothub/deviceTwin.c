#include "deviceTwin.h"

/* 
** Callback function to receive Device Twin update notification
*/
void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient = (IOTHUB_DEVICE_CLIENT_LL_HANDLE)userContextCallback;
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;

    LogInfo("%s() invoked : Update Status %s Paload %s", __func__, MU_ENUM_TO_STRING(DEVICE_TWIN_UPDATE_STATE, updateState), payload);

    if (_deviceTwinCallback_fn != NULL)
    {
        _deviceTwinCallback_fn(updateState, payload, size, userContextCallback);
    }
}

static void ReportStatusCallback(int result, void *context)
{
    LogInfo("Device Twin reported properties update result: Status code %d", result);
}

void updateReportedState(IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClientLL, char* buffer)
{    
    IOTHUB_CLIENT_RESULT iothubClientResult;

    if ((iothubClientResult = IoTHubDeviceClient_LL_SendReportedState(deviceClientLL, (const unsigned char*)buffer, strlen(buffer), ReportStatusCallback, NULL)) != IOTHUB_CLIENT_OK)
    {
        LogError("Unable to send reported state.  Error=%d", iothubClientResult);
    }
    else
    {
        LogInfo("Sent reported property : %s", buffer);
    }
}
