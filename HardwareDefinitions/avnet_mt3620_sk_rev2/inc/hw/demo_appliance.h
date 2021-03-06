// This file defines the mapping from the Avnet MT3620 Starter Kit (SK) to the
// 'sample hardware' abstraction used by the samples at https://github.com/Azure/azure-sphere-samples.
// Some peripherals are on-board on the Avnet MT3620 SK, while other peripherals must be attached externally if needed.
// See https://aka.ms/AzureSphereHardwareDefinitions for more information on how to use hardware abstractions,
// to enable apps to work across multiple hardware variants.

// This file is autogenerated from ../../demo_appliance.json.  Do not edit it directly.

#pragma once
#include "avnet_mt3620_sk_rev2.h"

// MT3620 SK: User Button A.
#define SAMPLE_BUTTON_1 AVNET_MT3620_SK_USER_BUTTON_A

// MT3620 SK: User Button B.
#define SAMPLE_BUTTON_2 AVNET_MT3620_SK_USER_BUTTON_B

// MT3620 SK: User LED RED Channel.
#define SAMPLE_RGBLED_RED AVNET_MT3620_SK_USER_LED_RED

// MT3620 SK: User LED GREEN Channel.
#define SAMPLE_RGBLED_GREEN AVNET_MT3620_SK_USER_LED_GREEN

// MT3620 SK: User LED BLUE Channel.
#define SAMPLE_RGBLED_BLUE AVNET_MT3620_SK_USER_LED_BLUE

// MT3620 SK: LSM6DSO accelerometer.
#define SAMPLE_LSM6DSO_I2C AVNET_MT3620_SK_ISU2_I2C

// MT3620 SK: LSM6DSO accelerometer.
#define SAMPLE_LSM6DSO_SCL AVNET_MT3620_SK_GPIO32

// MT3620 SK: LSM6DSO accelerometer.
#define SAMPLE_LSM6DSO_SDA AVNET_MT3620_SK_GPIO33

// MT3620 SK: LPS22HH pressure sensor.
#define SAMPLE_LPS22HH_I2C AVNET_MT3620_SK_ISU2_I2C

// MT3620 SK: LPS22HH pressure sensor.
#define SAMPLE_LPS22HH_SCL AVNET_MT3620_SK_GPIO32

// MT3620 SK: LPS22HH pressure sensor.
#define SAMPLE_LPS22HH_SDA AVNET_MT3620_SK_GPIO33

// MT3620 SK: Ambient light sensor.
#define SAMPLE_AMBIENT_LIGHT_ADC AVNET_MT3620_SK_ADC_CONTROLLER0

// MT3620 SK: OLED I2C.
#define SAMPLE_OLED_I2C AVNET_MT3620_SK_ISU2_I2C

// MT3620 SK: CLICK 1 ISU2.
#define SAMPLE_CLICK1_I2C AVNET_MT3620_SK_ISU2_I2C

// MT3620 SK: CLICK 2 ISU2.
#define SAMPLE_CLICK2_I2C AVNET_MT3620_SK_ISU2_I2C

// ISU0 UART is exposed on CLICK1
#define SAMPLE_CLICK1_UART AVNET_MT3620_SK_ISU0_UART

