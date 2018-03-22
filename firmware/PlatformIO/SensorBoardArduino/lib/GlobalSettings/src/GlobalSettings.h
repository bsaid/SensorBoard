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
	std::string WiFiSSID;
	std::string WiFiPassword;
	bool startLoggingImmediately;
	bool isHorseAnalysis;

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
		isWiFiClient = false;
		isWiFiAP = false;
		WiFiSSID = "SensorBoard";
		WiFiPassword = "1234567890";
		startLoggingImmediately = false;
		isHorseAnalysis = false;
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
