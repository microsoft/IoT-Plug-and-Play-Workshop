
#
# Script to : 
#  - Set environment variables
#  - REGISTRATION_ID : Subject Name (SN) used to generate X.509 certificate
#  - DPS_IDSCOPE : DPS ID Scope provided by Certification Portal
#  - PNP_MODEL_ID : IoT Plug and Play Digital Twin Model ID
#
# Daisuke Nakahara (daisuken@microsoft.com)
#

# Directories
FILE_SCRIPT=$(realpath -s $0)
DIR_PROJECT=$(dirname $(dirname $FILE_SCRIPT))
DIR_CMAKE=${DIR_PROJECT}/cmake

# Set IoT Plug and Play Model ID
export PNP_MODEL_ID='dtmi:iotpnpadt:DigitalTwins:SenseHat;1'

# Set IoT Hub Connection String
export IOTHUB_CS=''

cd ${DIR_CMAKE}
./RP4SenseHat

