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
Install-Module -Name Az.TimeSeriesInsights -SkipPublisherCheck -Force -AcceptLicense -AllowClobber
Install-Module -Name Az.Websites -SkipPublisherCheck -Force -AcceptLicense -AllowClobber

##################################################
# Step 1 : Download sample Drawing data
##################################################
$url = "https://github.com/daisukeiot/IoT-Plug-and-Play-Workshop-Deploy/raw/main/MapData/Drawing.zip"
Invoke-WebRequest -Uri "$url" -Method Get -OutFile ".\Drawing.zip"

##################################################
# Step 2 : Upload Drawing data
##################################################
$url = "https://us.atlas.microsoft.com/mapData/upload?api-version=1.0&dataFormat=zip&subscription-key=$($mapSubscriptionKey)"
Write-Host "Uploading $($url)"
Write-Host "Invoke-WebRequest -Uri "$url" -Method Post -ContentType 'application/octet-stream' -InFile '.\Drawing.zip'"
$resp = Invoke-WebRequest -Uri "$url" -Method Post -ContentType 'application/octet-stream' -InFile '.\Drawing.zip'
Write-Host "Response Status      : $($resp.StatusCode)"

# Make sure the drawing was uploaded.
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 

do {
    $resp = Invoke-RestMethod -Uri "$url" -Method Get
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
$url = "https://us.atlas.microsoft.com/mapData/metadata/$($udid)?api-version=1.0&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
}
$resp = Invoke-RestMethod -Uri "$url" -Method Get

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
$url = "https://us.atlas.microsoft.com/conversion/convert?subscription-key=$($mapSubscriptionKey)&api-version=1.0&udid=$($udid)&inputType=DWG"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Start map data conversion"
}
$resp = Invoke-WebRequest -Uri "$url" -Method Post
Write-Host "Response Status      : $($resp.StatusCode)"

# url to check operation status
$url = "$($resp.Headers.Location)&subscription-key=$($mapSubscriptionKey)" 
$url = $url.Replace("https://atlas", "https://us.atlas")
do {
    $resp = Invoke-RestMethod -Uri "$url" -Method Get
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
$url = "https://us.atlas.microsoft.com/dataset/create?api-version=1.0&conversionID=$($conversionId)&type=facility&subscription-key=$($mapSubscriptionKey)"
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
        $resp = Invoke-WebRequest -Uri "$url" -Method Post 
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
$url = $url.Replace("https://atlas", "https://us.atlas")

do {
    $resp = Invoke-RestMethod -Uri "$url" -Method Get
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
$resp = Invoke-RestMethod -method GET -uri "https://us.atlas.microsoft.com/wfs/datasets/$dataSetId/collections/unit/items?subscription-key=$mapSubscriptionKey&api-version=1.0"
Write-Host "Resp $resp"
Write-Host "Searching Feature : Dataset ID $dataSetId"

$bFound = $false
$url = "https://us.atlas.microsoft.com/wfs/datasets/$dataSetId/collections/unit/items?api-version=1.0"
$unitId = ""

do {

    Write-Host "Searching at $url"
    $resp = Invoke-RestMethod -method GET -uri "$url&subscription-key=$mapSubscriptionKey"
    $url = ""
    Foreach ($feature in $resp.features)
    {
        if ($feature.type -eq "Feature")
        {
            if ($debug)
            {
                Write-Host "Feature : $($feature.properties.name) ID : $($feature.id)"
            }
            if ($feature.properties.name -eq "141")
            {
                Write-Host "***************** Found!!!****************"
                $unitId = $feature.id;
                $bFound = $true
                break;
            }
        }
    }

    if ($bFound -eq $false)
    {
        Foreach ($link in $resp.links)
        {
            if ($link.rel -eq "next")
            {
                $url = $link.href;
                $url = $url.Replace("https://atlas", "https://us.atlas")
                break;
            }
        }
    }    

} while (($url -ne "") -and ($bFound -eq $false))


##################################################
# Step 5 : Create a tileset
##################################################
$url = "https://us.atlas.microsoft.com/tileset/create/vector?api-version=1.0&datasetID=$($dataSetId)&subscription-key=$($mapSubscriptionKey)"
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
        $resp = Invoke-WebRequest -Uri "$url" -Method Post 
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
$url = $url.Replace("https://atlas", "https://us.atlas")

do {
    $resp = Invoke-RestMethod -Uri "$url" -Method Get
    if ($resp.status -ne "Succeeded") {
        if ($Debug -eq $true) {

            Write-Host "Tileset              : $($resp.status)"
        }
        Start-Sleep -Seconds 3.0
    }
    else {
        Write-Host "Tileset              : completed"
        $resLocation = [uri]$resp.resourceLocation
        $tileSetId = $resLocation.Segments[2]
        break;
    }
} while ($true)

if ($Debug -eq $true) {
    #
    # Query Dataset
    #
    $url = "https://us.atlas.microsoft.com/wfs/datasets/$($dataSetId)/collections?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
    Write-Host "Calling RESTful API at $($url)"
    $resp = Invoke-RestMethod -Uri "$url" -Method Get
    $url = "https://us.atlas.microsoft.com/wfs/datasets/$($dataSetId)/collections/unit/items?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
}

Write-Host "Tileset ID           : $($tileSetId)"

##################################################
# Step 6 : Create a feature stateset
##################################################
Start-Sleep -Seconds 5.0
$stateSet = '{
    "styles":[
       {
          "keyname":"temperature",
          "type":"number",
          "rules":[
             {
                "range":{
                   "exclusiveMaximum":60
                },
                "color":"#FF1919"
             },
             {
                "range":{
                   "minimum":50,
                   "exclusiveMaximum":60
                },
                "color":"#19FFD9"
             },
             {
                "range":{
                   "minimum":40,
                   "exclusiveMaximum":50
                },
                "color":"#1966FF"
             },
             {
                "range":{
                   "minimum":30,
                   "exclusiveMaximum":40
                },
                "color":"#00E600"
             },
             {
                "range":{
                   "minimum":20,
                   "exclusiveMaximum":30
                },
                "color":"#33FFFF"
             },
             {
                "range":{
                   "minimum":20
                },
                "color":"#CCF7FF"
             }
          ]
       }
    ]
 }'

$url = "https://us.atlas.microsoft.com/featureState/stateset?api-version=1.0&datasetId=$($dataSetId)&subscription-key=$($mapSubscriptionKey)"
if ($debug)
{
    Write-Host "Calling RESTful API at $($url)"
} else {
    Write-Host "Creating feature set"
}
$resp = Invoke-RestMethod -Uri "$url" -Method Post -ContentType 'application/json' -Body $stateSet

$stateSetId = $resp.statesetId
Write-Host "Stateset ID          : $($stateSetId)"

##################################################
# Step 7 : Delete Map Data
##################################################
$url = "https://us.atlas.microsoft.com/mapData?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
$mapData = Invoke-RestMethod -Uri "$url" -Method Get

foreach ($mapDataItem in $mapData.mapDataList) {
    #Write-Host "Deleting $($mapDataItem.udid)"
    $url = "https://us.atlas.microsoft.com/mapData/$($mapDataItem.udid)?subscription-key=$($mapSubscriptionKey)&api-version=1.0"
    Invoke-RestMethod -Uri "$url" -Method Delete
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

$newFunctionAppSettings['UnitId'] = $unitId
$newFunctionAppSettings['StatesetId'] = $stateSetId
Set-AzWebApp -ResourceGroupName $resourceGroupName -Name $functionsAppName  -AppSettings $newFunctionAppSettings
