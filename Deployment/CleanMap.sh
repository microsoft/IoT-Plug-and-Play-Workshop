#!/bin/bash
mapSubscriptionKey=${1:?}

##################################################
# Delete Maps Data
# https://docs.microsoft.com/en-us/rest/api/maps/data/listpreview
##################################################
PHASE_HEADER="Maps Data      "
echo "${PHASE_HEADER}: Searching...."
url="https://us.atlas.microsoft.com/mapData?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`
REST_ERROR=`echo "${LIST_RESP}" | jq '.error | length' `

if [ ${REST_ERROR} == 0 ]; then

  for row in $(echo "${LIST_RESP}" | jq -r '.mapDataList[] | @base64'); do
    _jq() {
        echo ${row} | base64 --decode | jq -r ${1}
    }
    DATA_ID=$(_jq '.udid')
    url="$(_jq '.location')&subscription-key=${mapSubscriptionKey}"
    echo "${PHASE_HEADER}: Deleting UDID=${DATA_ID}"
    curl -s -X DELETE  "${url}"
  done
else
  echo "${PHASE_HEADER}: ERROR : " `echo "${LIST_RESP}" | jq '.error'`
fi

##################################################
# Delete Feature State
# https://docs.microsoft.com/en-us/rest/api/maps/featurestate/liststatesetpreview
##################################################
PHASE_HEADER="Feature State  "
echo "${PHASE_HEADER}: Searching...."
url="https://us.atlas.microsoft.com/featureState/stateset?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`
REST_ERROR=`echo "${LIST_RESP}" | jq '.error | length' `

if [ ${REST_ERROR} == 0 ]; then
  for row in $(echo "${LIST_RESP}" | jq -r '.statesetDetailList[] | @base64'); do
    _jq() {
        echo ${row} | base64 --decode | jq -r ${1}
    }
    DATA_ID=$(_jq '.statesetId')
    url="https://us.atlas.microsoft.com/featureState/stateset/${DATA_ID}?api-version=1.0&subscription-key=${mapSubscriptionKey}"
    echo "${PHASE_HEADER}: Deleting Stateset=${DATA_ID}"
    curl -s -X DELETE  "${url}"
  done
else
  echo "${PHASE_HEADER}: ERROR : " `echo "${LIST_RESP}" | jq '.error'`
fi

##################################################
# Delete Tileset
# https://docs.microsoft.com/en-us/rest/api/maps/tileset/listpreview
##################################################
PHASE_HEADER="Tileset        "
echo "${PHASE_HEADER}: Searching...."
url="https://us.atlas.microsoft.com/tileset?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`
REST_ERROR=`echo "${LIST_RESP}" | jq '.error | length' `

if [ ${REST_ERROR} == 0 ]; then
  for row in $(echo "${LIST_RESP}" | jq -r '.tilesets[] | @base64'); do
    _jq() {
        echo ${row} | base64 --decode | jq -r ${1}
    }
    DATA_ID=$(_jq '.tilesetId')
    url="https://us.atlas.microsoft.com/tileset/${DATA_ID}?api-version=1.0&subscription-key=${mapSubscriptionKey}"
    echo "${PHASE_HEADER}: Deleting Tileset=${DATA_ID}"
    curl -s -X DELETE  "${url}"
  done
else
  echo "${PHASE_HEADER}: ERROR : " `echo "${LIST_RESP}" | jq '.error'`
fi

##################################################
# Delete dataset
# https://docs.microsoft.com/en-us/rest/api/maps/dataset/listpreview
##################################################
PHASE_HEADER="Dataset        "
echo "${PHASE_HEADER}: Searching...."
url="https://us.atlas.microsoft.com/dataset?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`
REST_ERROR=`echo "${LIST_RESP}" | jq '.error | length' `

if [ ${REST_ERROR} == 0 ]; then
  for row in $(echo "${LIST_RESP}" | jq -r '.datasets[] | @base64'); do
    _jq() {
        echo ${row} | base64 --decode | jq -r ${1}
    }
    DATA_ID=$(_jq '.datasetId')
    url="https://us.atlas.microsoft.com/dataset/${DATA_ID}?api-version=1.0&subscription-key=${mapSubscriptionKey}"
    echo "${PHASE_HEADER}: Deleting Dataset=${DATA_ID}"
    curl -s -X DELETE  "${url}"
  done
else
  echo "${PHASE_HEADER}: ERROR : " `echo "${LIST_RESP}" | jq '.error'`
fi

##################################################
# Conversion dataset
# https://docs.microsoft.com/en-us/rest/api/maps/conversion/listpreview
##################################################
PHASE_HEADER="Conversion     "
echo "${PHASE_HEADER}: Searching...."
url="https://us.atlas.microsoft.com/conversion?subscription-key=${mapSubscriptionKey}&api-version=1.0"
LIST_RESP=`curl -s -X GET "${url}"`
REST_ERROR=`echo "${LIST_RESP}" | jq '.error | length' `

if [ ${REST_ERROR} == 0 ]; then
  for row in $(echo "${LIST_RESP}" | jq -r '.conversions[] | @base64'); do
    _jq() {
        echo ${row} | base64 --decode | jq -r ${1}
    }
    DATA_ID=$(_jq '.conversionId')
    url="https://us.atlas.microsoft.com/conversion/${DATA_ID}?api-version=1.0&subscription-key=${mapSubscriptionKey}"
    echo "${PHASE_HEADER}: Deleting Conversion=${DATA_ID}"
    curl -s -X DELETE  "${url}"
  done
else
  echo "${PHASE_HEADER}: ERROR : " `echo "${LIST_RESP}" | jq '.error'`
fi
