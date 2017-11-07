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

    // Enter main loop
    for(;;)
    {
    	if(isLoggingData)
    	{
    		spiSensors.readMPU9250();
			i2cSensors.readBMP280();
			//TODO: read console char breaks the loop
    	}
    	if(isSendingData)
    	{
    		//TODO: send measured data immediately
    	}
    	//TODO: main loop> receives commands from USB or WiFi
    	//TODO: working as WiFi AP or device
    }

    //TODO: program ended -> restart esp
}
