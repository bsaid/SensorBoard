#ifndef COMPUTING_TASK_H
#define COMPUTING_TASK_H

#include <FileSystem.h>
#include <GlobalSettings.h>
#include <SPI.h>
#include <MPU9250.h>
#include <ADP5062.h>
#include <TestSensor.h>
#include <I2C.h>
#include <BMP280.h>

#include "Stopwatch.h"
#include "led.h"
#include "RuntimeSettings.h"

// Horse Analysis, the test task for the Sensor Board
#include <horseanalysis.h>

const uint16_t FREQUENCY = 100; // Hz

// Filesystem initialization
FileSystem fs;
GlobalSettings settings;
RuntimeSettings machineState;

void dataLoopTask(void * pvParameters)
{
	printf("Computing task started...\n");

	// Peripherials initialize
	Led gLed(GPIO_NUM_32);
	gLed.blink();

	// Sensors initialization
	SPI spiBus = SPI(GPIO_NUM_18, GPIO_NUM_21, GPIO_NUM_19);
	TestSensor testSensor;
	MPU9250 mpu(spiBus, 1000000, GPIO_NUM_5);
	mpu.init(false, false);
	I2Cbus i2c;
	BMP280 bmp(&i2c);
	ADP5062 adp(&i2c);
	// Logging file initialization
	FILE* sdCSV = fs.getLogFile();
	// Stopwatch initialization
	Stopwatch loopWatch(1000 / FREQUENCY);
	// Horse movement analysis based on accelerometer data
	HorseAnalysis horse(FREQUENCY);

	// Help temporary variables
	int overflows = 0;
	int ovfTimes[32];

	for(;;)
    {
		// Update all sensors
		testSensor.read();
		mpu.read_all();
		bmp.update();

		gLed.reset();
    	if(machineState.isLogging)
    	{
			gLed.set();
			if(!sdCSV)
			{
				printf("Log file cannot be opened.\n");
				sdCSV = fs.getLogFile();
			}
			else
			{
				Vector3f acc = mpu.getAcc();
				Vector3f gyr = mpu.getGyr();
				Vector3f mag = mpu.getMag();
				fprintf(
					sdCSV,
					"S;%d;%d;%d;%f;%f;%x;%x;;%f;%f;%f;%f;%f;%f;%f;%f;%f;\n",
					loopWatch.get_ms(),
					loopWatch.getCycles(),
					testSensor.get(),
					bmp.getTemperature(),
					bmp.getPressure(),
					bmp.getData0(),
					bmp.getData1(),
					acc.x, acc.y, acc.z,
					gyr.x, gyr.y, gyr.z,
					mag.x, mag.y, mag.z
				);
				if(loopWatch.getCycles() % 100 == 0)
				{
					fclose(sdCSV);
					sdCSV = fs.getLogFile();
				}
			}
    	}
    	if(machineState.isStreaming)
    	{
			Vector3f acc = mpu.getAcc();
			Vector3f gyr = mpu.getGyr();
			Vector3f mag = mpu.getMag();
			printf(
				"S;%d;%d;%d;%f;%f;%x;%x;;%f;%f;%f;%f;%f;%f;%f;%f;%f;\n",
				loopWatch.get_ms(),
				loopWatch.getCycles(),
				testSensor.get(),
				bmp.getTemperature(),
				bmp.getPressure(),
				bmp.getData0(),
				bmp.getData1(),
				acc.x, acc.y, acc.z,
				gyr.x, gyr.y, gyr.z,
				mag.x, mag.y, mag.z
			);
    	}
		if(machineState.isHorseAnalysis)
        {
			Vector3f acc = mpu.getAcc();
			horse.addData(acc.x, acc.y, acc.z, 0, 0, 0);
			if(horse.elapsedTime() % 1000 == 0)
			{
				//TODO: make CSV compatible output, log data to SD card
				printf("Second %4d: moving %s, steps %4i, %s, ...\n",
	                   horse.elapsedTime()/1000,
	                   horse.isMoving() ? "yes": "no ",
	                   horse.numSteps(),
	                   horse.detectAndNameMovement().c_str()
	            );
			}
        }

		int ovfTime = loopWatch.waitForNext();
    	if(ovfTime > 0)
		{
			if(overflows < 32)
				ovfTimes[overflows++] = ovfTime;
			else
			{
				overflows = 0;
				printf("Loop overflows: ");
				for(int i=0; i<32; i++)
				{
					printf("%d, ", ovfTimes[i]);
				}
				printf("\n");
			}
		}
    }
}

#endif // COMPUTING_TASK_H
