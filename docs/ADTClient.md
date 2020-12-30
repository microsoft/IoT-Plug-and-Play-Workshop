# ADT Client Sample Application

The sample application in `/demo/ADTClient` is based on [Digital Twin Sample](https://github.com/Azure-Samples/digital-twins-samples/tree/master/) with minor customizations.

- Digital Twin Models (`/demo/ADTClient/Models`)
    Model files are customized with 3 models, Room, Floor, and Thermostat.
- ADT URL  
    In order to authenticate and access ADT instance, ADT Client application needs ADT Host Name.  The application is modified to take ADT Host Name as a command line parameter.

## Preparation

1. Clone this repo (e.g. C:\IoT-Plug-and-Play-Workshop)
1. Install .net core 3.1 runtime from <https://dotnet.microsoft.com/download/dotnet-core/3.1>

## Running ADT Client app

Run ADT Client application with ADT Host Name (_ADT_Host_Name) from the deployment output window:

![ADTHostName](media/ADTHostName.png)

```cmd
<clone folder>\demo\ADTClient\SampleClientApp.exe <ADT Host URL>
```

Example :

```cmd
cd C:\IoT-Plug-and-Play-Workshop\demo\ADTClient
SampleClientApp.exe https://IoTPnPWS-ADT.api.wus2.digitaltwins.azure.net
```

[Return to Azure Digital Twin instruction](AzureDigitalTwins.md)
