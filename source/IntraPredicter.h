#ifndef  __INTRAPREDICERT__
#define  __INTRAPREDICERT__
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "common/DistanceData.h"
#include "common/OutputWriter.h"
#include "common/DistanceCalculator.h"
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
class IntraPredicter {
public:
	IntraPredicter();
	~IntraPredicter();
public:
	virtual void predict();
	void generateOutPath(char * protocolPath, int calcMode);
	virtual void saveDistanceMatri(DistanceData* distanMatri, int uriMode, int width, int height, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2);
	virtual void convertXPoints(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	void writeMaxDistanceToFile(int calcMode);
	void writePostionToFile(DistanceData* distanMatri);
public:
	DistanceCalculator *distanceCalculator;
	OutputWriter*   outPutWriter;
	char outPath[MAX_PATH_LENGHT];
	char protocolOutPath[MAX_PATH_LENGHT];
	int calc_mode;
};
#endif

