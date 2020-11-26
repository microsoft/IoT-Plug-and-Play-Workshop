#////////////////////////////////////////////////////////////////////////////
#//
#// CMake to build Raspberry Pi + SenseHat + Azure IoT C SDK
#//
#////////////////////////////////////////////////////////////////////////////

#////////////////////////////////////////////////////////////////////////////
#// Project setup
#////////////////////////////////////////////////////////////////////////////
cmake_minimum_required(VERSION 2.8.11)
project(RP4SenseHat)

option(use_rtimulib "Use RTIMULIB for LSM9DS1" ON)
option(use_telemetrytrace "Enable additional trace for telemetry and sensor data" OFF)
option(adt_demo "Speical Build for ADT Demo" OFF)
# option(USE_PYTHONLIB "Use Python for LSM9DS1" OFF)

message("-- use_rtimulib       : " ${use_rtimulib})
message("-- use_telemetrytrace : " ${use_telemetrytrace})
message("-- adt_demo           : " ${adt_demo})

#Compile As C99
if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set (CMAKE_C_FLAGS "--std=c99 /DWIN32 /D_WINDOWS /W3")
        if (NOT IN_OPENWRT)
            set (CMAKE_CXX_FLAGS "--std=c++11 /DWIN32 /D_WINDOWS /W3 /GR /EHsc")
        endif()
    endif()
else()
    set (CMAKE_C_STANDARD 99)
    set (CMAKE_CXX_STANDARD 11)
endif()

#////////////////////////////////////////////////////////////////////////////
#// Build Type
#////////////////////////////////////////////////////////////////////////////
if(NOT CMAKE_CONFIGURATION_TYPES)
	if(DEFINED CMAKE_BUILD_TYPE)
		message("-- Set: " ${CMAKE_BUILD_TYPE})
		set(CMAKE_CONFIGURATION_TYPES Debug)
	else()
		message("-- Set: " Release)
		set(CMAKE_CONFIGURATION_TYPES Release)
	endif()
endif()

message("-- Configuration types: " ${CMAKE_CONFIGURATION_TYPES})

if(CMAKE_CONFIGURATION_TYPES MATCHES Debug)
    ADD_DEFINITIONS(-DDEBUG)
    if (${use_telemetrytrace})
        ADD_DEFINITIONS(-DDEBUG_TELEMETRY)
    endif()
endif()

if(${adt_demo})
    ADD_DEFINITIONS(-DADT_DEMO)
endif()


if(${use_rtimulib})
    ADD_DEFINITIONS(-DHAVE_RTIMULIB_H)
endif()

# IF(USE_PYTHONLIB)
#    ADD_DEFINITIONS(-DUSE_PYTHON_EXTENSION)
#    set(PYTHON_LIBRARY "/usr/lib/python3.7")
#    set(PYTHON_INCLUDE_DIR "/usr/include/python3.7")

#    set(PythonLibs_FIND_VERSION "3.7")
#    find_package(PythonLibs REQUIRED)
#    include_directories(${PYTHON_INCLUDE_DIRS})
#    include_directories(
#        /usr/include/python3.7m
#        /usr/include/arm-linux-gnueabihf/python3.7m
#    )
#ENDIF()

set(pnp-app_h_files
    ./senseHat/led.h
    ./senseHat/lsm9ds1.h
    ./senseHat/sensehat.h
    ./senseHat/hts221.h
    ./senseHat/lps25h.h
    ./senseHat/util.h
    ./iothub/dps.h
    ./iothub/callback.h
    ./iothub/d2c.h
    ./iothub/c2d.h
    ./iothub/deviceMethod.h
    ./iothub/deviceTwin.h
    ./iothub/iothub_op.h
    ./deviceInformation/deviceInformation.h
    ./main.h
)

set(pnp-app_c_files
    ./senseHat/led.cpp
    ./senseHat/lsm9ds1.cpp
    ./senseHat/sensehat.c
    ./senseHat/hts221.c
    ./senseHat/lps25h.c
    ./senseHat/util.c
    ./iothub/dps.c
    ./iothub/callback.c
    ./iothub/d2c.c
    ./iothub/c2d.c
    ./iothub/deviceMethod.c
    ./iothub/deviceTwin.c
    ./iothub/iothub_op.c
    ./deviceInformation/deviceInformation.c
    ./main.c
)

add_subdirectory(./azure-iot-sdk-c)
include_directories(.)
include_directories(${azure_iot_sdks_SOURCE_DIR}/deps/parson)
include_directories(${MQTT_INC_FOLDER})
include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${UMOCK_C_INC_FOLDER})
include_directories(${MACRO_UTILS_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${DIGITALTWIN_CLIENT_INC_FOLDER})
include_directories(${DEV_AUTH_MODULES_CLIENT_INC_FOLDER})

add_executable(RP4SenseHat ${pnp-app_c_files} ${pnp-app_h_files})

if(${use_prov_client})
    target_link_libraries(RP4SenseHat 
                          prov_mqtt_transport
                          iothub_client_mqtt_transport
                          prov_device_ll_client
                          prov_auth_client)
endif()

target_link_libraries(
    RP4SenseHat
    iothub_client
    aziotsharedutil
    i2c
#    python3.7m
    RTIMULib
)

