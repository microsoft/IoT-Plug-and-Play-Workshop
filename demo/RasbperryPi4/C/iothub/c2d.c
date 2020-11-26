#include "c2d.h"

/* 
** Callback function to receive Cloud to Device Message
**
** To Do : Add user callbcck to process C2D message 
*/
IOTHUBMESSAGE_DISPOSITION_RESULT cloudMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;

    const char* buffer;
    size_t size;
    MAP_HANDLE mapProperties;
    const char* messageId;
    const char* correlationId;
    const char* userDefinedContentType;
    const char* userDefinedContentEncoding;

    LogInfo("%s() invoked : message", __func__);

    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<null>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<null>";
    }

    if ((userDefinedContentType = IoTHubMessage_GetContentTypeSystemProperty(message)) == NULL)
    {
        userDefinedContentType = "<null>";
    }

    if ((userDefinedContentEncoding = IoTHubMessage_GetContentEncodingSystemProperty(message)) == NULL)
    {
        userDefinedContentEncoding = "<null>";
    }

    if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        LogInfo("unable to retrieve the message data");
    }
    else
    {
        LogInfo("Received Message\r\n Message ID: %s\r\n Correlation ID: %s\r\n Content-Type: %s\r\n Content-Encoding: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", 
            messageId, correlationId, userDefinedContentType, userDefinedContentEncoding, (int)size, buffer, (int)size);
    }

    mapProperties = IoTHubMessage_Properties(message);

    if (mapProperties != NULL)
    {
        const char*const* keys;
        const char*const* values;
        size_t propertyCount = 0;
        if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK)
        {
            if (propertyCount > 0)
            {
                size_t index;

                LogInfo(" Message Properties:");
                for (index = 0; index < propertyCount; index++)
                {
                    LogInfo("\tKey: %s Value: %s", keys[index], values[index]);
                }
            }
        }
    }

    if (_c2dCallback_fn != NULL)
    {
        return _c2dCallback_fn(message, userContextCallback);
    }

    return IOTHUBMESSAGE_ACCEPTED;
}