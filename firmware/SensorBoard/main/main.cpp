//#include "tests.h"

#include "SPIsensors.h"
#include "I2Csensors.h"
#include "WiFi.h"
#include "GlobalSettings.h"
#include "FileSystem.h"
#include "Stopwatch.h"


extern "C" {
	void app_main(void);
}

void app_main()
{
	// Self-test
	//int result = Catch::Session().run();
	//TODO: handle test results

	// Filesystem initialization
	FileSystem fs;
	GlobalSettings settings(fs.getConfigFile());

	// Connection initialization
	/*WiFi wifi;
	bool isConnected = false;
	if(settings.isWiFiClient)
	{
		isConnected = wifi.connectTo(settings.WiFiSSID, settings.WiFiPassword);
	}
	if(!isConnected && settings.isWiFiAP)
	{
		isConnected = wifi.createAP(settings.WiFiSSID, settings.WiFiPassword);
		assert(isConnected);
	}*/

	// Sensors initialization
	SPIsensors spiSensors;
	I2Csensors i2cSensors;

	// Start logging
	bool isLoggingData = settings.startLoggingImmediately;
	bool isSendingData = false;

	FILE* sdCSV = fs.getLogFile();
	Stopwatch loopWatch(10);

    // Enter main loop
    for(;;)
    {
    	if(isLoggingData || isSendingData)
    	{
    		spiSensors.readMPU9250();
			i2cSensors.readBMP280();
    	}
    	if(isLoggingData)
    	{
			if(!sdCSV)
				printf("Log file cannot be opened.\n");
			else
			{
				Vector3i a = spiSensors.getAcc();
				Vector3i g = spiSensors.getGyr();
				Vector3i m = spiSensors.getMag();
				int16_t  t = spiSensors.getTemp();
				fprintf(
					sdCSV,
					"MPU;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n",
					a.x, a.y, a.z,
					g.x, g.y, g.z,
					m.x, m.y, m.z,
					t
				);
			}
    	}
    	if(isSendingData)
    	{
    		Vector3i a = spiSensors.getAcc();
			Vector3i g = spiSensors.getGyr();
			Vector3i m = spiSensors.getMag();
			int16_t  t = spiSensors.getTemp();
    		printf(
				"MPU;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n",
				a.x, a.y, a.z,
				g.x, g.y, g.z,
				m.x, m.y, m.z,
				t
			);
    	}
    	//TODO: read console char breaks the loop
    	//TODO: main loop> receives commands from USB or WiFi
    	//TODO: working as WiFi AP or device

    	if(!loopWatch.waitForNext())
    		printf("Loop overflow. The loop takes more than 100 ms.\n");
    }

    //TODO: program ended -> restart esp
}
