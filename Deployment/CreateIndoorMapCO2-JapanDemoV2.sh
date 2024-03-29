#!/bin/bash

shopt -s extglob

unset TILESET_ID
unset STATESET_ID
unset DATASET_ID
unset STATESET_ID

mapSubscriptionKey=${1:?}
resGroup=${2:?}
uniqueId=${3:?}
apiVersion=2.0

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
      #return 1
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
url=$"https://us.atlas.microsoft.com/mapData?api-version=${apiVersion}&dataFormat=dwgzippackage&subscription-key=${mapSubscriptionKey}"
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

OPERATION_LOCATION=${HTTP_HEADERS[operation-location]}

echo "${PHASE_HEADER}: Location ${OPERATION_LOCATION}"

# for k in "${!HTTP_HEADERS[@]}"; do
#   printf '[%q]=%q\n' "$k" "${HTTP_HEADERS[$k]}"
# done
# typeset -p HTTP_HEADERS

# Make sure the drawing was uploaded.
url=$"$OPERATION_LOCATION&subscription-key=$mapSubscriptionKey"
#echo "${PHASE_HEADER}: Checking status @ $url"

unset HTTP_HEADERS

printf "${PHASE_HEADER}: Uploading"
while true; do
  REST_RESPONSE=`curl -s -X GET "$url"`
  REST_STATUS=`echo $REST_RESPONSE | jq -r .status`
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
    # echo "$REST_RESPONSE"
    printf ".\n"
    OPERATION_ID=`echo $REST_RESPONSE | jq -r .operationId`

    read RESOURCE_LOCATION < <(
      curl -sI -X GET "${url}" | 
      awk '/^resource-location/ { RESOURCE_LOCATION = $2 } 
          END { printf("%s\n",RESOURCE_LOCATION) }'
    )
    # RESOURCE_LOCATION=`echo $REST_RESPONSE | jq -r .resourceLocation`
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

unset url
# Double check uploaded data status
url=$"https://us.atlas.microsoft.com/mapData/metadata/${UDID}?api-version=${apiVersion}&subscription-key=${mapSubscriptionKey}"
# echo "${PHASE_HEADER}: URL=${url}"
REST_RESPONSE=`curl -s -X GET "$url"`
REST_STATUS=`echo $REST_RESPONSE | jq -r .uploadStatus`

if [ "$REST_STATUS" != "Completed" ]; then
  echo "${PHASE_HEADER}: Error 2 $REST_STATUS"
  exit 1
fi

UDID=`echo $REST_RESPONSE | jq -r .udid`

##################################################
# Step 3 : Convert a Drawing package
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/convertpreview
##################################################
PHASE_HEADER="Conversion     "
url="https://us.atlas.microsoft.com/conversions?subscription-key=${mapSubscriptionKey}&api-version=${apiVersion}&udid=${UDID}&inputType=DWG&&outputOntology=facility-2.0"
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

RESP_LOCATION=${HTTP_HEADERS[operation-location]}

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
    read RESOURCE_LOCATION < <(
      curl -sI -X GET "${url}" | 
      awk '/^resource-location/ { RESOURCE_LOCATION = $2 } 
          END { printf("%s\n",RESOURCE_LOCATION) }'
    )
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
url="https://us.atlas.microsoft.com/datasets?api-version=${apiVersion}&conversionId=${CONVERSION_ID}&subscription-key=${mapSubscriptionKey}"
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

RESP_LOCATION=${HTTP_HEADERS[operation-location]}

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

    read RESOURCE_LOCATION < <(
      curl -sI -X GET "${url}" | 
      awk '/^resource-location/ { RESOURCE_LOCATION = $2 } 
          END { printf("%s\n",RESOURCE_LOCATION) }'
    )

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
url="https://us.atlas.microsoft.com/tilesets?api-version=${apiVersion}&datasetID=${DATASET_ID}&subscription-key=${mapSubscriptionKey}"
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

RESP_LOCATION=${HTTP_HEADERS[operation-location]}

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

    read RESOURCE_LOCATION < <(
      curl -sI -X GET "${url}" | 
      awk '/^resource-location/ { RESOURCE_LOCATION = $2 } 
          END { printf("%s\n",RESOURCE_LOCATION) }'
    )

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
url="https://us.atlas.microsoft.com/featurestatesets?api-version=${apiVersion}&datasetId=${DATASET_ID}&subscription-key=${mapSubscriptionKey}"
echo "${PHASE_HEADER}: URL=${url}"
unset HTTP_HEADERS
declare -A HTTP_HEADERS
STATE_SET='{"styles":[{"keyname":"occupied","type":"boolean","rules":[{"true":"#FF0000","false":"#00FF00"}]},{"keyname":"co2","type":"number","rules":[{"range":{"exclusiveMaximum":1000},"color":"#007E16"},{"range":{"minimum":1000,"exclusiveMaximum":1500},"color":"#FFF83E"},{"range":{"minimum":1500},"color":"#FF001A"}]}]}'
# (echo "${STATE_SET}" | jq '.styles[]')

REST_RESPONSE=`curl -s -X POST -H "Content-type: application/json" -d ${STATE_SET} "${url}"`
STATESET_ID=`echo $REST_RESPONSE | jq -r .statesetId`

echo "${PHASE_HEADER}: Stateset ID ${STATESET_ID}"

##################################################
# Step 7 : Delete Maps Data
##################################################
PHASE_HEADER="Cleanup        "
url="https://us.atlas.microsoft.com/mapData?subscription-key=${mapSubscriptionKey}&api-version=${apiVersion}"
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

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${functionsName} --settings MAP_DATASET_ID=${DATASET_ID:?} --query "[?name=='DatasetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: Functions Dataset=  ${temp}"

temp=$(az webapp config appsettings set --resource-group ${resGroup} --name ${functionsName} --settings MAP_STATESET_ID=${STATESET_ID:?} --query "[?name=='StatesetId'].[value]" -o tsv)
echo "${PHASE_HEADER}: Functions Stateset= ${temp}"
