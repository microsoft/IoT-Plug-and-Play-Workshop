#include "deviceMethod.h"


// Success = 200;
// BadFormat = 400;
// NotFoundStatus = 404;
// InternalError = 500;

/* 
** Callback function to device Device Method (or Direct Methods)
**
** To Do : Add user callback to process Device Method
*/
int deviceMethodCallback(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient = (IOTHUB_DEVICE_CLIENT_LL_HANDLE)userContextCallback;
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;

    const char* RESPONSE_STRING = "{ \"Response\": \"Success\" }";
    int result = 200;

    LogInfo("%s() invoked : Method %s Paload %s", __func__, methodName, payload);

    if (_deviceMethodCallback_fn != NULL)
    {
        result = _deviceMethodCallback_fn(methodName, payload, size, response, responseSize, userContextCallback);
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

    return result;
}
