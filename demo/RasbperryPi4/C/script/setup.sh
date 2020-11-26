
#!/bin/bash
#
# Script to : 
#  - Clone Azure IoT SDK C
#  - Compile sample device application with dtmi:iotpnpadt:DigitalTwins:SenseHat;1
#
# Daisuke Nakahara (daisuken@microsoft.com)

print_usage() {
    printf '%s\n' "Usage: $0 [-c] [-r] [-v] [-h]" \
                  '  -c : (C)lean up environment.  Deletes Azure IoT SDK C and other folders and files' \
                  '  -v : (V)erbose' \
                  '  -h : (H)elp menu'
}

Log() {
    if [ $FLAG_VERBOSE = true ]; then
        printf "%s\n" "$1"
    fi
}

LogInfo() {
    printf "%s\n" "$1"
}

LogError() {
    printf "ERROR : %s\n" "$1"
}

# Directories
FILE_SCRIPT=$(realpath -s $0)
DIR_PROJECT=$(dirname $(dirname $FILE_SCRIPT))
DIR_SDK=${DIR_PROJECT}/azure-iot-sdk-c
DIR_CMAKE=${DIR_PROJECT}/cmake

cd ${DIR_PROJECT}

FLAG_VERBOSE=false
FLAG_CLEAN=false

# Check parameters
while getopts 'cvh' flag; do
  case "${flag}" in
    c) FLAG_CLEAN=true ;;
    v) FLAG_VERBOSE=true ;;
    h) print_usage
       exit 1 ;;
    *) print_usage
       exit 1 ;;
  esac
done

Log "Clean   : $FLAG_CLEAN"
Log "Verbose : $FLAG_VERBOSE"

if [ $FLAG_CLEAN = true ]; then
    for folder in "${DIR_SDK}" "${DIR_CMAKE}"
    do
        if [ -d "$folder" ]; then
            LogInfo "Removing $folder"
            rm -r -f "$folder"
        fi
    done
fi

# Clone Azure IoT SDK C
if [ ! -d "${DIR_SDK}" ]; then
    cd "${DIR_PROJECT}"
    if [ $FLAG_VERBOSE = true ]; then
        git_param="--recursive"
    else
        git_param="--recursive --quiet"
    fi    
    LogInfo "Cloning Azure IoT SDK C to "${DIR_SDK}""
    git clone https://github.com/Azure/azure-iot-sdk-c ${git_param}
    [ $? -eq 0 ] || exit $? ]
fi

if [ ! -d "${DIR_SDK}" ]; then
    LogError "Azure IoT SDK C not found"
    exit 1;
fi


# Compile the code
if [ ! -d ${DIR_CMAKE} ]; then
    mkdir ${DIR_CMAKE}
    chmod a+w ${DIR_CMAKE}
fi

if [ ! -d ${DIR_CMAKE}/certs ]; then
    mkdir ${DIR_CMAKE}/certs
fi

cd ${DIR_CMAKE}

cmake .. -Duse_prov_client=ON \
         -Dhsm_type_x509:BOOL=OFF \
         -Dhsm_type_symm_key:BOOL=ON \
         -Dskip_samples:BOOL=OFF \
         -Duse_amqp:BOOL=OFF \
         -Dbuild_service_client:BOOL=OFF \
         -Duse_http=:BOOL=OFF \
         -Duse_amqp=:BOOL=OFF \
         -Dbuild_provisioning_service_client=:BOOL=OFF \
         -Drun_e2e_tests=OFF

cmake --build . --config Release -- --silent
LogInfo " "
LogInfo "=========== Completed ========================================================"
LogInfo "Configure parameters in run.sh then, start run.sh script to run the app."
