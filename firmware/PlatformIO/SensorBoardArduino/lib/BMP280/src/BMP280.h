/*
 * BMP280.h
 *
 */

#ifndef COMPONENTS_BMP280_INCLUDE_BMP280_H_
#define COMPONENTS_BMP280_INCLUDE_BMP280_H_

#include <I2Cdevice.h>

class BMP280 : protected I2Cdevice
{
    static const uint8_t BMP280_ADDRESS = 0x76;
    uint8_t data_rd[6];

public:
    BMP280(I2Cbus* i2c)
        : I2Cdevice(i2c, BMP280_ADDRESS)
	{
        write(0xF4, 0x27);
        write(0xF5, 0xE0);
	}

	void update()
	{
		read(0xF7, data_rd, 6);
	}

    uint32_t get()
    {
        //TODO
        for(int i=0; i<6; i++)
            printf("%i, ", data_rd[i]);
        return data_rd[0];
    }
};

#endif /* COMPONENTS_BMP280_INCLUDE_BMP280_H_ */
