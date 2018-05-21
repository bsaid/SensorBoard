/*
 * GlobalSettings.h
 *
 */

#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_

#include <string>
#include <map>

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

	std::map<std::string,std::string> data;

    bool loadFile(FILE* f)
    {
		if(f == NULL)
			return false;

        char line[256];
        while (fgets(line, sizeof(line), f))
        {
			int index = 0;
            for(index = 0; line[index] != ' '; index++);
            std::string id;
            id.append(line, line+index);

			int startValueIndex = index+1;
            for(index++; line[index] != '\n'; index++);
            std::string value;
            value.append(line+startValueIndex, line+index);

            data[id] = value;
        }

        fclose(f);
        return true;
    }

	void saveFile(FILE* f)
	{
		//TODO
	}
};

#endif // GLOBAL_SETTINGS_H_
