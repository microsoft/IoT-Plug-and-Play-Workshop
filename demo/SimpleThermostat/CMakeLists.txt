#////////////////////////////////////////////////////////////////////////////
#//
#// CMake to build Simple Thermostat Demo
#//
#////////////////////////////////////////////////////////////////////////////

#////////////////////////////////////////////////////////////////////////////
#// Project setup
#////////////////////////////////////////////////////////////////////////////
cmake_minimum_required(VERSION 2.8.11)

project(SimpleThermostat)

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
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
endif()

set(pnp-app_h_files
    ./iothub/dps.h
    ./iothub/callback.h
    ./iothub/d2c.h
    ./iothub/deviceMethod.h
    ./iothub/deviceTwin.h
    ./iothub/iothub_op.h
    ./main.h
)

set(pnp-app_c_files
    ./iothub/dps.c
    ./iothub/callback.c
    ./iothub/d2c.c
    ./iothub/deviceMethod.c
    ./iothub/deviceTwin.c
    ./iothub/iothub_op.c
    ./main.c
)

add_subdirectory(./azure-iot-sdk-c)
add_subdirectory(./custom_hsm)
include_directories(.)
include_directories(${azure_iot_sdks_SOURCE_DIR}/deps/parson)
include_directories(${MQTT_INC_FOLDER})
include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${UMOCK_C_INC_FOLDER})
include_directories(${MACRO_UTILS_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${DEV_AUTH_MODULES_CLIENT_INC_FOLDER})

add_executable(SimpleThermostat ${pnp-app_c_files} ${pnp-app_h_files})

target_link_libraries(SimpleThermostat 
                      parson
                      prov_mqtt_transport
                      iothub_client
                      iothub_client_mqtt_transport
                      prov_device_ll_client
                      prov_mqtt_transport
                      prov_auth_client
                      aziotsharedutil
                      umqtt
                      )
