#pragma once

#include <stdbool.h>
#include <applibs/uart.h>
#include <hw/demo_appliance.h>

#include "build_options.h"

#include "click_zigbee_types.h"
#include "click_zigbee_config.h"
//#include "click-zigbee_other_peripherals.h"

extern bool zigbee_found;

extern void zigbee_open(void);
extern size_t zigbee_read(char* buffer, size_t size);
extern size_t zigbee_write(const char* buffer, size_t size);
