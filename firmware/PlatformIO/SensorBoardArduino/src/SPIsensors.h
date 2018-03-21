/*
 * SPIsensors.h
 *
 */

#ifndef SPISENSORS_H_
#define SPISENSORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#include "vector3.h"

#include <SPI.h>

// MPU9250 MAG registers
#define AK8963_WIA    0x00
#define AK8963_INFO   0x01
#define AK8963_ST1    0x02
#define AK8963_HXL    0x03
#define AK8963_HXH    0x04
#define AK8963_HYL    0x05
#define AK8963_HYH    0x06
#define AK8963_HZL    0x07
#define AK8963_HZH    0x08
#define AK8963_ST2    0x09

#define AK8963_I2C_ADDR   0x0c//0x18
#define AK8963_Device_ID  0x48
#define MPUREG_EXT_SENS_DATA_00    0x49

class SPIsensors
{
	//TODO: make constants as #define or as member constants?
	const gpio_num_t PIN_NUM_MISO = GPIO_NUM_21;
	const gpio_num_t PIN_NUM_MOSI = GPIO_NUM_19;
	const gpio_num_t PIN_NUM_CLK  = GPIO_NUM_18;
	const gpio_num_t PIN_NUM_CS   = GPIO_NUM_5;
	const uint8_t MPUREG_I2C_SLV0_ADDR = 0x25;
	const uint8_t MPUREG_I2C_SLV0_REG  = 0x26;
	const uint8_t MPUREG_I2C_SLV0_CTRL = 0x27;
	const uint8_t READ_FLAG = 0x80;

	spi_device_handle_t spi;
	// format: axh, axl, ayh, ayl, azh, azl, th, tl, gxh, gxl, gyh, gyl, gzh, gzl
	uint8_t rx_data[14];
	// format: mxh, mxl, myh, myl, mzh, mzl
	uint8_t mag_data[6];

	SPI spiBus;
	SPIdevice mpu;

public:
	SPIsensors()
	{
		printf("start...\n");
		spiBus = SPI(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI);
		mpu = spiBus.getDevice(PIN_NUM_CS);
		spi = mpu.getHandle();
		/*esp_err_t ret;
		spi_bus_config_t buscfg;
		memset(&buscfg, 0, sizeof(buscfg));
		buscfg.miso_io_num=PIN_NUM_MISO;
		buscfg.mosi_io_num=PIN_NUM_MOSI;
		buscfg.sclk_io_num=PIN_NUM_CLK;
		buscfg.quadwp_io_num=-1;
		buscfg.quadhd_io_num=-1;
		spi_device_interface_config_t devcfg;
		memset(&devcfg, 0, sizeof(devcfg));
		devcfg.clock_speed_hz=1000000;
		devcfg.mode=3;
		devcfg.spics_io_num=PIN_NUM_CS;
		devcfg.address_bits=8;
		devcfg.queue_size=1;
		printf("init...\n");
		//Initialize the SPI bus
		ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
		printf("ret==%d\n", ret);
		//assert(ret==ESP_OK);
		ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
		//assert(ret==ESP_OK);*/
		printf("spi...\n");
	}

	void readMPU9250()
	{
		esp_err_t ret = mpu.read(59, 15, rx_data);
		/*spi_transaction_t s;
		memset(&s, 0, sizeof(s));       //Zero out the transaction
		s.addr = 59 | READ_FLAG;
		s.rx_buffer = rx_data;
		s.length = 15*8;
		esp_err_t ret = spi_device_transmit(spi, &s);  //Transmit!
		*/

		// ACC+GYR+TEMP reg. 59 - 72
		// MAG as I2C slave

		// Set MPU9250 - MAG
		ret = mpu.write(MPUREG_I2C_SLV0_ADDR, 1, AK8963_I2C_ADDR|READ_FLAG);
		/*spi_transaction_t t;
		memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.flags = SPI_TRANS_USE_TXDATA;
		t.addr = MPUREG_I2C_SLV0_ADDR;
		t.tx_data[0] = AK8963_I2C_ADDR|READ_FLAG;	// Set the I2C slave addres of AK8963 and set for read.
		t.length = 1*8;
		ret = spi_device_transmit(spi, &t);*/

		ret = mpu.write(MPUREG_I2C_SLV0_REG, 1);
		/*memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.flags = SPI_TRANS_USE_TXDATA;
		t.addr = MPUREG_I2C_SLV0_REG;
		t.tx_data[0] = 0;//AK8963_HXL;
		t.length = 2*8;
		ret = spi_device_transmit(spi, &t);*/

		ret = mpu.write(MPUREG_I2C_SLV0_CTRL, 2, 0x87);
		/*memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.flags = SPI_TRANS_USE_TXDATA;
		t.addr = MPUREG_I2C_SLV0_CTRL;
		t.tx_data[0] = 0x87;
		t.length = 2*8;
		ret = spi_device_transmit(spi, &t);*/

		// Read MAG
		ret = mpu.read(MPUREG_EXT_SENS_DATA_00, 8, rx_data);
		/*memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.addr = MPUREG_EXT_SENS_DATA_00 | READ_FLAG;
		t.rx_buffer = rx_data;
		t.length = 8*8;
		ret = spi_device_transmit(spi, &t);*/

		//printf("MAG data: ");
		//for(int i=0; i<13; i++)
		//	printf("%.2d ", rx_data[i]);
		//printf("\n");
	}

	Vector3i getAcc()
	{
		return Vector3i(
			((int16_t)rx_data[0]<<8) | rx_data[1],
			((int16_t)rx_data[2]<<8) | rx_data[3],
			((int16_t)rx_data[4]<<8) | rx_data[5]
		);
	}

	int16_t getTemp()
	{
		return ((int16_t)rx_data[6]<<8) | rx_data[7];
	}

	Vector3i getGyr()
	{
		return Vector3i(
			((int16_t)rx_data[8]<<8) | rx_data[9],
			((int16_t)rx_data[10]<<8) | rx_data[11],
			((int16_t)rx_data[12]<<8) | rx_data[13]
		);
	}

	Vector3i getMag()
	{
		return Vector3i(
			((int16_t)mag_data[0]<<8) | mag_data[1],
			((int16_t)mag_data[2]<<8) | mag_data[3],
			((int16_t)mag_data[4]<<8) | mag_data[5]
		);
	}

	//TODO: delete SPI driver in destructor
};

#endif // SPISENSORS_H_
