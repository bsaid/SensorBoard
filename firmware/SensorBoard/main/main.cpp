#include "tests.h"

#include "../components/SPIsensors.h"
#include "../components/I2Csensors.h"
#include "../components/SDcard.h"

class SPIffs{};

class GlobalSettings
{
public:
	GlobalSettings(){}

	bool isWiFiClient()
	{
		//TODO
		return false;
	}

	bool isWiFiAP()
	{
		//TODO
		return false;
	}

	char* getWiFiSSID()
	{
		//TODO
		return 0;
	}

	char* getWiFiPassword()
	{
		//TODO
		return 0;
	}

	bool startLoggingImmediately()
	{
		//TODO
		return true;
	}
};

class WiFi
{
public:
	WiFi(){}

	bool connectTo(char* ssid, char* pwd, int attempts = 3)
	{
		//TODO
		return false;
	}

	bool createAP(char* ssid, char* pwd)
	{
		//TODO
		return false;
	}
};

class FileSystem
{
public:
	FileSystem()
	{
		SDcard sd;
		if(!sd.isPresent())
		{
			SPIffs fs;
		}
	}

	GlobalSettings readConfigFile()
	{
		//TODO
		return GlobalSettings();
	}
};

extern "C" {
	void app_main(void);
}

void app_main()
{
	// Self-test
	int result = Catch::Session().run();
	//TODO: handle test results

	// Filesystem initialization
	FileSystem fs;
	GlobalSettings settings = fs.readConfigFile();

	// Connection initialization
	WiFi wifi;
	bool isConnected = false;
	if(settings.isWiFiClient())
	{
		isConnected = wifi.connectTo(settings.getWiFiSSID(), settings.getWiFiPassword());
	}
	if(!isConnected && settings.isWiFiAP())
	{
		isConnected = wifi.createAP(settings.getWiFiSSID(), settings.getWiFiPassword());
		assert(isConnected);
	}

	// Sensors initialization
	SPIsensors spiSensors;
	I2Csensors i2cSensors;

	// Start logging
    if(settings.startLoggingImmediately())
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
