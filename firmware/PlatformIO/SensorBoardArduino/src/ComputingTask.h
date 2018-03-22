#ifndef COMPUTING_TASK_H
#define COMPUTING_TASK_H

// Horse Analysis, the test task for the Sensor Board
#include <horseanalysis.h>

const uint16_t FREQUENCY = 100; // Hz

// Filesystem initialization
FileSystem fs;
GlobalSettings settings;
RuntimeSettings machineState;

void dataLoopTask(void * pvParameters)
{
	// Sensors initialization
	SPI spiBus = SPI(GPIO_NUM_18, GPIO_NUM_21, GPIO_NUM_19);
	TestSensor testSensor;
	MPU9250 mpu(spiBus, 1000000, GPIO_NUM_5);
	mpu.init(false, false);
	// Logging file initialization
	FILE* sdCSV = fs.getLogFile();
	// Stopwatch initialization
	Stopwatch loopWatch(1000 / FREQUENCY);
	// Horse movement analysis based on accelerometer data
	HorseAnalysis horse(FREQUENCY);

	for(;;)
    {
		// Update all sensors
		testSensor.read();

    	if(machineState.isLogging)
    	{
			if(!sdCSV)
			{
				printf("Log file cannot be opened.\n");
				sdCSV = fs.getLogFile();
			}
			else
			{
				uint16_t ts = testSensor.get();
				fprintf(
					sdCSV,
					"S;%d;%d;\n",
					loopWatch.getCycles(),
					ts
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
			uint16_t ts = testSensor.get();
    		printf(
				"S;%d;%d;\n",
				loopWatch.getCycles(),
				ts
			);
    	}
		if(machineState.isHorseAnalysis)
        {
			Vector3i acc = mpu.getAcc();
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

    	if(!loopWatch.waitForNext())
    		printf("Loop overflow. The loop has taken more than 10 ms.\n");
    }
}

#endif // COMPUTING_TASK_H
