#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)
project (AzureSphereDemo C)

azsphere_configure_tools(TOOLS_REVISION "21.10")
azsphere_configure_api(TARGET_API_SET "11")

# Create executable
add_executable (${PROJECT_NAME} main.c "HardwareDefinitions/avnet_mt3620_sk_rev2/inc/hw/demo_appliance.h")
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "HardwareDefinitions/avnet_mt3620_sk_rev2" TARGET_DEFINITION "demo_appliance.json")

azsphere_target_add_image_package(${PROJECT_NAME})
