//#include "tests.h"

#include "SPIsensors.h"
#include "I2Csensors.h"
#include "WiFi.h"
#include "GlobalSettings.h"
#include "FileSystem.h"


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
	WiFi wifi;
	bool isConnected = false;
	if(settings.isWiFiClient)
	{
		isConnected = wifi.connectTo(settings.WiFiSSID, settings.WiFiPassword);
	}
	if(!isConnected && settings.isWiFiAP)
	{
		isConnected = wifi.createAP(settings.WiFiSSID, settings.WiFiPassword);
		assert(isConnected);
	}

	// Sensors initialization
	SPIsensors spiSensors;
	I2Csensors i2cSensors;

	// Start logging
    if(settings.startLoggingImmediately)
    {
		for(;;)
		{
			spiSensors.readMPU9250();
			i2cSensors.readBMP280();
			//TODO: read console char breaks the loop
		}
    }

    // Enter main loop
    for(;;)
    {
    	//TODO: main loop> receives commands from USB or WiFi
    	//TODO: working as WiFi AP or device
    }
}
