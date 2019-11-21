#ifndef  __INTRAPREDICERT__
#define  __INTRAPREDICERT__
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "common/DistanceData.h"
#include "common/SrcData.h"
#include "common/OutputWriter.h"
#include "common/DistanceCalculator.h"
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// HardWare Value
#define MAX_INDEX 255
#define CENTRE_OFFSET 128
#define SECOND_GROUP_STEP 31
#define LINE_REFER_SIZE 8
#define MATRI_REFER_SIZE 32
enum GroupType {
	GROUD_TYPE_MIN = 0,
	GROUD_TYPE_MAX = 1
};
class IntraPredicter {
public:
	IntraPredicter();
	~IntraPredicter();
public:
	virtual void predict();
	virtual void setPredictSrcData(SrcData *srcData);
	void setPredictMode(int mode);
	void setTuSize(int width, int height);
	void generateOutPath(char * protocolPath, int calcMode);
	void generateDigOutPath(char * protocolPath, int calcMode);
	virtual void saveDistanceMatri(DistanceData* distanMatri, int uriMode, int width, int height, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2);
	virtual void convertXPoints(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	void writeMaxDistanceToFile(int calcMode);
	void writePostionToFile(DistanceData* distanMatri);
	void writeDiagPostionToFile();
	int calcMin(int** data, int width, int height);
	int calcMax(int** data, int width, int height);
	void initIndexMatri(int maxIndexNumber, int distanceSize);
	void deinitIndexMatri(int maxIndexNumber);
public:
	DistanceCalculator *distanceCalculator;
	OutputWriter*   outPutWriter;
	SrcData *src_data; 
	char outPath[MAX_PATH_LENGHT];
	char digOutPath[MAX_PATH_LENGHT];
	char protocolOutPath[MAX_PATH_LENGHT];
	int calc_mode;
	int tu_width;
	int tu_height;
	// HardWare Value
	int **max_min_indexs;
	int max_index;
	int min_index;
	int second_max;
	int second_min;
	int save_index;
	int first_bouds[2];
	int second_bouds[2];
	int minBounds[2];
	int maxBounds[2];
	int groupType;
};
#endif

