/*
 * I2C.h
 * Note: The functionality defined in the class does not cover the whole device functionality.
 *
 */

#ifndef COMPONENTS_I2C_INCLUDE_I2C_H_
#define COMPONENTS_I2C_INCLUDE_I2C_H_

#include <stdio.h>
#include "driver/i2c.h"

class I2Cbus
{
    i2c_port_t usedI2C;

public:
    I2Cbus(i2c_port_t usedI2C = I2C_NUM_1, gpio_num_t sclIO = GPIO_NUM_26, gpio_num_t sdaIO = GPIO_NUM_27, uint32_t freqHz = 1000000)
    {
        this->usedI2C = usedI2C;
        i2c_config_t conf;
	    conf.mode = I2C_MODE_MASTER;
	    conf.sda_io_num = sdaIO;
	    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	    conf.scl_io_num = sclIO;
	    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	    conf.master.clk_speed = freqHz;
	    i2c_param_config(usedI2C, &conf);
	    i2c_driver_install(usedI2C, conf.mode, 0, 0, 0);

        const int WRITE_BIT      = I2C_MASTER_WRITE; // I2C master write
        const int ACK_CHECK_EN   = 0x1;     // I2C master will check ack from slave

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, 0x76 << 1 | WRITE_BIT, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, 0xF4, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, 0x27, ACK_CHECK_EN);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(usedI2C, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    printf("ret %i\n", ret);
    }

    i2c_port_t num()
    {
        return usedI2C;
    }

    ~I2Cbus()
    {
        i2c_driver_delete(usedI2C);
    }
};

#endif /* COMPONENTS_I2C_INCLUDE_I2C_H_ */
