/*
 * WiFi.h
 *
 */

#ifndef MAIN_WIFI_H_
#define MAIN_WIFI_H_

#include <string>

class WiFi
{
public:
	WiFi(){}

	bool connectTo(std::string ssid, std::string pwd, int attempts = 3)
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

#endif /* MAIN_WIFI_H_ */
