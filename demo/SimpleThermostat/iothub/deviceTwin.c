// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

#include "deviceTwin.h"

/* 
** Callback function to receive Device Twin update notification
*/
void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient = (IOTHUB_DEVICE_CLIENT_LL_HANDLE)userContextCallback;
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;
    char* paylod_copy = (char*)malloc(size+1);
    JSON_Value* allJSON;
    char* pretty;

    if (paylod_copy == NULL)
    {
        LogError("failed to allocate json buffer");
    }
    else{
        (void)memcpy(paylod_copy, payload, size);
        paylod_copy[size] = '\0';

        allJSON = json_parse_string(paylod_copy);

        if (allJSON == NULL)
        {
            LogError("filed to parse json");
        }
        else 
        {
            pretty = json_serialize_to_string_pretty(allJSON);

            LogInfo("%s() : Update Status %s Paload \r\n%s", __func__, MU_ENUM_TO_STRING(DEVICE_TWIN_UPDATE_STATE, updateState), pretty);

            if (_deviceTwinCallback_fn != NULL)
            {
                _deviceTwinCallback_fn(updateState, payload, size, userContextCallback);
            }
            json_value_free(allJSON);
        }

        free(paylod_copy);
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
