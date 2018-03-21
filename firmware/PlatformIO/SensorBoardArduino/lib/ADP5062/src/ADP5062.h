/*
 * BMP280.h
 *
 */

#ifndef COMPONENTS_BMP280_INCLUDE_ADP5062_H_
#define COMPONENTS_BMP280_INCLUDE_ADP5062_H_

#include <I2Cdevice.h>

class ADP5062 : protected I2Cdevice
{
    static const uint8_t ADP5062_ADDRESS = 0x29;
    uint8_t data_rd[6];

public:
    ADP5062(I2Cbus* i2c)
        : I2Cdevice(i2c, ADP5062_ADDRESS)
	{
        rr(0x00, data_rd, 6);
        for(int i=0; i<6; i++) printf("ADP %.2i 0x%x\n", i, data_rd[i]);
        ww(0x02, 0x0F);
        rr(0x00, data_rd, 6);
        for(int i=0; i<6; i++) printf("ADP %.2i 0x%x\n", i, data_rd[i]);
	}

    esp_err_t rr(uint8_t reg, uint8_t* data_rd, size_t size)
	{
	    if (size == 0) {
	        return ESP_OK;
	    }
	    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, 0x28, true);
	    i2c_master_write_byte(cmd, 0x00, true);
	    i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x29, true);
	    if (size > 1) {
			i2c_master_read(cmd, data_rd, size - 1, 0);
		}
		i2c_master_read_byte(cmd, data_rd + size - 1, 1);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}

    esp_err_t ww(uint8_t reg, uint8_t data)
	{
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, 0x28, true);
	    i2c_master_write_byte(cmd, reg, true);
	    i2c_master_write_byte(cmd, data, true);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}

	void update()
	{
		//
	}

    uint32_t get()
    {
        //
    }
};

#endif /* COMPONENTS_BMP280_INCLUDE_ADP5062_H_ */
