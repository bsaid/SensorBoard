/*
 * I2Cdevice.h
 * Note: The functionality defined in the class does not cover the whole device functionality.
 *
 */

#ifndef COMPONENTS_I2C_INCLUDE_I2C_DEVICE_H_
#define COMPONENTS_I2C_INCLUDE_I2C_DEVICE_H_

#include "I2C.h"

class I2Cdevice
{
    static const uint8_t WRITE_BIT      = I2C_MASTER_WRITE; // I2C master write
	static const uint8_t READ_BIT       = I2C_MASTER_READ;  // I2C master read
	static const uint8_t ACK_CHECK_EN   = 0x1;     // I2C master will check ack from slave
	static const uint8_t ACK_CHECK_DIS  = 0x0;     // I2C master will not check ack from slave
	static const uint8_t ACK_VAL        = 0x0;     // I2C ack value
	static const uint8_t NACK_VAL       = 0x1;     // I2C nack value

    I2Cbus* i2c;
    uint8_t deviceAddress;

protected:
    I2Cdevice(I2Cbus* i2c, uint8_t address)
        : i2c(i2c), deviceAddress(address)
    {}

    esp_err_t write(uint8_t reg, uint8_t data)
	{
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, deviceAddress << 1 | WRITE_BIT, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(i2c->num(), cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}

	esp_err_t read(uint8_t reg, uint8_t* data_rd, size_t size)
	{
	    if (size == 0) {
	        return ESP_OK;
	    }
	    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, deviceAddress << 1 | WRITE_BIT, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, deviceAddress << 1 | READ_BIT, ACK_CHECK_EN);
	    if (size > 1) {
			i2c_master_read(cmd, data_rd, size - 1, (i2c_ack_type_t)ACK_VAL);
		}
		i2c_master_read_byte(cmd, data_rd + size - 1, (i2c_ack_type_t)NACK_VAL);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(i2c->num(), cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}


	static void disp_buf(uint8_t* buf, int len)
	{
	    int i;
	    for (i = 0; i < len; i++) {
	        printf("%02x ", buf[i]);
	        if (( i + 1 ) % 16 == 0) {
	            printf("\n");
	        }
	    }
	    printf("\n");
	}
};

#endif /* COMPONENTS_I2C_INCLUDE_I2C_DEVICE_H_ */
