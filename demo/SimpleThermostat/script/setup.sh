#!/bin/bash
#
# Script to : 
#  - Clone Azure IoT SDK C
#  - Generate self-signed X.509 certificate and device key
#  - Copy certificates to ./cmake/cert
#  - Compile sample device application for dtmi:com:Example:Thermostat;1
#
# Daisuke Nakahara (daisuken@microsoft.com)
#

print_usage() {
    printf '%s\n' "Usage: $0 [-c] [-r] [-v] [-h]" \
                  '  -c : (C)lean up environment.  Deletes Azure IoT SDK C and other folders and files' \
                  '  -r : (R)e-create new X509 certificates' \
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
DIR_CERTIFICATE=${DIR_PROJECT}/certificate
DIR_CMAKE=${DIR_PROJECT}/cmake

# Files
FILE_ROOTCA=${DIR_CERTIFICATE}/certs/azure-iot-test-only.root.ca.cert.pem
FILE_INTERMEDIATE=${DIR_CERTIFICATE}/certs/azure-iot-test-only.intermediate.cert.pem
FILE_DEVICE_CER=${DIR_CERTIFICATE}/certs/new-device.cert.pem
FILE_DEVICE_KEY=${DIR_CERTIFICATE}/private/new-device.key.pem

cd ${DIR_PROJECT}

# Host Name becomes Common Name of the certificate
REGISTRATION_ID=$(hostname)

# Certificate and key file names
X509_DEVICE_KEY="device.key.pem"
X509_DEVICE_CER="device.cer.pem"

# Flags
FLAG_X509=false
FLAG_VERBOSE=false
FLAG_CLEAN=false

# Check parameters
while getopts 'crvh' flag; do
  case "${flag}" in
    c) FLAG_CLEAN=true ;;
    r) FLAG_X509=true ;;
    v) FLAG_VERBOSE=true ;;
    h) print_usage
       exit 1 ;;
    *) print_usage
       exit 1 ;;
  esac
done

Log "Force   : $FLAG_X509"
Log "Clean   : $FLAG_CLEAN"
Log "Verbose : $FLAG_VERBOSE"

if [ $FLAG_CLEAN = true ]; then
    for folder in "${DIR_SDK}" "${DIR_CERTIFICATE}" "${DIR_CMAKE}"
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

# Check if we just want to re-creata
LogInfo "Checking X.509 certificates"
if [ $FLAG_X509 = true ]; then
    X509_FOUND=false
    if [ -d "${DIR_CERTIFICATE}" ]; then
        Log "Removing ${DIR_CERTIFICATE}"
        rm -r -f "${DIR_CERTIFICATE}"
    fi
else
    X509_FOUND=true
    for certFile in "${DIR_TARGET}/${X509_DEVICE_KEY}" "${DIR_TARGET}/${X509_DEVICE_CER}"
    do
        if [ ! -e "${certFile}" ]; then
            Log "${certFile} not found"
            X509_FOUND=false
            break
        fi
    done
fi

# Generate X.509 certificates
if [ $X509_FOUND = true ] ; then
    LogInfo "X509 certificates found"
else
    LogInfo "Processing X.509 certificates"

    if [ ! -d "${DIR_CERTIFICATE}" ]; then
        Log "Creating ${DIR_CERTIFICATE}"
        mkdir ${DIR_CERTIFICATE}
    fi

    # Copy scripts from SDK
    cd ${DIR_CERTIFICATE}
    cp ${DIR_SDK}/tools/CACertificates/*.sh ${DIR_CERTIFICATE}
    cp ${DIR_SDK}/tools/CACertificates/*.cnf ${DIR_CERTIFICATE}
    chmod a+x *.sh

    # Generate Root CA and Intermediate certificate
    LogInfo "Generating Root CA and Intermediate certificate"
    ./certGen.sh create_root_and_intermediate > /dev/null 2>&1
    [ $? -eq 0 ] || exit $? ]

    if [ ! -e "${FILE_ROOTCA}" ]; then
        LogError "Root CA not found : (${FILE_ROOTCA})"
        exit 1
    else
        Log "Root CA found                  : (${FILE_ROOTCA})"
    fi

    if [ ! -e "${FILE_INTERMEDIATE}" ]; then
        LogError "Intermediate Certificate not found : (${FILE_INTERMEDIATE})"
        exit 1
    else
        Log "Intermediate Certificate found : (${FILE_INTERMEDIATE})"
    fi

    # Generate device certificate using host name
    LogInfo "Generating Device Certificate"
    ./certGen.sh create_device_certificate ${REGISTRATION_ID} > /dev/null 2>&1
    [ $? -eq 0 ] || exit $? ]

    if [ ! -e "${FILE_DEVICE_CER}" ]; then
        LogError "Device Certificate not found : (${FILE_DEVICE_CER})"
        exit 1
    else
        Log "Deivce Certificate found       : (${FILE_DEVICE_CER})"
    fi

    if [ ! -e "${FILE_DEVICE_KEY}" ]; then
        LogError "Device Key not found : (${FILE_DEVICE_KEY})"
        exit 1
    else
        Log "Deivce Key found               : (${FILE_DEVICE_KEY})"
    fi
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

# Copy certificates
cp "${FILE_ROOTCA}" ${DIR_CMAKE}/certs/rootCA.pem
cp "${FILE_INTERMEDIATE}" ${DIR_CMAKE}/certs/intermediate.pem
cp "${FILE_DEVICE_CER}" ${DIR_CMAKE}/certs/${REGISTRATION_ID}.cer.pem
cp "${FILE_DEVICE_KEY}" ${DIR_CMAKE}/certs/${REGISTRATION_ID}.key.pem
chmod a+w ${DIR_CMAKE}/certs/*.pem

if [ ! -e "${DIR_CMAKE}/certs/rootCA.pem" ]; then
    LogError "Root CA not found : (${DIR_CMAKE}/certs/rootCA.pem)"
    exit 1
fi

if [ ! -e "${DIR_CMAKE}/certs/intermediate.pem" ]; then
    LogError "intermediate certificate not found : (${DIR_CMAKE}/certs/intermediate.pem)"
    exit 1
fi

if [ ! -e "${DIR_CMAKE}/certs/${REGISTRATION_ID}.cer.pem" ]; then
    LogError "Device certificate not found : (${DIR_CMAKE}/certs/${REGISTRATION_ID}.cer.pem)"
    exit 1
fi

if [ ! -e "${DIR_CMAKE}/certs/${REGISTRATION_ID}.key.pem" ]; then
    LogError "Device Key not found : (${DIR_CMAKE}/certs/${REGISTRATION_ID}.key.pem)"
    exit 1
fi

LogInfo "Certificates generated"
LogInfo "  Root CA Certificate      : ${DIR_CMAKE}/certs/rootCA.pem"
LogInfo "  Intermediate Certificate : ${DIR_CMAKE}/certs/intermediate.pem"
LogInfo "  Device Certificate       : ${DIR_CMAKE}/certs/${REGISTRATION_ID}.cer.pem"
LogInfo "  Device Key               : ${DIR_CMAKE}/certs/${REGISTRATION_ID}.key.pem"

cmake .. -Duse_prov_client=ON \
         -Dhsm_type_x509:BOOL=ON \
         -Dhsm_type_symm_key:BOOL=ON \
         -Dhsm_custom_lib=custom_hsm \
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
LogInfo "Upload ${DIR_CMAKE}/certs/${REGISTRATION_ID}.cer.pem to Certification Portal."
LogInfo "The simulator will provision as ${REGISTRATION_ID}."
LogInfo "Configure parameters in run.sh then, start run.sh script to run the simulator."
