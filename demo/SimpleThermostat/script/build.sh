#
# Script to : 
#  - Build the simulator with Azure IoT SDK C
#
# Upload ./cmake/new-device.cer.pem to set up X.509 certificate authentication method in the Certification Portal
# 
# Daisuke Nakahara (daisuken@microsoft.com)
#

# Directories
FILE_SCRIPT=$(realpath -s $0)
DIR_PROJECT=$(dirname $(dirname $FILE_SCRIPT))
DIR_SDK=${DIR_PROJECT}/azure-iot-sdk-c
DIR_CMAKE=${DIR_PROJECT}/cmake

# Clone Azure IoT SDK C
if [ ! -d "${DIR_SDK}" ]; then
    cd "${DIR_PROJECT}"
    git clone https://github.com/Azure/azure-iot-sdk-c --recursive
    [ $? -eq 0 ] || exit $? ]
fi

if [ ! -d "${DIR_CMAKE}" ]; then
    mkdir "${DIR_CMAKE}"
fi

cd ${DIR_CMAKE}

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
