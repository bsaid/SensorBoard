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
		FILE* file = sd.getFile(CONFIG_FILE_NAME, "r");
		if(!file)
		{
			file = spi.getFile(CONFIG_FILE_NAME, "r");
		}
		return file;
	}
};

#endif /* MAIN_FILESYSTEM_H_ */
