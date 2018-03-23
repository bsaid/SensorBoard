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
	SDcard sd;
	SPIffs spi;

	FileSystem()
	{
		//TODO: initialization cannot be here, because some main() code has to run before
	}

	bool init()
	{
		if(!sd.init())
			return false;

		sessionCounter = readCounter();
		writeCounter(sessionCounter+1);
		return true;
	}

	bool writeCounter(int number)
	{
		FILE* f = getFile(COUNTER_FILE_NAME, "w");
		if(!f)
			return false;
		fprintf(f, "%i\n", number);
		fclose(f);
		return true;
	}

	int readCounter()
	{
		FILE* f = getFile(COUNTER_FILE_NAME, "r");
		if(!f)
			return 0;
		int sessionCounter = 0;
		fscanf(f, "%i", &sessionCounter);
		fclose(f);
		return sessionCounter;
	}

	void increaseLogCounter()
	{
		logCounter++;
	}

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
