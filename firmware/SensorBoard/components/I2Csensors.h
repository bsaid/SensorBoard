/*
 * I2Csensors.h
 *
 */

#ifndef MAIN_I2CSENSORS_H_
#define MAIN_I2CSENSORS_H_

#include <stdio.h>
#include "driver/i2c.h"

class I2Csensors
{
	const i2c_port_t USED_I2C        = I2C_NUM_1;
	const gpio_num_t MASTER_SCL_IO   = GPIO_NUM_26;
	const gpio_num_t MASTER_SDA_IO   = GPIO_NUM_27;
	const int MASTER_FREQ_HZ  = 1000000;
	const int SENSOR_ADDR     = 0x76;

	const int WRITE_BIT      = I2C_MASTER_WRITE; // !< I2C master write
	const int READ_BIT       = I2C_MASTER_READ;  // !< I2C master read
	const int ACK_CHECK_EN   = 0x1;     // !< I2C master will check ack from slave
	const int ACK_CHECK_DIS  = 0x0;     // !< I2C master will not check ack from slave
	const int ACK_VAL        = 0x0;     // !< I2C ack value
	const int NACK_VAL       = 0x1;     // !< I2C nack value

	void i2c_example_master_init()
	{
	    i2c_port_t i2c_master_port = USED_I2C;
	    i2c_config_t conf;
	    conf.mode = I2C_MODE_MASTER;
	    conf.sda_io_num = MASTER_SDA_IO;
	    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	    conf.scl_io_num = MASTER_SCL_IO;
	    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	    conf.master.clk_speed = MASTER_FREQ_HZ;
	    i2c_param_config(i2c_master_port, &conf);
	    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
	}


	esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t reg, uint8_t data)
	{
	    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}


	esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t reg, uint8_t* data_rd, size_t size)
	{
	    if (size == 0) {
	        return ESP_OK;
	    }
	    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	    if (size > 1) {
			i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
		}
		i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
	    i2c_master_stop(cmd);
	    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);
	    return ret;
	}


	void disp_buf(uint8_t* buf, int len)
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


public:
	I2Csensors()
	{
	    i2c_example_master_init();
	    esp_err_t ret = i2c_master_write_slave(USED_I2C, 0xF4, 0x27);
	    ret = i2c_master_write_slave(USED_I2C, 0xF5, 0xE0);
	    vTaskDelay(100);
	    printf("Write returned %d.\n", ret);
	}

	~I2Csensors()
	{
		i2c_driver_delete(USED_I2C);
	}

	void readBMP280()
	{
	    uint8_t data_rd[6];
	    for(int i=0; i<6; i++)
	    	data_rd[i] = i+16;
		esp_err_t ret = i2c_master_read_slave(USED_I2C, 0xF7, data_rd, 6);
		//esp_err_t ret = i2c_master_read_slave(USED_I2C, 0xF4, data_rd, 1);
		printf("Read returned %d.\n", ret);
		disp_buf(data_rd, 6);
		vTaskDelay(100);
	}
};

#endif // MAIN_I2CSENSORS_H_
