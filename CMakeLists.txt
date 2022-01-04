#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)
project (AzureSphereDemo C)

azsphere_configure_tools(TOOLS_REVISION "21.10")
azsphere_configure_api(TARGET_API_SET "11")

# set(SPHERE_INCLUDE_DIR "C:/Program Files (x86)/Microsoft Azure Sphere SDK/Sysroots/11/usr/include")

# Create executable
add_executable (${PROJECT_NAME} main.c
				i2c.c
				lsm6dso_reg.c
				lps22hh_reg.c
				sd1306.c 
				eventloop_timer_utilities.c
				oled.c
				click_airquality7.c click_heartrate4.c click_eth.c click_zigbee.c click_gps4.c click_nfc4.c
				avnet_ble51.c azure_iothub.c)
				# "HardwareDefinitions/avnet_mt3620_sk_rev2/inc/hw/demo_appliance.h" "lps22hh_reg.h" "lsm6dso_reg.h" "i2c.h" "build_options.h" "oled.h" "sd1306.h" "eventloop_timer_utilities.h" "exit_codes.h")

# target_include_directories(${PROJECT_NAME} PRIVATE ${SPHERE_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "HardwareDefinitions/avnet_mt3620_sk_rev2" TARGET_DEFINITION "demo_appliance.json")

azsphere_target_add_image_package(${PROJECT_NAME})
