#ifndef __OUTPUTWRITER__
#define __OUTPUTWRITER__
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <string.h>
#include <errno.h>
#include <io.h>
#include "DistanceData.h"
#include "dirent.h"

#define MAX_PATH_LENGHT 4096
#define MAX_MODE_PATH 16

#define SUCCESS 0
#define FAILURE 1

class OutputWriter {

public:
	OutputWriter();
	~OutputWriter();
	int initModeInfoFp(char * path, int predictMode);
	int initDistanceInfoFp(char * path, char*  calc_mode);
	int initOutputAllWriter(char * path);
	void writeModeInfoToFile(char *data);
	void writeModeInfoToFile(DistanceData *distancedata);
	void writeDistanceToFile(int *distanceData, int modeNumber);

public:
	FILE *modeInfoFp;
	FILE *distanceInfoFp;
	FILE *outAllFp;
	int predictMode;
	char outPutPath[MAX_PATH_LENGHT];
};
#endif




