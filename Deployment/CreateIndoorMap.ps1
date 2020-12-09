param([string] [Parameter(Mandatory=$true)] $mapSubscriptionKey,
      [string] [Parameter(Mandatory=$true)] $resourceGroupName,
      [string] [Parameter(Mandatory=$true)] $functionsAppName,
      [string] [Parameter(Mandatory=$true)] $webAppName
)

$DeploymentScriptOutputs = @{}
$Debug = $false

$progressPreference = 'silentlyContinue'
$ErrorActionPreference = 'silentlyContinue'
$WarningPreference = "SilentlyContinue"

Install-Module -Name AzureAD -SkipPublisherCheck -Force -AcceptLicense -AllowClobber
# Install-Module -Name Az.TimeSeriesInsights -SkipPublisherCheck -Force -AcceptLicense -AllowClobber
# Install-Module -Name Az.Websites -SkipPublisherCheck -Force -AcceptLicense -AllowClobber

##################################################
# Step 1 : Download sample Drawing data
##################################################
$url = "https://github.com/daisukeiot/IoT-Plug-and-Play-Workshop-Deploy/raw/main/MapData/Drawing.zip"
Invoke-WebRequest -Uri $url -Method Get -OutFile ".\Drawing.zip"

##################################################
# Step 2 : Upload Drawing data
##################################################
$url = "https://atlas.microsoft.com/mapData/upload?api-version=1.0&dataFormat=zip&subscription-key=$($mapSubscriptionKey)"
$resp = Invoke-WebRequest -Uri $url -Method Post -ContentType 'application/octet-stream' -InFile ".\Drawing.zip"
Write-Host "Response Status      : $($resp.StatusCode)"

# Make sure the drawing was uploaded.
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 

do {
    $resp = Invoke-RestMethod -Uri $url -Method Get
    if ($resp.status -ne "Succeeded") {
        if ($Debug -eq $true) {
            Write-Host "Upload : $($resp.status)"
        }
        Start-Sleep -Seconds 3.0
    }
    else {
        Write-Host "Upload : completed"
        $resLocation = [uri]$resp.resourceLocation
        $udid = $resLocation.Segments[3]
        break;
    }
} while ($true)

# Check status
$url = "https://atlas.microsoft.com/mapData/metadata/$($udid)?api-version=1.0&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
}
$resp = Invoke-RestMethod -Uri $url -Method Get

# double check status
if ($resp.uploadStatus -ne "Completed") {
    Write-Error "Upload Failed. Status : $($resp.uploadStatus)"
    return
}
$udid = $resp.udid

##################################################
# Step 3 : Convert a Drawing package
##################################################
Start-Sleep -Seconds 5
$url = "https://atlas.microsoft.com/conversion/convert?subscription-key=$($mapSubscriptionKey)&api-version=1.0&udid=$($udid)&inputType=DWG"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Start map data conversion"
}
$resp = Invoke-WebRequest -Uri $url -Method Post
Write-Host "Response Status      : $($resp.StatusCode)"

# url to check operation status
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 
do {
    $resp = Invoke-RestMethod -Uri $url -Method Get
    if ($resp.status -ne "Succeeded") {
        if ($Debug -eq $true) {
            Write-Host "Conversion           : $($resp.status)"
        }
        Start-Sleep -Seconds 1.0
    }
    else {
        Write-Host "Conversion           : completed"
        $resLocation = [uri]$resp.resourceLocation
        $conversionId = $resLocation.Segments[2]
        break;
    }
} while ($true)

##################################################
# Step 4 : Create a dataset
##################################################
Start-Sleep -Seconds 5
$url = "https://atlas.microsoft.com/dataset/create?api-version=1.0&conversionID=$($conversionId)&type=facility&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Creating dataset"
}

# this call fails with HTTP Status 500 every once in a while.
# wrap with try..catch and retry
do {
    try
    {
        $resp = Invoke-WebRequest -Uri $url -Method Post 
        if (($resp.StatusCode -eq 200) -or ($resp.StatusCode -eq 202))
        {
            break;
        }
        else
        {
            Start-Sleep -Seconds 5
        }
    }
    catch
    {
        Write-Host "Retrying.."
    }
} while ($true)

Write-Host "Response Status      : $($resp.StatusCode)"

# url to check operation status
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 
do {
    $resp = Invoke-RestMethod -Uri $url -Method Get
    if ($resp.status -ne "Succeeded") {
        if ($Debug -eq $true) {
            Write-Host "Dataset              : $($resp.status)"
        }
        Start-Sleep -Seconds 3.0
    }
    else {
        Write-Host "Dataset              : completed"
        $resLocation = [uri]$resp.resourceLocation
        $dataSetId = $resLocation.Segments[2]
        break;
    }
} while ($true)

Start-Sleep -Seconds 5
##################################################
# Step 5 : Create a tileset
##################################################
$url = "https://atlas.microsoft.com/tileset/create/vector?api-version=1.0&datasetID=$($dataSetId)&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Creating tileset"
}

# this call fails with HTTP Status 500 every once in a while.
# wrap with try..catch and retry
do {
    try
    {
        $resp = Invoke-WebRequest -Uri $url -Method Post 
        if (($resp.StatusCode -eq 200) -or ($resp.StatusCode -eq 202))
        {
            break;
        }
        else
        {
            Start-Sleep -Seconds 5
        }
    }
    catch
    {
        Write-Host "Retrying.."
    }
} while ($true)
Write-Host "Response Status      : $($resp.StatusCode)"

# url to check operation status
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 
do {
    $resp = Invoke-RestMethod -Uri $url -Method Get
    if ($resp.status -ne "Succeeded") {
        if ($Debug -eq $true) {

            Write-Host "Conversion           : $($resp.status)"
        }
        Start-Sleep -Seconds 3.0
    }
    else {
        Write-Host "Conversion           : completed"
        $resLocation = [uri]$resp.resourceLocation
        $tileSetId = $resLocation.Segments[2]
        break;
    }
} while ($true)

if ($Debug -eq $true) {
    #
    # Query Dataset
    #
    $url = "https://atlas.microsoft.com/wfs/datasets/$($dataSetId)/collections?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
    Write-Host "Calling RESTful API at $($url)"
    $resp = Invoke-RestMethod -Uri $url -Method Get
    $url = "https://atlas.microsoft.com/wfs/datasets/$($dataSetId)/collections/unit/items?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
}

Write-Host "Tileset ID           : $($tileSetId)"

##################################################
# Step 6 : Create a feature stateset
##################################################
Start-Sleep -Seconds 5.0
$stateSet = '{
    "styles":[
       {
          "keyname":"occupied",
          "type":"boolean",
          "rules":[
             {
                "true":"#FF0000",
                "false":"#00FF00"
             }
          ]
       },
       {
          "keyname":"temperature",
          "type":"number",
          "rules":[
             {
                "range":{
                   "exclusiveMaximum":66
                },
                "color":"#00204e"
             },
             {
                "range":{
                   "minimum":66,
                   "exclusiveMaximum":70
                },
                "color":"#0278da"
             },
             {
                "range":{
                   "minimum":70,
                   "exclusiveMaximum":74
                },
                "color":"#187d1d"
             },
             {
                "range":{
                   "minimum":74,
                   "exclusiveMaximum":78
                },
                "color":"#fef200"
             },
             {
                "range":{
                   "minimum":78,
                   "exclusiveMaximum":82
                },
                "color":"#fe8c01"
             },
             {
                "range":{
                   "minimum":82
                },
                "color":"#e71123"
             }
          ]
       }
    ]
 }'

$url = "https://atlas.microsoft.com/featureState/stateset?api-version=1.0&datasetId=$($dataSetId)&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Creating feature set"
}
$resp = Invoke-RestMethod -Uri $url -Method Post -ContentType 'application/json' -Body $stateSet
Write-Host "Response Status      : $($resp.StatusCode)"

$stateSetId = $resp.statesetId
Write-Host "Stateset ID          : $($stateSetId)"

##################################################
# Step 7 : Delete Map Data
##################################################
$url = "https://atlas.microsoft.com/mapData?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
$mapData = Invoke-RestMethod -Uri $url -Method Get

foreach ($mapDataItem in $mapData.mapDataList) {
    #Write-Host "Deleting $($mapDataItem.udid)"
    $url = "https://atlas.microsoft.com/mapData/$($mapDataItem.udid)?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
    Invoke-RestMethod -Uri $url -Method Delete
}

##################################################
# Step 7 : Create AD App, SP, and add settings to webapp
##################################################
# Install-Module -Name AzureAD -SkipPublisherCheck -Force -AcceptLicense -AllowClobber
# From deployment script, we cannot manipulate AD App because of priviledge.
#
$webapp = Get-AzWebApp -ResourceGroupName $resourceGroupName -Name $webAppName
$appSettings = $webapp.SiteConfig.AppSettings

$newAppSettings = @{}
ForEach ($item in $appSettings) {
    $newAppSettings[$item.Name] = $item.Value
}

$newAppSettings['Azure__AzureMap__TilesetId'] = $tileSetId
$newAppSettings['Azure__AzureMap__StatesetId'] = $stateSetId
Set-AzWebApp -ResourceGroupName $resourceGroupName -Name $webAppName  -AppSettings $newAppSettings

$funcitonsApp = Get-AzWebApp -ResourceGroupName $resourceGroupName -Name $functionsAppName
$newFunctionAppSettings = @{}
$functionAppSettings = $funcitonsApp.SiteConfig.AppSettings
ForEach ($item in $functionAppSettings) {
    $newFunctionAppSettings[$item.Name] = $item.Value
}

$newFunctionAppSettings['StatesetId'] = $stateSetId
Set-AzWebApp -ResourceGroupName $resourceGroupName -Name $functionsAppName  -AppSettings $newFunctionAppSettings
