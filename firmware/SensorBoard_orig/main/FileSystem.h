/*
 * FileSystem.h
 *
 */

#ifndef MAIN_FILESYSTEM_H_
#define MAIN_FILESYSTEM_H_

#include "SDcard.h"
#include "SPIffs.h"
#include "GlobalSettings.h"

class FileSystem
{
	const char* CONFIG_FILE_NAME = "config.ini";
	const char* LOG_FILE_NAME = "log.csv";
public:
	SDcard sd;
	SPIffs spi;

	FILE* getFile(std::string name, std::string attributes)
	{
		if(sd.isInitialized())
			return sd.getFile(name, attributes);
		else
			return spi.getFile(name, attributes);
	}

	FILE* getConfigFile()
	{
		return getFile(CONFIG_FILE_NAME, "r");
	}

	FILE* getLogFile()
	{
		return getFile(LOG_FILE_NAME, "a");
	}
};

#endif /* MAIN_FILESYSTEM_H_ */
