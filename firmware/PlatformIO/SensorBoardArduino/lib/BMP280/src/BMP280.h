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
    uint8_t calibration[26];
    uint8_t data_rd[6];
    double pressure = 0;
    double celTemp = 0;

    void convertData()
    {
        // temp coefficients
        unsigned int dig_T1 = (calibration[0] & 0xFF) + ((calibration[1] & 0xFF) * 256);
        int dig_T2 = calibration[2] + (calibration[3] * 256);
        int dig_T3 = calibration[4] + (calibration[5] * 256);

        // pressure coefficients
        unsigned int dig_P1 = (calibration[6] & 0xFF) + ((calibration[7] & 0xFF) * 256);
        int dig_P2 = calibration[8] + (calibration[9] * 256);
        int dig_P3 = calibration[10] + (calibration[11] * 256);
        int dig_P4 = calibration[12] + (calibration[13] * 256);
        int dig_P5 = calibration[14] + (calibration[15] * 256);
        int dig_P6 = calibration[16] + (calibration[17] * 256);
        int dig_P7 = calibration[18] + (calibration[19] * 256);
        int dig_P8 = calibration[20] + (calibration[21] * 256);
        int dig_P9 = calibration[22] + (calibration[23] * 256);

        // Convert pressure and temperature data to 19-bits
        long adc_p = (((long)(data_rd[0] & 0xFF) * 65536) + ((long)(data_rd[1] & 0xFF) * 256) + (long)(data_rd[2] & 0xF0)) / 16;
        long adc_t = (((long)(data_rd[3] & 0xFF) * 65536) + ((long)(data_rd[4] & 0xFF) * 256) + (long)(data_rd[5] & 0xF0)) / 16;

        // Temperature offset calculations
        double var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
        double var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
        double t_fine = (long)(var1 + var2);
        celTemp = (var1 + var2) / 5120.0;

        // Pressure offset calculations
        var1 = ((double)t_fine / 2.0) - 64000.0;
        var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
        var2 = var2 + var1 * ((double)dig_P5) * 2.0;
        var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
        var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
        var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
        double p = 1048576.0 - (double)adc_p;
        p = (p - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double) dig_P9) * p * p / 2147483648.0;
        var2 = p * ((double) dig_P8) / 32768.0;
        pressure = (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100;
    }

public:
    BMP280(I2Cbus* i2c)
        : I2Cdevice(i2c, BMP280_ADDRESS)
	{
        // BMP280 configuration
        write(0xF4, 0x27);
        write(0xF5, 0xE0);

        // Read calibration data
        read(0x88, calibration, 26);
	}

	void update()
	{
		read(0xF7, data_rd, 6);
        convertData();
	}

    double getTemperature()
    {
        return celTemp;
    }

    double getPressure()
    {
        return pressure;
    }

    uint32_t getData0()
    {
        return (data_rd[0] << 16) | (data_rd[1] << 8) | (data_rd[2]);
    }

    uint32_t getData1()
    {
        return (data_rd[3] << 16) | (data_rd[4] << 8) | (data_rd[5]);
    }
};

#endif /* COMPONENTS_BMP280_INCLUDE_BMP280_H_ */
