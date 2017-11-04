/*
 * GlobalSettings.h
 *
 */

#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_

#include <string>

class GlobalSettings
{
public:
	bool isWiFiClient;
	bool isWiFiAP;
	std::string getWiFiSSID;
	std::string getWiFiPassword;
	bool startLoggingImmediately;

	GlobalSettings()
	{
		loadDefault();
	}

	GlobalSettings(FILE* f)
	{
		if(!loadFile(f))
			loadDefault();
	}

	void loadDefault()
	{
		isWiFiClient = true;
		isWiFiAP = true;
		getWiFiSSID = "SensorBoard";
		getWiFiPassword = "1234567890";
		startLoggingImmediately = true;
	}

	bool loadFile(FILE* f)
	{
		//TODO
		return false;
	}

	void saveFile(FILE* f)
	{
		//TODO
	}
};

#endif // GLOBAL_SETTINGS_H_
