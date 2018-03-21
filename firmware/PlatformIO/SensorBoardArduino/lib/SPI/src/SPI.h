/*
 * SPI.h
 *
 */

#ifndef COMPONENTS_SPI_INCLUDE_SPI_H_
#define COMPONENTS_SPI_INCLUDE_SPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

class SPIdevice
{
    static const uint8_t READ_FLAG = 0x80;
    spi_device_handle_t spi;

public:
    SPIdevice() {}

    SPIdevice(gpio_num_t pinCS, spi_host_device_t hostType = HSPI_HOST)
    {
        spi_device_interface_config_t devcfg;
		memset(&devcfg, 0, sizeof(devcfg));
		devcfg.clock_speed_hz=1000000;
		devcfg.mode=3;
		devcfg.spics_io_num=pinCS;
		devcfg.address_bits=8;
		devcfg.queue_size=1;
        esp_err_t ret;
		ret=spi_bus_add_device(hostType, &devcfg, &spi);
		assert(ret==ESP_OK);
		printf("spi...\n");
    }

    spi_device_handle_t getHandle()
    {
        return spi;
    }

    esp_err_t read(uint8_t addr, uint8_t length, uint8_t* rx_buffer)
    {
        spi_transaction_t t;
		memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.addr = addr | READ_FLAG;
		t.rx_buffer = rx_buffer;
		t.length = length*8;
		return spi_device_transmit(spi, &t);
    }

    uint8_t write(uint8_t addr, uint8_t length, uint8_t data0 = 0, uint8_t data1 = 0, uint8_t data2 = 0, uint8_t data3 = 0)
    {
        spi_transaction_t t;
		memset(&t, 0, sizeof(t));       //Zero out the transaction
		t.addr = addr;
        t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
		t.tx_data[0] = data0;
        t.tx_data[1] = data1;
        t.tx_data[2] = data2;
        t.tx_data[3] = data3;
		t.length = length*8;
		spi_device_transmit(spi, &t);
        return t.rx_data[0];
    }
};

class SPI
{
    spi_host_device_t hostType;

public:
    SPI() {}

    SPI(gpio_num_t pinCLK, gpio_num_t pinMISO, gpio_num_t pinMOSI)
    {
        this->hostType = HSPI_HOST;
        esp_err_t ret;
		spi_bus_config_t buscfg;
		memset(&buscfg, 0, sizeof(buscfg));
		buscfg.miso_io_num=pinMISO;
		buscfg.mosi_io_num=pinMOSI;
		buscfg.sclk_io_num=pinCLK;
		buscfg.quadwp_io_num=-1;
		buscfg.quadhd_io_num=-1;

        ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
		printf("ret==%d\n", ret);
		assert(ret==ESP_OK);
    }

    SPIdevice getDevice(gpio_num_t pinCS)
    {
        return SPIdevice(pinCS, hostType);
    }
};

#endif /* COMPONENTS_SPI_INCLUDE_SPI_H_ */
