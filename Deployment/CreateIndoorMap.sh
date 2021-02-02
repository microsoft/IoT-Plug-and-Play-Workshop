#!/bin/bash

shopt -s extglob

unset TILESET_ID
unset STATESET_ID
unset DATASET_ID
unset STATESET_ID

mapSubscriptionKey=${1:?}
resGroup=${2:?}
uniqueId=${3:?}
trackerUrl=$4

functionsName="IoTPnPWS-Functions-${uniqueId:?}"
webAppName="IoTPnPWS-Portal-${uniqueId:?}"

echo "Web App Name   : ${webAppName}"
echo "Functions Name : ${functionsName}"

parse_header() {
  local -n header=$1 # Nameref argument
  # Check that argument is the name of an associative array
  case ${header@a} in
    A | At) ;;
    *)
      printf \
      'Variable %s with attributes %s is not a suitable associative array\n' \
      "${!header}" "${header@a}" >&2
      return 1
      ;;
  esac
  header=() # Clear the associative array
  local -- line rest v
  local -l k # Automatically lowercased

  # Get the first line, assuming HTTP/1.0 or above. Note that these fields
  # have Capitalized names.
  IFS=$' \t\n\r' read -r header['Proto'] header['Status'] rest
  # Drop the CR from the message, if there was one.
  header['Message']="${rest%%*([[:space:]])}"
  # Now read the rest of the headers.
  while IFS=: read -r line rest && [ -n "$line$rest" ]; do
    rest=${rest%%*([[:space:]])}
    rest=${rest##*([[:space:]])}
    line=${line%%*([[:space:]])}
    [ -z "$line" ] && break # Blank line is end of headers stream
    if [ -n "$rest" ]; then
      k=$line
      v=$rest
    else
      # Handle folded header
      # See: https://tools.ietf.org/html/rfc2822#section-2.2.3
      v+=" ${line##*([[:space:]])}"
    fi
    header["$k"]="$v"
  done
}

# Send Tracker
if [ ! -z ${trackerUrl} ];then
  echo "Tracker        : Progress Marker 5"
  trackerFullUrl="${trackerUrl}?resGroup=${resGroup}&uniqueId=${uniqueId}&progressMarker=5"
  curl -X GET $trackerFullUrl
fi

##################################################
# Step 1 : Download sample Drawing data
##################################################
DATA_FILE="./Drawing.zip"
if [ -e $DATA_FILE ]; then
  rm -r -f $DATA_FILE
fi

url="https://raw.githubusercontent.com/microsoft/IoT-Plug-and-Play-Workshop/main/MapData/Drawing.zip"
echo "Data download  : Start downloading from $url"
wget --quiet -O $DATA_FILE $url

if [ ! -e $DATA_FILE ]; then
  echo "Error : Download failed"
  exit 1
fi

##################################################
# Step 2 : Upload Drawing data
# https://docs.microsoft.com/en-us/rest/api/maps/data/uploadpreview
##################################################
declare -A HTTP_HEADERS
PHASE_HEADER="Data Upload    "
url=$"https://us.atlas.microsoft.com/mapData/upload?api-version=1.0&dataFormat=zip&subscription-key=${mapSubscriptionKey}"
echo "${PHASE_HEADER}: URL=$url"
parse_header HTTP_HEADERS < <(
  curl -is -X POST -H "Content-Type: application/octet-stream" --data-binary @"$DATA_FILE" "$url"
)

RESP_STATUS=${HTTP_HEADERS[Status]}
# https://docs.microsoft.com/en-us/rest/api/maps/data/uploadpreview#response
if [ $RESP_STATUS == "202" -o RESP_STATUS == "201" ]; then
  echo "${PHASE_HEADER}: Request Success [${HTTP_HEADERS[Status]}]"
else
  echo "${PHASE_HEADER}: Error ${HTTP_HEADERS[Status]}"
  typeset -p HTTP_HEADERS
  exit 1
fi

RESP_LOCATION=`echo ${HTTP_HEADERS[location]} | sed 's+https://atlas.microsoft.com+https://us.atlas.microsoft.com+g'`

echo "${PHASE_HEADER}: Location ${RESP_LOCATION}"

# for k in "${!HTTP_HEADERS[@]}"; do
#   printf '[%q]=%q\n' "$k" "${HTTP_HEADERS[$k]}"
# done
# typeset -p HTTP_HEADERS

# Make sure the drawing was uploaded.
url=$"$RESP_LOCATION&subscription-key=$mapSubscriptionKey"
#echo "${PHASE_HEADER}: Checking status @ $url"

unset HTTP_HEADERS

printf "${PHASE_HEADER}: Uploading"
while true; do
  REST_REPONSE=`curl -s -X GET "$url"`
  REST_STATUS=`echo $REST_REPONSE | jq -r .status`
  if [[ "$REST_STATUS" == "NotStarted" ]]; then
    # echo "${PHASE_HEADER}: Waiting for Start"
    printf "."
    sleep 1
    continue
  elif [[ "$REST_STATUS" == "Running" ]]; then
    # echo "${PHASE_HEADER}: Upload running"
    printf "."
    sleep 1
    continue
  elif [ "$REST_STATUS" = "Succeeded" ]; then
    # echo "$REST_REPONSE"
    printf ".\n"
    OPERATION_ID=`echo $REST_REPONSE | jq -r .operationId`
    RESOURCE_LOCATION=`echo $REST_REPONSE | jq -r .resourceLocation`
    arrayUrl=(${RESOURCE_LOCATION//\// })
    # echo "Array Count ${#arrayUrl[@]}"
    arrayUrl=(${arrayUrl[@]//[=?]/ })
    UDID=${arrayUrl[4]}
    #IFS='/' read -r -a array <<< "$RESOURCE_LOCATION"
    #UDID=${array[5]}
    echo "${PHASE_HEADER}: Success Operation=$OPERATION_ID UDID=$UDID @ $RESOURCE_LOCATION"
    break
  elif [ "$REST_STATUS" = "Failed" ]; then
    echo "${PHASE_HEADER}: Error failed : ${REST_RESPONSE}"
    exit 1
  else
    echo "${PHASE_HEADER}: Warning : Unrecognized status $REST_STATUS"
  fi
done

# Double check uploaded data status
RESOURCE_LOCATION=`echo ${RESOURCE_LOCATION} | sed 's+https://atlas.microsoft.com+https://us.atlas.microsoft.com+g'`
url=$"${RESOURCE_LOCATION}&subscription-key=${mapSubscriptionKey}"
#echo "${PHASE_HEADER}: Checking status @ $url"
REST_REPONSE=`curl -s -X GET "$url"`
REST_STATUS=`echo $REST_REPONSE | jq -r .uploadStatus`

if [ "$REST_STATUS" != "Completed" ]; then
  echo "${PHASE_HEADER}: Error $REST_STATUS"
  exit 1
fi

UDID=`echo $REST_REPONSE | jq -r .udid`

##################################################
# Step 3 : Convert a Drawing package
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/convertpreview
##################################################
PHASE_HEADER="Conversion     "
url="https://us.atlas.microsoft.com/conversion/convert?subscription-key=${mapSubscriptionKey}&api-version=1.0&udid=${UDID}&inputType=DWG"
echo "${PHASE_HEADER}: URL=${url}"
unset HTTP_HEADERS
declare -A HTTP_HEADERS
parse_header HTTP_HEADERS < <(
  curl -is -X POST -H "Content-Length: 0" "${url}"
)

RESP_STATUS=${HTTP_HEADERS[Status]}
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/convertpreview#response
if [ $RESP_STATUS == "202" -o RESP_STATUS == "201" ]; then
  echo "${PHASE_HEADER}: Request Success [${HTTP_HEADERS[Status]}]"
else
  echo "${PHASE_HEADER}: Error ${HTTP_HEADERS[Status]}"
  typeset -p HTTP_HEADERS
  exit 1
fi

RESP_LOCATION=`echo ${HTTP_HEADERS[location]} | sed 's+https://atlas.microsoft.com+https://us.atlas.microsoft.com+g'`

url=$"$RESP_LOCATION&subscription-key=$mapSubscriptionKey"
#echo "${PHASE_HEADER}: Checking status @ $url"

printf "${PHASE_HEADER}: Converting"

while true; do
  REST_RESPONSE=`curl -s -X GET "$url"`
  REST_STATUS=`echo $REST_RESPONSE | jq -r .status`
  if [[ "$REST_STATUS" == "NotStarted" ]]; then
    # echo "${PHASE_HEADER}: Waiting for Start"
    printf "."
    sleep 1
    continue
  elif [[ "$REST_STATUS" == "Running" ]]; then
    # echo "${PHASE_HEADER}: Conversion running"
    printf "."
    sleep 1
    continue
  elif [ "$REST_STATUS" = "Succeeded" ]; then
    printf ".\n"
    # echo "$REST_RESPONSE"
    OPERATION_ID=`echo $REST_RESPONSE | jq -r .operationId`
    RESOURCE_LOCATION=`echo $REST_RESPONSE | jq -r .resourceLocation`
    arrayUrl=(${RESOURCE_LOCATION//\// })
    arrayUrl=(${arrayUrl[@]//[=?]/ })
    # echo "Array Count ${#arrayUrl[@]}"
    # echo "${arrayUrl[@]}"
    CONVERSION_ID=${arrayUrl[3]}
    # #IFS='/' read -r -a array <<< "$RESOURCE_LOCATION"
    # #UDID=${array[5]}
    echo "${PHASE_HEADER}: Success Operation=$OPERATION_ID ConversionID=$CONVERSION_ID @ $RESOURCE_LOCATION"
    break
  elif [ "$REST_STATUS" = "Failed" ]; then
    echo "${PHASE_HEADER}: Error failed : ${REST_RESPONSE}"
    exit 1
  else
    echo "${PHASE_HEADER}: Warning : Unrecognized status $REST_STATUS"
  fi
done

##################################################
# Step 4 : Create a dataset
##################################################
PHASE_HEADER="Dataset Create "
url="https://us.atlas.microsoft.com/dataset/create?api-version=1.0&conversionID=${CONVERSION_ID}&type=facility&subscription-key=${mapSubscriptionKey}"
echo "${PHASE_HEADER}: URL=${url}"
unset HTTP_HEADERS
declare -A HTTP_HEADERS
parse_header HTTP_HEADERS < <(
  curl -is -X POST -H "Content-Length: 0" "${url}"
)

RESP_STATUS=${HTTP_HEADERS[Status]}
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/convertpreview#response
if [ $RESP_STATUS == "202" -o RESP_STATUS == "201" ]; then
  echo "${PHASE_HEADER}: Request Success [${HTTP_HEADERS[Status]}]"
else
  echo "${PHASE_HEADER}: Error ${HTTP_HEADERS[Status]}"
  typeset -p HTTP_HEADERS
  exit 1
fi

RESP_LOCATION=`echo ${HTTP_HEADERS[location]} | sed 's+https://atlas.microsoft.com+https://us.atlas.microsoft.com+g'`

url=$"$RESP_LOCATION&subscription-key=$mapSubscriptionKey"
#echo "${PHASE_HEADER}: Checking status @ $url"

printf "${PHASE_HEADER}: Creating Dataset"

while true; do
  REST_RESPONSE=`curl -s -X GET "$url"`
  REST_STATUS=`echo $REST_RESPONSE | jq -r .status`
  if [[ "$REST_STATUS" == "NotStarted" ]]; then
    # echo "${PHASE_HEADER}: Waiting for Start"
    printf "."
    sleep 1
    continue
  elif [[ "$REST_STATUS" == "Running" ]]; then
    # echo "${PHASE_HEADER}: Conversion running"
    printf "."
    sleep 1
    continue
  elif [ "$REST_STATUS" = "Succeeded" ]; then
    printf ".\n"
    # echo "$REST_RESPONSE"
    OPERATION_ID=`echo $REST_RESPONSE | jq -r .operationId`
    RESOURCE_LOCATION=`echo $REST_RESPONSE | jq -r .resourceLocation`
    arrayUrl=(${RESOURCE_LOCATION//\// })
    arrayUrl=(${arrayUrl[@]//[=?]/ })
    # echo "Array Count ${#arrayUrl[@]}"
    # echo "${arrayUrl[@]}"
    DATASET_ID=${arrayUrl[3]}
    # # #IFS='/' read -r -a array <<< "$RESOURCE_LOCATION"
    # # #UDID=${array[5]}
    echo "${PHASE_HEADER}: Success Operation=$OPERATION_ID Dataset ID=$DATASET_ID @ $RESOURCE_LOCATION"
    break
  elif [ "$REST_STATUS" = "Failed" ]; then
    echo "${PHASE_HEADER}: Error failed : ${REST_RESPONSE}"
    exit 1
  else
    echo "${PHASE_HEADER}: Warning : Unrecognized status $REST_STATUS"
  fi
done

##################################################
# Step 5 : Create a tileset
##################################################
PHASE_HEADER="Tileset Create "
url="https://us.atlas.microsoft.com/tileset/create/vector?api-version=1.0&datasetID=${DATASET_ID}&subscription-key=${mapSubscriptionKey}"
echo "${PHASE_HEADER}: URL=${url}"
unset HTTP_HEADERS
declare -A HTTP_HEADERS
parse_header HTTP_HEADERS < <(
  curl -is -X POST -H "Content-Length: 0" "${url}"
)

RESP_STATUS=${HTTP_HEADERS[Status]}
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/convertpreview#response
if [ $RESP_STATUS == "202" -o RESP_STATUS == "201" ]; then
  echo "${PHASE_HEADER}: Request Success [${HTTP_HEADERS[Status]}]"
else
  echo "${PHASE_HEADER}: Error ${HTTP_HEADERS[Status]}"
  typeset -p HTTP_HEADERS
  exit 1
fi

RESP_LOCATION=`echo ${HTTP_HEADERS[location]} | sed 's+https://atlas.microsoft.com+https://us.atlas.microsoft.com+g'`

url=$"$RESP_LOCATION&subscription-key=$mapSubscriptionKey"
echo "${PHASE_HEADER}: Checking convetsion status @ $url"

printf "${PHASE_HEADER}: Creating Tileset"

while true; do
  REST_RESPONSE=`curl -s -X GET "$url"`
  REST_STATUS=`echo $REST_RESPONSE | jq -r .status`
  if [[ "$REST_STATUS" == "NotStarted" ]]; then
    # echo "${PHASE_HEADER}: Waiting for Start"
    printf "."
    sleep 1
    continue
  elif [[ "$REST_STATUS" == "Running" ]]; then
    # echo "${PHASE_HEADER}: Conversion running"
    printf "."
    sleep 1
    continue
  elif [ "$REST_STATUS" = "Succeeded" ]; then
    printf ".\n"
    # echo "$REST_RESPONSE"
    OPERATION_ID=`echo $REST_RESPONSE | jq -r .operationId`
    RESOURCE_LOCATION=`echo $REST_RESPONSE | jq -r .resourceLocation`
    arrayUrl=(${RESOURCE_LOCATION//\// })
    arrayUrl=(${arrayUrl[@]//[=?]/ })
    # echo "Array Count ${#arrayUrl[@]}"
    # echo "${arrayUrl[@]}"
    TILESET_ID=${arrayUrl[3]}
    # # #IFS='/' read -r -a array <<< "$RESOURCE_LOCATION"
    # # #UDID=${array[5]}
    echo "${PHASE_HEADER}: Success Operation=$OPERATION_ID Tileset ID=$TILESET_ID @ $RESOURCE_LOCATION"
    break
  elif [ "$REST_STATUS" = "Failed" ]; then
    echo "${PHASE_HEADER}: Error failed : ${REST_RESPONSE}"
    exit 1
  else
    echo "${PHASE_HEADER}: Warning : Unrecognized status $REST_STATUS"
  fi
done

##################################################
# Step 6 : Create a feature stateset
##################################################
PHASE_HEADER="Feature State  "
url="https://us.atlas.microsoft.com/featureState/stateset?api-version=1.0&datasetId=${DATASET_ID}&subscription-key=${mapSubscriptionKey}"
echo "${PHASE_HEADER}: URL=${url}"
unset HTTP_HEADERS
declare -A HTTP_HEADERS
STATE_SET='{"styles":[{"keyname":"co2","type":"number","rules":[{"range":{"exclusiveMaximum":1000},"color":"#00FF00"},{"range":{"minimum":1000,"exclusiveMaximum":1100},"color":"#c0ff00"},{"range":{"minimum":1100,"exclusiveMaximum":1200},"color":"#dfff00"},{"range":{"minimum":1200,"exclusiveMaximum":1300},"color":"#ffff00"},{"range":{"minimum":1300,"exclusiveMaximum":1400},"color":"#ffd200"},{"range":{"minimum":1400,"exclusiveMaximum":1500},"color":"#ffa500"},{"range":{"minimum":1500},"color":"#FF0000"}]},{"keyname":"temperature","type":"number","rules":[{"range":{"exclusiveMaximum":20},"color":"#CCF7FF"},{"range":{"minimum":20,"exclusiveMaximum":30},"color":"#33FFFF"},{"range":{"minimum":30,"exclusiveMaximum":40},"color":"#a4ff2e"},{"range":{"minimum":40,"exclusiveMaximum":50},"color":"#ff2eff"},{"range":{"minimum":50,"exclusiveMaximum":60},"color":"#ff6d2e"},{"range":{"minimum":60},"color":"#FF1919"}]},{"keyname":"light","type":"number","rules":[{"range":{"exclusiveMaximum":10},"color":"#000000"},{"range":{"minimum":10,"exclusiveMaximum":20},"color":"#242424"},{"range":{"minimum":20,"exclusiveMaximum":30},"color":"#3a3a3a"},{"range":{"minimum":30,"exclusiveMaximum":40},"color":"#4f4f4f"},{"range":{"minimum":40,"exclusiveMaximum":50},"color":"#646464"},{"range":{"minimum":50,"exclusiveMaximum":60},"color":"#797979"},{"range":{"minimum":60,"exclusiveMaximum":70},"color":"#919191"},{"range":{"minimum":70,"exclusiveMaximum":80},"color":"#ababab"},{"range":{"minimum":80,"exclusiveMaximum":90},"color":"#c8c8c8"},{"range":{"minimum":90},"color":"#FFFFFF"}]}]}'
# (echo "${STATE_SET}" | jq '.styles[]')

REST_RESPONSE=`curl -s -X POST -H "Content-type: application/json" -d ${STATE_SET} "${url}"`
STATESET_ID=`echo $REST_RESPONSE | jq -r .statesetId`

echo "${PHASE_HEADER}: Stateset ID ${STATESET_ID}"

##################################################
# Step 7 : Delete Maps Data
##################################################
PHASE_HEADER="Cleanup        "
url="https://us.atlas.microsoft.com/mapData?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`

#DELETE_LIST=$(echo ${LIST_RESP} | jq -c '[.mapDataList[]| {udid: .udid, location: .location}]')
for row in $(echo "${LIST_RESP}" | jq -r '.mapDataList[] | @base64'); do
  _jq() {
      echo ${row} | base64 --decode | jq -r ${1}
  }
  # echo $(_jq '.udid') $(_jq '.location')
  url="$(_jq '.location')&subscription-key=${mapSubscriptionKey}"
  echo "${PHASE_HEADER}: Deleting UDID=${UDID}"
  curl -s -X DELETE  "${url}"
done

##################################################
# Step 7 : Create AD App, SP, and add settings to webapp
##################################################
PHASE_HEADER="App Settings   "

echo "${PHASE_HEADER}: Tileset=            ${TILESET_ID}"
echo "${PHASE_HEADER}: Stateset=           ${STATESET_ID}"
echo "${PHASE_HEADER}: Dataset=            ${DATASET_ID}"

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${webAppName} --settings Azure__AzureMap__TilesetId=${TILESET_ID:?} --query "[?name=='Azure__AzureMap__TilesetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: WebApp Tileset=     ${temp}"

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${webAppName} --settings Azure__AzureMap__StatesetId=${STATESET_ID:?} --query "[?name=='Azure__AzureMap__StatesetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: WebApp Stateset=    ${temp}"

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${functionsName} --settings DatasetId=${DATASET_ID:?} --query "[?name=='DatasetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: Functions Dataset=  ${temp}"

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${functionsName} --settings StatesetId=${STATESET_ID:?} --query "[?name=='StatesetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: Functions Stateset= ${temp}"

# Send Tracker
if [ ! -z ${trackerUrl} ];then
  echo "Tracker        : Progress Marker 6"
  trackerFullUrl="${trackerUrl}?resGroup=${resGroup}&uniqueId=${uniqueId}&progressMarker=6"
  curl -X GET $trackerFullUrl
fi