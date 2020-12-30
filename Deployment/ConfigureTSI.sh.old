#!/bin/bash

az extension add --name timeseriesinsights --yes

uniqueId=$1
resGroup=$2
webAppName="IoTPnPWS-Portal-${uniqueId}"
tsiName="IoTPnPWS-TSI-${uniqueId}"

echo "Web App Name : ${webAppName}"
echo "TSI Env Name : ${tsiName}"

subscriptionId=$(az account show --query id -o tsv)
adAppName='IoTPnPWS-TSI-AD-App'-"$subscriptionId"
# az login --identity
servicePrincipalAppId=$(az ad app create --display-name $adAppName --identifier-uris "https://${adAppName}"  --oauth2-allow-implicit-flow true --required-resource-accesses '[{"resourceAppId":"120d688d-1518-4cf7-bd38-182f158850b6","resourceAccess":[{"id":"a3a77dfe-67a4-4373-b02a-dfe8485e2248","type":"Scope"}]}]' --query appId -o tsv)
#servicePrincipalObjectId=$(az ad sp list --show-mine --query "[?appDisplayName=='$adAppName'].principalObjectId" -o tsv)
servicePrincipalObjectId=$(az ad sp show --id "https://${adAppName}" --query objectId -o tsv)
if [ -z "$servicePrincipalObjectId" ]; then
    servicePrincipalObjectId=$(az ad sp create --id $servicePrincipalAppId --query objectId -o tsv)
fi
servicePrincipalSecret=$(az ad app credential reset --append --id $servicePrincipalAppId --credential-description "TSISecret" --query password -o tsv)
servicePrincipalTenantId=$(az ad sp show --id $servicePrincipalAppId --query appOwnerTenantId -o tsv)
echo "servicePrincipalSecret :   ${servicePrincipalSecret}"
echo "servicePrincipalTenantId : ${servicePrincipalTenantId}"
#json="{\"appId\":\"$servicePrincipalAppId\",\"spSecret\":\"$servicePrincipalSecret\",\"tenantId\":\"$servicePrincipalTenantId\",\"spObjectId\":\"$servicePrincipalObjectId\"}"

az ad app update --id $servicePrincipalAppId --reply-urls "https://${webAppName}.azurewebsites.net/"
az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__tenantId=$servicePrincipalTenantId
az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__clientId=$servicePrincipalAppId
az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__tsiSecret=$servicePrincipalSecret
az webapp config appsettings set --name $webAppName --resource-group $resGroup --settings Azure__TimeSeriesInsights__clientId=$servicePrincipalAppId
az timeseriesinsights access-policy create -g $resGroup --environment-name $tsiName -n "TSI-SP" --principal-object-id $servicePrincipalObjectId --roles Reader
