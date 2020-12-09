#!/bin/bash

#az provider register --namespace 'Microsoft.DigitalTwins'

az extension add --name timeseriesinsights --yes

uniqueId=$1
resGroup=$2
webAppName="IoTPnPWS-Portal-${uniqueId}"
tsiName="IoTPnPWS-TSI-${uniqueId}"

az functionapp identity assign -g <your-resource-group> -n <your-App-Service-(function-app)-name>
az dt role-assignment create --dt-name <your-Azure-Digital-Twins-instance> --assignee "<principal-ID>" --role "Azure Digital Twins Data Owner"
az dt role-assignment create --dt-name <your-Azure-Digital-Twins-instance> --assignee "<Azure-AD-user-principal-name-of-user-to-assign>" --role "Azure Digital Twins Data Owner"

Create Event Route
az dt route create -n <your-Azure-Digital-Twins-instance-name> --endpoint-name <Event-Grid-endpoint-name> --route-name <my_route> --filter "type = 'Microsoft.DigitalTwins.Twin.Update'"

