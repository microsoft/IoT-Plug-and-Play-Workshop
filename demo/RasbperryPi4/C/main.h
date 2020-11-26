#ifndef _SENSE_HAT_MAIN
#define _SENSE_HAT_MAIN

#include <parson.h>
#include <iothub_device_client_ll.h>

typedef struct {
    void* deviceData;
    bool isConnected;
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient;
} APP_CONTEXT;

void deviceTwin_CB(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback);
IOTHUBMESSAGE_DISPOSITION_RESULT c2d_CB(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
int deviceMethod_CB(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);
int processTelemetry(APP_CONTEXT* appContext);
static bool BuildUtcTimeFromCurrentTime(char* utcTimeBuffer, size_t utcTimeBufferSize);

#endif