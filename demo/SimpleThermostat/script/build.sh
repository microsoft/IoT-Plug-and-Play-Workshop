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
DIR_CMAKE=${DIR_PROJECT}/cmake

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
