// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

#include "deviceMethod.h"

/* 
** Callback function to device Device Method (or Direct Methods)
**
** To Do : Add user callback to process Device Method
*/
int deviceMethodCallback(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient = (IOTHUB_DEVICE_CLIENT_LL_HANDLE)userContextCallback;
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;
    char* paylod_copy = (char*)malloc(size+1);
    JSON_Value* allJSON;
    char* pretty;

    const char* RESPONSE_STRING = "{ \"Response\": \"Success\" }";
    int result = 200;

    if (paylod_copy == NULL)
    {
        LogError("failed to allocate json buffer");
        result = 500;
    }
    else{
        (void)memcpy(paylod_copy, payload, size);
        paylod_copy[size] = '\0';

        allJSON = json_parse_string(paylod_copy);

        if (allJSON == NULL)
        {
            LogError("filed to parse json");
            result = 500;
        }
        else 
        {
            pretty = json_serialize_to_string_pretty(allJSON);

            LogInfo("%s() : Method %s Paload : %s", __func__, methodName, pretty);

            if (_deviceMethodCallback_fn != NULL)
            {
                result = _deviceMethodCallback_fn(methodName, paylod_copy, size, response, responseSize, userContextCallback);
            }
            else if (*response == NULL)
            {
                *responseSize = strlen(RESPONSE_STRING);

                if ((*response = malloc(*responseSize)) == NULL)
                {
                    result = 500;
                }
                else
                {
                    memcpy(*response, RESPONSE_STRING, *responseSize);
                }
            }

            if (pretty != NULL)
            {
                json_free_serialized_string(pretty);
            }
            json_value_free(allJSON);
        }

        free(paylod_copy);
    }

    LogInfo("Method : %s return : %d", methodName, result);
    return result;
}
