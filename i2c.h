#pragma once

#include <applibs/i2c.h>
#include <applibs/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <hw/demo_appliance.h>

#include "build_options.h"
#include "lsm6dso_reg.h"
#include "lps22hh_reg.h"
#include "oled.h"
#ifdef CLICK_AIRQUALITY7
#include "airquality7.h"
#endif

#define LSM6DSO_ADDRESS 0x6A // I2C Address

typedef struct {
    float x;
    float y;
    float z;
} AngularRateDegreesPerSecond;

typedef struct {
    float x;
    float y;
    float z;
} AccelerationgForce;

extern bool lps22hhDetected;
extern AccelerationgForce acceleration_g;
extern AngularRateDegreesPerSecond angular_rate_dps;
extern float lsm6dso_temperature;
extern float pressure_kPa;
extern float lps22hh_temperature;
extern int i2cFd;

#ifdef CLICK_AIRQUALITY7
extern uint16_t airquality7_tvoc_ppb;
extern uint16_t airquality7_co2_ppm;
extern uint32_t airquality7_res_val_ohm;

extern uint8_t airquality7_rev_year;
extern uint8_t airquality7_rev_month;
extern uint8_t airquality7_rev_day;
extern uint8_t airquality7_rev_ascii_code;

uint8_t lp_get_click_air_quality(void);
#endif

void lp_imu_initialize(void);
void lp_imu_close(void);
float lp_get_temperature(void);
float lp_get_pressure(void);
float lp_get_temperature_lps22h(void); // get_temperature() from lsm6dso is faster
void lp_calibrate_angular_rate(void);
AngularRateDegreesPerSecond lp_get_angular_rate(void);
AccelerationgForce lp_get_acceleration(void);
