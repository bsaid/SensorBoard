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
	const char* COUNTER_FILE_NAME = "counter.txt";
	int sessionCounter = 0;
	int logCounter = 0;

public:
	//SDcard sd;
	//SPIffs spi;

	FileSystem()
	{
		//TODO
		/*FILE* counterFile = getFile(COUNTER_FILE_NAME, "r");
		if(!counterFile)
		{
			counterFile = getFile(COUNTER_FILE_NAME, "w");
			fprintf(counterFile, "0\n");
			fclose(counterFile);
			counterFile = getFile(COUNTER_FILE_NAME, "r");
		}
		fscanf(counterFile, "%i", &sessionCounter);
		fclose(counterFile);*/
	}

	FILE* getFile(std::string name, std::string attributes)
	{
		//if(sd.isInitialized())
		//	return sd.getFile(name, attributes);
		//else
		//	return spi.getFile(name, attributes);
		return NULL;
	}

	FILE* getConfigFile()
	{
		return getFile(CONFIG_FILE_NAME, "r");
	}

	FILE* getLogFile()
	{
		char fileName[64];
		sprintf(fileName, "%i_%i_%s", sessionCounter, logCounter, LOG_FILE_NAME);
		return getFile(fileName, "a");
	}

	FILE* getNewLogFile()
	{
		logCounter++;
		return getLogFile();
	}
};

#endif /* MAIN_FILESYSTEM_H_ */
