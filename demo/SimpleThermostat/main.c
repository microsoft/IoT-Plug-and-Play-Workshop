// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//
#include "main.h"
#include "iothub/callback.h"
#include "iothub/iothub_op.h"

#define DEFAULT_TEMPERATURE_VALUE 22

static double g_currentTemperature = DEFAULT_TEMPERATURE_VALUE;
static double g_minTemperature = DEFAULT_TEMPERATURE_VALUE;
static double g_maxTemperature = DEFAULT_TEMPERATURE_VALUE;
static double g_allTemperatures = DEFAULT_TEMPERATURE_VALUE;
static int g_numTemperatureUpdates = 1;
char g_ProgramStartTime[128];

int main(int argc, char *argv[])
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClientLL = NULL;
    char msgBuffer[1024];
    APP_CONTEXT* appContext;

    printf("Starting Simple Thermostat IoT Hub Device App\r\n");

    appContext = malloc(sizeof(APP_CONTEXT));

    if (appContext == NULL)
    {
        printf("Failed to allocate app context\r\n");
        return -1;
    }

    BuildUtcTimeFromCurrentTime(g_ProgramStartTime, sizeof(g_ProgramStartTime));
    appContext->isConnected = false;

    while (true)
    {
        if (appContext->isConnected != true)
        {
            if (deviceClientLL == NULL)
            {
                if ((appContext->deviceClient = IoTHubInitialize(appContext)) == NULL)
                {
                    LogError("Failed to initialize IoT Hub connection.  Retrying in 5 seconds");
                    ThreadAPI_Sleep(5000);
                    continue;
                }
                else
                {
                    set_callback(CB_DEVICE_TWIN, deviceTwin_CB, appContext);
                    set_callback(CB_DEVICE_METHOD, deviceMethod_CB, appContext);
                }
            }

            // wait for connection up to 10 sec
            int retry = 100;
            while (appContext->isConnected != true && retry > 0)
            {
                ThreadAPI_Sleep(100);
                IoTHubDeviceClient_LL_DoWork(appContext->deviceClient);
            }

            if (appContext->isConnected != true)
            {
                LogError("IoT Hub connection not established after 10 seconds");
            }
        }

        if (appContext->isConnected == true)
        {
            // process periodic tasks
            // e.g. sending telemetry
            processTelemetry(appContext);
            ThreadAPI_Sleep(1000);
        }
        else
        {
            LogInfo("Not Connected");
        }

        IoTHubDeviceClient_LL_DoWork(appContext->deviceClient);
    }

    if (appContext != NULL)
    {
        free(appContext);
    }

    return 0;
}

//
// From PnP Simple Thermostat sample
//
static void UpdateTemperatureAndStatistics(double desiredTemp, bool* maxTempUpdated)
{
    if (desiredTemp > g_maxTemperature)
    {
        g_maxTemperature = desiredTemp;
        *maxTempUpdated = true;
    }
    else if (desiredTemp < g_minTemperature)
    {
        g_minTemperature = desiredTemp;
    }

    g_numTemperatureUpdates++;
    g_allTemperatures += desiredTemp;

    g_currentTemperature = desiredTemp;
}

/*
** Callback on Device Twin changes
*/
void deviceTwin_CB(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback)
{
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;
    JSON_Value* root_Value = NULL;
    JSON_Object* root_Object = NULL;
    JSON_Object* desired_Object = NULL;
    JSON_Value* version_Value = NULL;
    int version;
    size_t i;
    IOTHUB_CLIENT_RESULT result;
    bool maxTempUpdated = updateState == DEVICE_TWIN_UPDATE_COMPLETE?true:false;
    char targetTemperatureResponseProperty[256];
    char maxTemperatureSinceRebootProperty[256];

    if ((root_Value = json_parse_string(payload)) == NULL)
    {
        LogError("Unable to parse Device Twin payload");
    }
    else if ((root_Object = json_value_get_object(root_Value)) == NULL)
    {
        LogError("Unable to get root object of JSON");
    }
    else
    {
        if (updateState == DEVICE_TWIN_UPDATE_COMPLETE)
        {
            desired_Object = json_object_get_object(root_Object, "desired");
        }
        else
        {
            desired_Object = root_Object;
        }
    }

    if (desired_Object == NULL)
    {
       LogError("Could not find Desired Property");
    }
    else if ((version_Value = json_object_get_value(desired_Object, "$version")) == NULL)
    {
        LogError("Could not find $version");
    }
    else if (json_value_get_type(version_Value) != JSONNumber)
    {
        LogError("$version is not a number");
    }
    else
    {
        int node_count = json_object_get_count(desired_Object);
        version = json_value_get_number(version_Value);

        for (i = 0; i < node_count; i++)
        {
            const char* node_Name = json_object_get_name(desired_Object, i);
            JSON_Value* desired_Value = json_object_get_value_at(desired_Object, i);

            if (strcmp(node_Name, "targetTemperature") == 0)
            {
                if ((json_type(desired_Value) == JSONNumber) == false)
                {
                    LogError("targetTemperature is not a number");
                }
                else{
                    double targetTemperature = json_value_get_number(desired_Value);

                    LogInfo("Received targetTemperature = %f", targetTemperature);

                    UpdateTemperatureAndStatistics(targetTemperature, &maxTempUpdated);

                    if (snprintf(targetTemperatureResponseProperty, 
                                 sizeof(targetTemperatureResponseProperty), 
                                 "{\"targetTemperature\":{\"value\":%.2f,\"ac\":%d,\"av\":%d,\"ad\":\"%s\"}}",
                                 targetTemperature,
                                 200,
                                 version,
                                 "Success") < 0)
                    {
                        LogError("snprintf building targetTemperature response failed");
                    }
                    else if ((result = IoTHubDeviceClient_LL_SendReportedState(appContext->deviceClient,
                                                                               (const unsigned char*)targetTemperatureResponseProperty,
                                                                               strlen(targetTemperatureResponseProperty),
                                                                               NULL,
                                                                               NULL)) != IOTHUB_CLIENT_OK)
                    {
                        LogError("Unable to send reported state for target temperature.  Error=%d", result);
                    }
                    else
                    {
                        LogInfo("Sending target temperature property");
                    }
                }
            }
        }
    }

    if (maxTempUpdated)
    {
        if (snprintf(maxTemperatureSinceRebootProperty,
                        sizeof(maxTemperatureSinceRebootProperty),
                        "{\"maxTempSinceLastReboot\":%.2f}",
                        g_maxTemperature) < 0)
        {
            LogError("snprintf building maxTemperature failed");
        }
        else if ((result = IoTHubDeviceClient_LL_SendReportedState(appContext->deviceClient,
                                                                    (const unsigned char*)maxTemperatureSinceRebootProperty,
                                                                    strlen(maxTemperatureSinceRebootProperty),
                                                                    NULL,
                                                                    NULL)) != IOTHUB_CLIENT_OK)
        {
            LogError("Unable to send reported state for maximum temperature.  Error=%d", result);
        }
        else
        {
            LogInfo("Sending maxTempSinceReboot property");
        }
    }
}

static const char g_maxMinCommandResponseFormat[] = "{\"maxTemp\":%.2f,\"minTemp\":%.2f,\"avgTemp\":%.2f,\"startTime\":\"%s\",\"endTime\":\"%s\"}";

static bool BuildMaxMinCommandResponse(unsigned char** response, size_t* responseSize)
{
    int responseBuilderSize = 0;
    unsigned char* responseBuilder = NULL;
    bool result;
    char currentTime[128];

    if (BuildUtcTimeFromCurrentTime(currentTime, sizeof(currentTime)) == false)
    {
        LogError("Unable to output the current time");
        result = false;
    }
    // Get the size of buffer for the response data
    else if ((responseBuilderSize = snprintf(NULL, 0, g_maxMinCommandResponseFormat, g_maxTemperature, g_minTemperature, g_allTemperatures / g_numTemperatureUpdates, g_ProgramStartTime, currentTime)) < 0)
    {
        LogError("snprintf to determine string length for command response failed");
        result = false;
    }
    // We MUST allocate the response buffer.  It is returned to the IoTHub SDK in the direct method callback and the SDK in turn sends this to the server.
    else if ((responseBuilder = calloc(1, responseBuilderSize + 1)) == NULL)
    {
        LogError("Unable to allocate %lu bytes", (unsigned long)(responseBuilderSize + 1));
        result = false;
    }
    // Build response data
    else if ((responseBuilderSize = snprintf((char*)responseBuilder, responseBuilderSize + 1, g_maxMinCommandResponseFormat, g_maxTemperature, g_minTemperature, g_allTemperatures / g_numTemperatureUpdates, g_ProgramStartTime, currentTime)) < 0)
    {
        LogError("snprintf to output buffer for command response");
        result = false;
    }
    else
    {
        result = true;
    }

    if (result == true)
    {
        JSON_Value *json_root = json_parse_string(responseBuilder);

        if (json_root == NULL)
        {
            printf("json root null\r\n");
        }
        else{
            char* pretty = json_serialize_to_string_pretty(json_root);
            *response = responseBuilder;
            *responseSize = (size_t)responseBuilderSize;
            LogInfo("Response\r\n%s", pretty);

            if (pretty != NULL)
            {
                json_free_serialized_string(pretty);
            }
        }
    }
    else
    {
        free(responseBuilder);
    }

    return result;
}       

/*
** Callback on Device Method
*/
int deviceMethod_CB(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback)
{
    LogInfo("%s %s Payload : %s", __func__, methodName, payload);
    APP_CONTEXT* appContext = (APP_CONTEXT* )userContextCallback;
    char jsonStr[33];
    JSON_Value* rootValue = NULL;
    const char* timeSince;
    int result = 200;

    memcpy(jsonStr, payload, size);
    jsonStr[size] = '\0';

    if (strcmp(methodName, "getMaxMinReport") != 0)
    {
        LogError("Method name %s is not supported on this component", methodName);
        const char deviceMethodResponse[] = "{ \"Response\": \"Not Supported\" }";
        *responseSize = sizeof(deviceMethodResponse)-1;
        *response = malloc(*responseSize);
        (void)memcpy(*response, deviceMethodResponse, *responseSize);
        result = 405;
    }
    else if ((rootValue = json_parse_string(jsonStr)) == NULL)
    {
        LogError("Unable to parse twin JSON");
        const char deviceMethodResponse[] = "{ \"Response\": \"Invalid JSON\" }";
        *responseSize = sizeof(deviceMethodResponse)-1;
        *response = malloc(*responseSize);
        (void)memcpy(*response, deviceMethodResponse, *responseSize);
        result = 400;
    }
    else if ((timeSince = json_value_get_string(rootValue)) == NULL)
    {
        LogError("Cannot retrieve JSON string for command");
        const char deviceMethodResponse[] = "{ \"Response\": \"Invalid JSON\" }";
        *responseSize = sizeof(deviceMethodResponse)-1;
        *response = malloc(*responseSize);
        (void)memcpy(*response, deviceMethodResponse, *responseSize);
        result = 400;
    }
    else{
        BuildMaxMinCommandResponse(response, responseSize);
    }

    if (rootValue)
    {
        json_value_free(rootValue);
    }

    return result;
}

//
// Sends telemetry data
//
int processTelemetry(APP_CONTEXT* appContext)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceClient;
    IOTHUB_MESSAGE_HANDLE messageHandle = NULL;
    char temperatureStringBuffer[32];

    deviceClient = appContext->deviceClient;

    if (snprintf(temperatureStringBuffer, sizeof(temperatureStringBuffer), "{\"temperature\":%.02f}", g_currentTemperature) < 0)
    {
        LogError("snprintf of current temperature telemetry failed");
    }
    else if (appContext->isConnected != true)
    {
        LogError("Not Connected");
    }
    else if ((messageHandle = IoTHubMessage_CreateFromString(temperatureStringBuffer)) == NULL)
    {
        LogError("IoTHubMessage_CreateFromString failed");
    }
    else if ((sendMessage(deviceClient, temperatureStringBuffer)) != true)
    {
        LogError("Unable to send telemetry message");
    }

    IoTHubMessage_Destroy(messageHandle);
}

//
// Build ISO8601 format time stamp string
//
static bool BuildUtcTimeFromCurrentTime(char* utcTimeBuffer, size_t utcTimeBufferSize)
{
    bool result;
    time_t currentTime;
    struct tm * currentTimeTm;
     static const char ISO8601Format[] = "%Y-%m-%dT%H:%M:%SZ";

    time(&currentTime);
    currentTimeTm = gmtime(&currentTime);

    if (strftime(utcTimeBuffer, utcTimeBufferSize, ISO8601Format, currentTimeTm) == 0)
    {
        LogError("snprintf on UTC time failed");
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}