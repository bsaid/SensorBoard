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
	bool isStreaming;

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
		isWiFiAP = true;
		WiFiSSID = "SensorBoard";
		WiFiPassword = "1234567890";
		startLoggingImmediately = false;
		isHorseAnalysis = false;
		isStreaming = false;
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
