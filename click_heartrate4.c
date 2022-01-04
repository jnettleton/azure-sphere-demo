/*
 * MikroSDK - MikroE Software Development Kit
 * Copyright© 2020 MikroElektronika d.o.o.
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
 * OR OTHER DEALINGS IN THE SOFTWARE. 
 */

/*!
 * \file
 *
 */

// https://github.com/MikroElektronika/mikrosdk_click_v2/tree/master/clicks/heartrate4

#include "click_heartrate4.h"

bool heartrate4_found;

void heartrate4_default_cfg(void)
{
    heartrate4_reset();
    heartrate4_fifo_setting(2, HEARTRATE4_SAMPLEAVG_32);
    heartrate4_fifo_setting(1, 0);
    heartrate4_enable_disable_interrupts(3, 1);
    heartrate4_setting_function(2, HEARTRATE4_MODE_REDONLY);
    heartrate4_setting_function(4, HEARTRATE4_SAMPLERATE_1600);
    heartrate4_enable_disable_interrupts(1, 1);
    heartrate4_enable_disable_interrupts(0, 1);
    heartrate4_setting_prox_and_amp(0, HEARTRATE4_AVG_POWER_LEVEL);
    heartrate4_setting_prox_and_amp(1, HEARTRATE4_AVG_POWER_LEVEL);
    heartrate4_setting_prox_and_amp(2, HEARTRATE4_AVG_POWER_LEVEL);
    heartrate4_setting_prox_and_amp(3, 0x0F);
    heartrate4_setting_prox_and_amp(4, 0x1F);
}

void heartrate4_write_data(uint8_t wr_addr, uint8_t wr_data)
{
    uint8_t tx_buf[2];
    tx_buf[0] = wr_addr;
    tx_buf[1] = wr_data;

    I2CMaster_Write(i2cIsu2Fd, HEARTRATE4_I2C_ADDRESS, tx_buf, 2);
}

uint8_t heartrate4_read_data(uint8_t rd_addr)
{
    uint8_t rx_buf[1];
    uint8_t tx_buf[1];

    tx_buf [0] = rd_addr;
    I2CMaster_WriteThenRead(i2cIsu2Fd, HEARTRATE4_I2C_ADDRESS, tx_buf, 1, rx_buf, 1);

    return rx_buf[0];
}

void heartrate4_multi_read (uint8_t rd_addr, uint8_t *buffer, uint8_t cnt)
{
    I2CMaster_WriteThenRead(i2cIsu2Fd, HEARTRATE4_I2C_ADDRESS, &rd_addr, 1, buffer, cnt);
}

void heartrate4_set_new_value (uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t old_val;

    old_val = heartrate4_read_data(reg);
    old_val = old_val & mask;

    heartrate4_write_data(reg, old_val | value);
}

uint8_t heartrate4_get_intrrupt(uint8_t flag)
{
    if (flag == 1)
    {
        // Get the main interrupt group 
        return heartrate4_read_data(HEARTRATE4_INT_STAT1);
    }
    if (flag == 2)
    {
        // Get the temporary ready interrupt
        return heartrate4_read_data(HEARTRATE4_INT_STAT2);
    }

    return 0;
}

void heartrate4_enable_disable_interrupts(uint8_t interrupt_flag, uint8_t enable_flag)
{
    uint8_t reg_addr;
    uint8_t mask; 
    uint8_t value;

    if (interrupt_flag >= 0 && interrupt_flag <= 3)
    {
        reg_addr = HEARTRATE4_INT_ENABLE1;
    }
    else if (interrupt_flag == 4)
    {
        reg_addr = HEARTRATE4_INT_ENABLE2;
    }

    if (interrupt_flag == 0)
    {
        mask = HEARTRATE4_INT_A_FULL_MASK;
        if (enable_flag == 1)
        {
            value = HEARTRATE4_INT_A_FULL_ENABLE;
        }
        else if (enable_flag == 0)
        {
            value = HEARTRATE4_INT_A_FULL_DISABLE;
        }
    }
    
    if (interrupt_flag == 1)
    {
        mask = HEARTRATE4_INT_DATA_RDY_MASK;
        if (enable_flag == 1)
        {
            value = HEARTRATE4_INT_DATA_RDY_ENABLE;
        }
        else if (enable_flag == 0)
        {
            value = HEARTRATE4_INT_DATA_RDY_DISABLE;
        }
    }

    if (interrupt_flag == 2)
    {
        mask = HEARTRATE4_INT_ALC_OVF_MASK;
        if (enable_flag == 1)
        {
            value = HEARTRATE4_INT_ALC_OVF_ENABLE;
        }
        else if (enable_flag == 0)
        {
            value = HEARTRATE4_INT_ALC_OVF_DISABLE;
        }
    }

    if (interrupt_flag == 3)
    {
        mask = HEARTRATE4_INT_PROX_INT_MASK;
        if (enable_flag == 1)
        {
            value = HEARTRATE4_INT_PROX_INT_ENABLE;
        }
        else if (enable_flag == 0)
        {
            value = HEARTRATE4_INT_PROX_INT_DISABLE;
        }
    }

    if (interrupt_flag == 4)
    {
        mask = HEARTRATE4_INT_DIE_TEMP_RDY_MASK;
        if (enable_flag == 1)
        {
            value = HEARTRATE4_INT_DIE_TEMP_RDY_ENABLE;
        }
        else if (enable_flag == 0)
        {
            value = HEARTRATE4_INT_DIE_TEMP_RDY_DISABLE;
        }
    }

    heartrate4_set_new_value(reg_addr, mask, value);
}

void heartrate4_setting_function(uint8_t flag, uint8_t mode)
{
    uint8_t reg_addr;
    uint8_t mask; 
    uint8_t value;

    if (flag >= 0 && flag <= 2)
    {
        reg_addr = HEARTRATE4_MODE_CONFIG;
    }
    else if (flag >= 3 && flag <= 5)
    {
        reg_addr = HEARTRATE4_SPO2_CONFIG;
    }

    if (flag == 0)
    {
        mask  = HEARTRATE4_SHUTDOWN_MASK;
        value = HEARTRATE4_SHUTDOWN;
    }
    else if (flag == 1)
    {
        mask  = HEARTRATE4_SHUTDOWN_MASK;
        value = HEARTRATE4_WAKEUP;
    }
    else if (flag == 2)
    {
        mask = HEARTRATE4_MODE_MASK;
    } 
    else if (flag == 3)
    {
        mask = HEARTRATE4_ADCRANGE_MASK;
    } 
    else if (flag == 4)
    {
        mask = HEARTRATE4_SAMPLERATE_MASK;
    } 
    else if (flag == 5)
    {
        mask = HEARTRATE4_PULSEWIDTH_MASK;
    } 

    if (flag >= 2  && flag <= 5)
    {
        value = mode;
    }

    heartrate4_set_new_value(reg_addr, mask, value);
}

void heartrate4_setting_prox_and_amp(uint8_t flag, uint8_t write_data)
{
   uint8_t write_addr;

    if (flag == 0)
    {
        write_addr = HEARTRATE4_LED1_PULSE_AMP;
    }
    else if (flag == 1)
    {
        write_addr = HEARTRATE4_LED2_PULSE_AMP;
    }
    else if (flag == 2)
    {
        write_addr = HEARTRATE4_LED3_PULSE_AMP;
    } 
    else if (flag == 3)
    {
        write_addr = HEARTRATE4_LED_PROX_AMP;
    } 
    else if (flag == 4)
    {
        write_addr = HEARTRATE4_PROX_INT_THRESH;
    } 

   heartrate4_write_data(write_addr, write_data);
}

void heartrate4_enable_slot(uint8_t slot_num, uint8_t dev)
{
    if (slot_num == 1)
    {
        heartrate4_set_new_value(HEARTRATE4_MULTI_LED_CONFIG1, HEARTRATE4_SLOT1_MASK, dev);
    }
    else if (slot_num == 2)
    {
        heartrate4_set_new_value(HEARTRATE4_MULTI_LED_CONFIG1, HEARTRATE4_SLOT2_MASK, (uint8_t)(dev << 4));
    }
    else if (slot_num == 3)
    {
        heartrate4_set_new_value(HEARTRATE4_MULTI_LED_CONFIG2, HEARTRATE4_SLOT3_MASK, dev);
    }
    else if (slot_num == 4)
    {
        heartrate4_set_new_value(HEARTRATE4_MULTI_LED_CONFIG2, HEARTRATE4_SLOT4_MASK, (uint8_t)(dev << 4));
    }
}

void heartrate4_disable_slots()
{
    heartrate4_write_data(HEARTRATE4_MULTI_LED_CONFIG1, 0);
    heartrate4_write_data(HEARTRATE4_MULTI_LED_CONFIG2, 0);
}
void heartrate4_clear_fifo()
{
    heartrate4_write_data(HEARTRATE4_FIFO_WRITE_PTR, 0);
    heartrate4_write_data(HEARTRATE4_FIFO_OVERFLOW, 0);
    heartrate4_write_data(HEARTRATE4_FIFO_READ_PTR, 0);
}

void heartrate4_fifo_setting(uint8_t flag, uint8_t samp_num)
{
    uint8_t mask; 
    uint8_t value;
    
    if (flag == 0 || flag == 1)
    {
        mask = HEARTRATE4_ROLLOVER_MASK;
        if (flag == 0)
        {
            value = HEARTRATE4_ROLLOVER_DISABLE;
        }
        else if (flag == 1)
        {
            value = HEARTRATE4_ROLLOVER_ENABLE;
        }
    }
    else if (flag == 2 || flag == 3)
    {
        value = samp_num;
        if (flag == 2)
        {
            value = HEARTRATE4_SAMPLEAVG_MASK;
        }
        else if (flag == 3)
        {
            value = HEARTRATE4_A_FULL_MASK;
        }
    }

    heartrate4_set_new_value(HEARTRATE4_FIFO_CONFIG, mask, value);
}

uint8_t heartrate4_get_write_ptr()
{
    return heartrate4_read_data(HEARTRATE4_FIFO_WRITE_PTR);
}

uint8_t heartrate4_get_read_ptr()
{
    return heartrate4_read_data(HEARTRATE4_FIFO_READ_PTR);
}

float heartrate4_read_temp_c()
{
    float result;
    uint8_t temp_int;
    uint8_t temp_frac;

    heartrate4_write_data(HEARTRATE4_DIE_TEMP_CONFIG, 0x01);

    temp_int = heartrate4_read_data(HEARTRATE4_DIE_TEMP_INT);
    temp_frac = heartrate4_read_data(HEARTRATE4_DIE_TEMP_FRAC);

    result = (float)temp_int + ((float)temp_frac * 0.0625F);

    return result;
}

float heartrate4_read_temp_f()
{
    float result = heartrate4_read_temp_c();

    result = result * 1.8F + 32.0F;

    return result;
}

void heartrate4_reset()
{
    heartrate4_set_new_value(HEARTRATE4_MODE_CONFIG, HEARTRATE4_RESET_MASK, HEARTRATE4_RESET);
}

uint8_t heartrate4_check_int()
{
    //return digital_in_read(&ctx->int1);
    return 0;
}

uint32_t heartrate4_read_fifo_oldest_sample()
{
    uint32_t red_val;
    uint8_t temp_red[ 4 ];
    uint8_t rd_buff[ 9 ];
    uint8_t wr_buff[ 1 ];

    wr_buff[ 0 ] = HEARTRATE4_FIFO_DATA;

    I2CMaster_WriteThenRead(i2cIsu2Fd, HEARTRATE4_I2C_ADDRESS, wr_buff, 1, rd_buff, 3);

    temp_red[ 3 ] = 0;
    temp_red[ 2 ] = rd_buff[ 0 ] & 0x03;
    temp_red[ 1 ] = rd_buff[ 1 ];
    temp_red[ 0 ] = rd_buff[ 2 ];
    
    red_val = ((uint32_t)temp_red[ 3 ] << 24 | (uint32_t)temp_red[ 2 ] << 16 | (uint32_t)temp_red[ 1 ] << 8 | (uint32_t)temp_red[ 0 ]);

    return red_val;
}

uint8_t heartrate4_check_int_pow_and_temp(uint8_t flag)
{
    uint8_t result;
    if (flag == 5)
    {
        result = heartrate4_get_intrrupt(2);
    }
    else
    {
        result = heartrate4_get_intrrupt(1);
    }

    switch (flag)
    {
        case 0:
            result >>= 7;
            break;
        case 1:
            result >>= 6;
            break;
        case 2:
            result &= (1 << 5);
            result >>= 5;
            break;
        case 3:
            result &= (1 << 4);
            result >>= 4;
            break;
        case 4:
            result &= 1;
            break;
        case 5:
            result &= (1 << 1);
            result >>= 1;
            break;
    }

    return result;
}

uint16_t heartrate4_read_fifo_all_samples(uint8_t *buff)
{
    uint8_t *tem_ptr = buff;
    uint16_t samp_to_rd = 0;
    uint16_t fifo_rd_ptr = 0;
    uint16_t fifo_wr_ptr = 0;
    uint16_t cnt = 0;

    fifo_wr_ptr = heartrate4_get_write_ptr();
    fifo_rd_ptr = heartrate4_get_read_ptr();
    samp_to_rd = (uint16_t)(fifo_wr_ptr - fifo_rd_ptr);

    for (cnt = 0; cnt < samp_to_rd; cnt++)
    {
        heartrate4_multi_read(HEARTRATE4_FIFO_DATA, tem_ptr, 9);
        tem_ptr+= 9;
    }

    return samp_to_rd;
}

uint32_t heartrate4_get_red_val()
{
    uint8_t i;
    uint8_t sample_num;
    uint8_t wr_ptr;
    uint8_t rd_ptr;
    uint8_t temp_red[ 4 ];
    uint32_t red_val;
    uint32_t red_aver;
    uint8_t rd_buff[ 3 ];
    wr_ptr = 0;
    rd_ptr = 0;
    red_aver = 0;
    red_val = 0;

    wr_ptr = heartrate4_get_write_ptr();
    rd_ptr = heartrate4_get_read_ptr();

    sample_num = (uint8_t)((16 + wr_ptr - rd_ptr) % 16);

    if (sample_num >= 1)
    {
        for (i = 0; i < sample_num; ++i)
        {
            heartrate4_multi_read(HEARTRATE4_FIFO_DATA, rd_buff, 3);

            temp_red[ 3 ] = 0;
            temp_red[ 2 ] = rd_buff[ 0 ] & 0x03;
            temp_red[ 1 ] = rd_buff[ 1 ];
            temp_red[ 0 ] = rd_buff[ 2 ];

            red_val = ((uint32_t)temp_red[ 3 ] << 24 | (uint32_t)temp_red[ 2 ] << 16 | (uint32_t)temp_red[ 1 ] << 8 | (uint32_t)temp_red[ 0 ]);

            red_aver += red_val;
        }
        red_aver /= sample_num;
    }
    return red_aver;
}

void heartrate4_set_registers(heartrate4_set_registers_t *registers)
{
    heartrate4_fifo_setting(2, registers->avg_samp);
    heartrate4_fifo_setting(1, 0);
    heartrate4_setting_function(2, registers->mode);
    heartrate4_setting_function(3, registers->adc_range);   
    heartrate4_setting_function(4, registers->samp_rate);
    heartrate4_setting_function(5, registers->pulse_width);
    heartrate4_setting_prox_and_amp(0, registers->led_pow_lvl);
    heartrate4_setting_prox_and_amp(1, registers->led_pow_lvl);
    heartrate4_setting_prox_and_amp(2, registers->led_pow_lvl);
    heartrate4_setting_prox_and_amp(3, registers->led_pow_lvl);

    heartrate4_enable_slot(1, HEARTRATE4_SLOT_RED_LED);

    if (registers->mode > 2)
    {
        heartrate4_enable_slot(1, HEARTRATE4_SLOT_RED_LED);
        heartrate4_enable_slot(2, HEARTRATE4_SLOT_IR_LED);
    }
    if (registers->mode > 3)
    {
        heartrate4_enable_slot(1, HEARTRATE4_SLOT_RED_LED);
        heartrate4_enable_slot(2, HEARTRATE4_SLOT_IR_LED);
        heartrate4_enable_slot(3, HEARTRATE4_SLOT_GREEN_LED);
    }

    heartrate4_clear_fifo();
}
