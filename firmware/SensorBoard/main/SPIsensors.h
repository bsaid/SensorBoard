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

class SPIsensors
{
	const gpio_num_t PIN_NUM_MISO = GPIO_NUM_21;
	const gpio_num_t PIN_NUM_MOSI = GPIO_NUM_19;
	const gpio_num_t PIN_NUM_CLK  = GPIO_NUM_18;
	const gpio_num_t PIN_NUM_CS   = GPIO_NUM_5;

	spi_device_handle_t spi;
	uint8_t data[4];

public:
	SPIsensors()
	{
		printf("start...\n");
		esp_err_t ret;
		/*spi_bus_config_t buscfg={
			.miso_io_num=PIN_NUM_MISO,
			.mosi_io_num=PIN_NUM_MOSI,
			.sclk_io_num=PIN_NUM_CLK,
			.quadwp_io_num=-1,
			.quadhd_io_num=-1
		};*/
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
		assert(ret==ESP_OK);
		//Attach the LCD to the SPI bus
		ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
		assert(ret==ESP_OK);
		printf("spi...\n");
	}

	void readMPU9250()
	{
		printf("transaction...\n");
		spi_transaction_t s;
		memset(&s, 0, sizeof(s));       //Zero out the transaction
		s.addr = 0x43 | 0x80;
		s.flags = SPI_TRANS_USE_RXDATA;
		s.length = 40;
		esp_err_t ret=spi_device_transmit(spi, &s);  //Transmit!
		assert(ret==ESP_OK);            //Should have had no issues.
		for(int i=0;i<4;i++)
			data[i] = s.rx_data[i];
		printf("ACC data: %d %d %d %d\n", s.rx_data[0], s.rx_data[1], s.rx_data[2], s.rx_data[3]);
		printf("end...\n");
	}

	uint8_t getData()
	{
		return data[0];
	}

	//TODO: delete SPI driver in destructor
};

#endif // SPISENSORS_H_
