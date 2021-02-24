#!/bin/bash

az extension add --name timeseriesinsights --yes --only-show-errors

uniqueId=$1
resGroup=$2
trackerUrl=$3
webAppName="IoTPnPWS-Portal-${uniqueId}"
tsiName="IoTPnPWS-TSI-${uniqueId}"

trackerFullUrl="${trackerUrl}?resGroup=${resGroup}&uniqueId=${uniqueId}&progressMarker=3"
curl -X GET $trackerFullUrl

echo "Web App Name  : ${webAppName}"
echo "TSI Env Name  : ${tsiName}"

subscriptionId=$(az account show --query id -o tsv)
adAppName='IoTPnPWS-TSI-AD-App'-"$subscriptionId"
# az login --identity
servicePrincipalAppId=$(az ad app list --all --query '[].{AppId:appId}' --display-name $adAppName -o tsv)
if [ -z $servicePrincipalAppId ]; then
    servicePrincipalAppId=$(az ad app create --display-name $adAppName --identifier-uris "https://${adAppName}"  --oauth2-allow-implicit-flow true --required-resource-accesses '[{"resourceAppId":"120d688d-1518-4cf7-bd38-182f158850b6","resourceAccess":[{"id":"a3a77dfe-67a4-4373-b02a-dfe8485e2248","type":"Scope"}]}]' --query appId -o tsv)
fi

servicePrincipalObjectId=$(az ad sp list --query '[].objectId' --display-name "${adAppName}" -o tsv)
if [ -z "$servicePrincipalObjectId" ]; then
    servicePrincipalObjectId=$(az ad sp create --id $servicePrincipalAppId --query objectId -o tsv)
fi
servicePrincipalSecret=$(az ad app credential reset --append --id $servicePrincipalAppId --credential-description "TSISecret" --query password --only-show-errors -o tsv )
servicePrincipalTenantId=$(az ad sp show --id $servicePrincipalAppId --query appOwnerTenantId -o tsv)
#json="{\"appId\":\"$servicePrincipalAppId\",\"spSecret\":\"$servicePrincipalSecret\",\"tenantId\":\"$servicePrincipalTenantId\",\"spObjectId\":\"$servicePrincipalObjectId\"}"

az ad app update --id $servicePrincipalAppId --reply-urls "https://${webAppName}.azurewebsites.net/"

temp=$(az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__tenantId=$servicePrincipalTenantId --query "[?name=='Azure__TimeSeriesInsights__tenantId'].[value]" -o tsv)
echo "TSI Tenant ID : ${temp}"

temp=$(az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__clientId=$servicePrincipalAppId --query "[?name=='Azure__TimeSeriesInsights__clientId'].[value]" -o tsv)
echo "TSI Client ID : ${temp}"

temp=$(az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__tsiSecret=$servicePrincipalSecret --query "[?name=='Azure__TimeSeriesInsights__tsiSecret'].[value]" -o tsv)
echo "TSI Secret    : ${temp}"

temp=$(az tsi access-policy list -g $resGroup --environment-name $tsiName --query 'value[].principalObjectId' -o tsv --only-show-errors)

if [ -z $temp ]; then
    temp=$(az tsi access-policy create -g $resGroup --environment-name $tsiName -n "TSI-SP" --principal-object-id $servicePrincipalObjectId --roles Reader --only-show-errors)
fi

trackerFullUrl="${trackerUrl}?resGroup=${resGroup}&uniqueId=${uniqueId}&progressMarker=4"
curl -X GET $trackerFullUrl