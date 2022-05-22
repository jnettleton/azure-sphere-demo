#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

# Modified with parts from https://github.com/Azure/azure-sphere-samples/blob/main/Samples/AzureIoT/CMakeLists.txt

cmake_minimum_required (VERSION 3.10)
project (AzureSphereDemo C)

azsphere_configure_tools(TOOLS_REVISION "21.10")
azsphere_configure_api(TARGET_API_SET "12")

# set(SPHERE_INCLUDE_DIR "C:/Program Files (x86)/Microsoft Azure Sphere SDK/Sysroots/11/usr/include")

# Create executable
add_executable (${PROJECT_NAME} main.c
				i2c.c
				lsm6dso_reg.c
				lps22hh_reg.c
				sd1306.c
				# eventloop_timer_utilities.c
				oled.c
				click_airquality7.c click_heartrate4.c click_eth.c click_zigbee.c click_gps4.c click_nfc4.c
				avnet_ble51.c eventloop_timer_utilities.c)
        #        azure_iothub.c)
				# "HardwareDefinitions/avnet_mt3620_sk_rev2/inc/hw/demo_appliance.h" "lps22hh_reg.h" "lsm6dso_reg.h" "i2c.h" "build_options.h" "oled.h" "sd1306.h" "eventloop_timer_utilities.h" "exit_codes.h")

add_subdirectory(AzureIoT/common)
add_subdirectory(AzureIoT/IotHub)

# target_include_directories(${PROJECT_NAME} PRIVATE ${SPHERE_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC
													 ${CMAKE_SOURCE_DIR}/AzureIoT/common
                           ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot
                           ${AZURE_SPHERE_API_SET_DIR}/usr/include/azure_prov_client
                           ${AZURE_SPHERE_API_SET_DIR}/usr/include/azure_c_shared_utility)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries (${PROJECT_NAME} m azureiot applibs pthread gcc_s c)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "HardwareDefinitions/avnet_mt3620_sk_rev2" TARGET_DEFINITION "demo_appliance.json")

azsphere_target_add_image_package(${PROJECT_NAME})
