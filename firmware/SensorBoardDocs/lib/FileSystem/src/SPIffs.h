/*
 * SPIffs.h
 *
 */

#ifndef MAIN_SPIFFS_H_
#define MAIN_SPIFFS_H_

#include <stdio.h>

class SPIffs
{
public:
	FILE* getFile(std::string name, std::string attributes)
	{
		// not supported
		return NULL;
	}
};

#endif /* MAIN_SPIFFS_H_ */
