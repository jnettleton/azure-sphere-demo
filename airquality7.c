#include "airquality7.h"

static void airquality7_communication_delay( void );
static airquality7_err_t airquality7_get_crc( uint8_t *data_in, uint8_t data_size );

uint8_t airquality7_addr = AIRQUALITY7_DEV_ADDR;

int32_t airquality7_generic_write(uint8_t *data_buf)
{  
    airquality7_communication_delay();
    return I2CMaster_Write(i2cIsu2Fd, airquality7_addr, data_buf, 6);
}

int32_t airquality7_generic_read(uint8_t *data_buf)
{
    airquality7_communication_delay();
    return I2CMaster_Write(i2cIsu2Fd, airquality7_addr, data_buf, 7);
}

int32_t airquality7_generic_write_then_read(uint8_t* write_buf, uint8_t* read_buf)
{
    return I2CMaster_WriteThenRead(i2cIsu2Fd, airquality7_addr, write_buf, 6, read_buf, 7);
}

void airquality7_set_ppmco2(uint8_t *ppmco2_value)
{
    uint8_t tmp_data[6] = { AIRQUALITY7_DUMMY };
    uint8_t cnt;
    
    for (cnt = 1; cnt <= 4; cnt++)
    {
        tmp_data[cnt] = *ppmco2_value;
        ppmco2_value++;
    }
    
    tmp_data[0] = AIRQUALITY7_CMD_SET_PPMCO2;
    tmp_data[5] = airquality7_get_crc(tmp_data, 5);
    airquality7_generic_write(tmp_data);
}

airquality7_err_t airquality7_get_status(uint16_t *tvoc_ppb, 
										 uint16_t *co2_ppm, 
                                         uint32_t *res_val_ohm, 
                                         uint8_t *err_byte)
{
#if true2
    uint8_t tmp_data[7] = { AIRQUALITY7_DUMMY };
    uint8_t crc_calc = AIRQUALITY7_DUMMY;

    tmp_data[0] = AIRQUALITY7_CMD_GET_STATUS;
    tmp_data[5] = airquality7_get_crc(tmp_data, 5);

    int32_t exitCode = airquality7_generic_write(tmp_data);
    if (exitCode != 0)
    {
        Log_Debug("ERROR: AirQuality7 get status write: %s (%d)\n", strerror(errno), errno);
        return AIRQUALITY7_ERR_WRITE;
    }

    exitCode = airquality7_generic_read(tmp_data);
    if (exitCode != 0)
    {
        Log_Debug("ERROR: AirQuality7 get status read: %s (%d)\n", strerror(errno), errno);
        return AIRQUALITY7_ERR_READ;
    }
#else
    uint8_t tmp_data[7] = { AIRQUALITY7_DUMMY };
    uint8_t crc_calc = AIRQUALITY7_DUMMY;

    uint8_t write_data[6] = { AIRQUALITY7_DUMMY };
    write_data[0] = AIRQUALITY7_CMD_GET_STATUS;
    write_data[5] = airquality7_get_crc(tmp_data, 5);

    int32_t exitCode = airquality7_generic_write_then_read(write_data, tmp_data);
    if (exitCode != 0)
    {
        Log_Debug("ERROR: AirQuality7 get status: %s (%d)\n", strerror(errno), errno);
        return AIRQUALITY7_ERR_READ;
    }
#endif

    crc_calc = airquality7_get_crc(tmp_data, 6);
    if (crc_calc != tmp_data[6])
    {
        return AIRQUALITY7_ERR_CRC;
    }
    
    if (tvoc_ppb != AIRQUALITY7_NULL)
    {
        float res = AIRQUALITY7_DUMMY;
        res = (float)(tmp_data[0] - 13);
        res *= 1000;
        res /= 229;
        
        *tvoc_ppb = (uint16_t)res;
    }
    
    if (co2_ppm != AIRQUALITY7_NULL)
    {
        float res = AIRQUALITY7_DUMMY;
        res = (float)(tmp_data[1] - 13);
        res *= 1600;
        res /= 229;
        res += 400;
    
        *co2_ppm = (uint16_t)res;
    }
    
    if (res_val_ohm != AIRQUALITY7_NULL)
    {
        uint32_t res = AIRQUALITY7_DUMMY;
        res = (uint32_t)( tmp_data[2] * 65536 );
        res += (uint16_t)( tmp_data[3] * 256 );
        res += tmp_data[4];
        res *= 10;
    
        *res_val_ohm = res;
    }
    
    if (err_byte != AIRQUALITY7_NULL)
    {
        *err_byte = tmp_data[5];
    }
    
    return AIRQUALITY7_ERR_OK;
}

airquality7_err_t airquality7_get_revision(uint8_t *year, 
                                           uint8_t *month, 
                                           uint8_t *day, 
                                           uint8_t *ascii_code)
{
#if true2
    uint8_t crc_calc = AIRQUALITY7_DUMMY;

    uint8_t tmp_data[7] = { AIRQUALITY7_DUMMY };
    tmp_data[0] = AIRQUALITY7_CMD_GET_REVISION;
    tmp_data[5] = airquality7_get_crc(tmp_data, 5);

    int32_t exitCode = airquality7_generic_write(tmp_data);
    if (exitCode != 0) return AIRQUALITY7_ERR_WRITE;

    exitCode = airquality7_generic_read(tmp_data);
    if (exitCode != 0) return AIRQUALITY7_ERR_READ;
#else
    uint8_t tmp_data[7] = { AIRQUALITY7_DUMMY };
    uint8_t crc_calc = AIRQUALITY7_DUMMY;

    uint8_t write_data[6] = { AIRQUALITY7_DUMMY };
    write_data[0] = AIRQUALITY7_CMD_GET_REVISION;
    write_data[5] = airquality7_get_crc(write_data, 5);

    int32_t exitCode = airquality7_generic_write_then_read(write_data, tmp_data);
    if (exitCode != 0) return AIRQUALITY7_ERR_READ;
#endif

    crc_calc = airquality7_get_crc(tmp_data, 6);
    if (crc_calc != tmp_data[6])
    {
        return AIRQUALITY7_ERR_CRC;
    }
    
    if (year != AIRQUALITY7_NULL)
    {
        *year = tmp_data[0];
    }
    
    if (month != AIRQUALITY7_NULL)
    {
        *month = tmp_data[1];
    }
    
    if (day != AIRQUALITY7_NULL)
    {
        *day = tmp_data[2];
    }
    
    if (ascii_code != AIRQUALITY7_NULL)
    {
        *ascii_code = tmp_data[3];
    }
    
    return AIRQUALITY7_ERR_OK;
}

airquality7_err_t airquality7_get_r0_calib(uint16_t *r0_kohm)
{
    uint8_t tmp_data[7] = { AIRQUALITY7_DUMMY };
    uint8_t crc_calc = AIRQUALITY7_DUMMY;

    tmp_data[0] = AIRQUALITY7_CMD_GET_R0;
    tmp_data[5] = airquality7_get_crc(tmp_data, 5);

    airquality7_generic_write(tmp_data);
    airquality7_generic_read(tmp_data);

    crc_calc = airquality7_get_crc(tmp_data, 6);
    if (crc_calc != tmp_data[6])
    {
        return AIRQUALITY7_ERR_CRC;
    }

    if (r0_kohm != AIRQUALITY7_NULL)
    {
        //*r0_kohm = tmp_data[1];
        //*r0_kohm = (uint16_t)(*r0_kohm << 8);
        //*r0_kohm = (uint16_t)(*r0_kohm | tmp_data[0]);
        *r0_kohm = (uint16_t)((tmp_data[1] << 8) | tmp_data[0]);
    }

    return AIRQUALITY7_ERR_OK;
}

// ----------------------------------------------- PRIVATE FUNCTION DEFINITIONS

static void airquality7_communication_delay( void )
{
    uint32_t ms = 100;

	struct timespec ts;
    ts.tv_sec = (long int)(ms / 1000u);
    ts.tv_nsec = (long int)((ms - ((long unsigned int)ts.tv_sec * 1000u)) * 1000000u);

    nanosleep(&ts, NULL);
}

static uint8_t airquality7_get_crc( uint8_t *data_in, uint8_t data_size )
{
    uint8_t crc = 0x00;
    uint8_t cnt = 0x00;
    uint16_t sum = 0x0000;
 
    for (cnt = 0; cnt < data_size; cnt++)
    {
        sum = (uint16_t)(sum + *data_in);
        data_in++;
    }
    
    crc = (uint8_t)sum;
    crc = (uint8_t)(crc + sum / 0x0100);
    crc = (uint8_t)(0xFF - crc);

    return crc;
}
