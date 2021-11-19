#ifndef AIRQUALITY7_H
#define AIRQUALITY7_H

#include <stdint.h>
#include <applibs/i2c.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "i2c.h"


#define AIRQUALITY7_RETVAL  uint8_t
#define airquality7_err_t   uint8_t

#define AIRQUALITY7_OK           0x00
#define AIRQUALITY7_INIT_ERROR   0xFF

#define AIRQUALITY7_CMD_SET_PPMCO2    0x08
#define AIRQUALITY7_CMD_GET_STATUS    0x0C
#define AIRQUALITY7_CMD_GET_REVISION  0x0D
#define AIRQUALITY7_CMD_GET_R0        0x10

#define AIRQUALITY7_DEV_ADDR  0x70

#define AIRQUALITY7_ERR_OK    0x00
#define AIRQUALITY7_ERR_CRC   0x01
#define AIRQUALITY7_ERR_WRITE 0x02
#define AIRQUALITY7_ERR_READ  0x03

#define AIRQUALITY7_NULL      0x00
#define AIRQUALITY7_DUMMY     0x00

 
#ifdef __cplusplus
extern "C"{
#endif

extern bool airquality7_found;

/**
 * @brief Generic write function.
 *
 * @param data_buf     Data buf to be written.
 *
 * @description This function writes data to the desired register.
 */
int32_t airquality7_generic_write(uint8_t *data_buf);

/**
 * @brief Generic read function.
 * 
 * @param data_buf     Output data buf
 *
 * @description This function reads data from the desired register.
 */
int32_t airquality7_generic_read(uint8_t *data_buf);

/**
 * @brief Set CO2 [ppm] function.
 *
 * @param ppmco2_value  4 bytes of CO2 data to be written in ppm.
 *
 * @description This function is used to send the ppmCO2 value given by an 
 * external analyzer to the Air quality 7 click in order to recalibrate its 
 * outputs.
 */
void airquality7_set_ppmco2(uint8_t *ppmco2_value);

/**
 * @brief Get Status function.
 *
 * @param ctx          Click object.
 * @param tvoc_ppb     tVOC output data in ppb.
 * @param co2_ppm      CO2 output data in ppm.
 * @param res_val_ohm  Resistor value output data in Ohms.
 * @param err_byte     Error status output data.
 *
 * @returns 0 - Ok,
 *          1 - CRC error.
 *
 * @description This function is used to read the Air quality 7 click status 
 * coded on tVOC data byte [0 - 1000 ppb], CO2 data byte [400 - 2000 ppm], 
 * resistor value data 3 bytes [Ohm], error status byte and CRC byte.
 */
airquality7_err_t airquality7_get_status(uint16_t *tvoc_ppb, 
                                         uint16_t *co2_ppm, 
                                         uint32_t *res_val_ohm, 
                                         uint8_t *err_byte );

/**
 * @brief Get Revision function.
 *
 * @param ctx          Click object.
 * @param year         Revision year output data.
 * @param month        Revision month output data.
 * @param day          Revision day output data.
 * @param ascii_code   Revision ASCII code output data.
 *
 * @returns 0 - Ok,
 *          1 - CRC error.
 *
 * @description This function will return the revision code of the module coded
 * on year data byte, month data byte, day data byte, ASCII code data byte
 * for a charter and CRC byte.
 */
airquality7_err_t airquality7_get_revision(uint8_t *year, 
                                           uint8_t *month, 
                                           uint8_t *day, 
                                           uint8_t *ascii_code );

/**
 * @brief Get R0 Calibration function.
 *
 * @param ctx          Click object.
 * @param r0_kohm      R0 calibration output data in kOhms.
 *
 * @returns 0 - Ok,
 *          1 - CRC error.
 *
 * @description This function is used to read the R0 (calibration) value in
 * [kOhms] coded on 2 data bytes and CRC byte.
 */
airquality7_err_t airquality7_get_r0_calib(uint16_t *r0_kohm);

#ifdef __cplusplus
}
#endif
#endif  // _AIRQUALITY7_H_
