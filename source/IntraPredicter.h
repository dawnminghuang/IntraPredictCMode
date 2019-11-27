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
#define  LINEREFER256 256
#define  LINEREFER64 64
#define  LINEREFER32 32
#define  LINEREFER4   4
#define  LINEREFER8   8
#define NUM_COLOR_SPACE_SIZE             3
enum GroupType {
	GROUD_TYPE_MIN = 0,
	GROUD_TYPE_MAX = 1
};

enum ExtractType {
	EXTRACT_BOUND_128 = 0,
	EXTRACT_BOUND_64 = 1,
	EXTRACT_BOUND_32 = 2,
	EXTRACT_BOUND_16 = 3,
	EXTRACT_BOUND_8 = 4,
	EXTRACT_BOUND_4 = 5,
};


enum ColorSpaceType {
	COLOR_SPACE_LUMA = 0,
	COLOR_SPACE_CHROME_U = 1,
	COLOR_SPACE_CHROME_V = 2
};
static inline int imax(int a, int b)
{
	return ((a) > (b)) ? (a) : (b);
}
static inline int imin(int a, int b)
{
	return ((a) < (b)) ? (a) : (b);
}

static inline int iClip1(int high, int x)
{
	x = imax(x, 0);
	x = imin(x, high);

	return x;
}

class IntraPredicter {
public:
	IntraPredicter();
	~IntraPredicter();
public:
	virtual void predict();
	virtual void setPredictSrcData(SrcData *srcData);
	virtual void saveDistanceMatri(DistanceData* distanMatri, int uriMode, int width, int height, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2);
	virtual void convertXPoints(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	void setPredictMode(int mode);
	void setTuSize(int width, int height);
	void generateOutPath(char * protocolPath, int calcMode);
	void generateDigOutPath(char * protocolPath, int calcMode);
	void writeMaxDistanceToFile(int calcMode);
	void writePostionToFile(DistanceData* distanMatri);
	void writeDiagPostionToFile();
	void initIndexMatri(int maxIndexNumber, int distanceSize);
	void deinitIndexMatri(int maxIndexNumber);
	int calcMin(int** data, int width, int height);
	int calcMax(int** data, int width, int height);

	// hardWare simulate
	void group256(int index);
	void group128(int left, int right, int index);
	void group64(int left, int right, int index);
	void group32(int left, int right, int index);
	void group16(int left, int right, int index);
	void group8(int left, int right, int index);
	void extractBouds(int left, int right);
	void initLog2size();
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
	int bi_left_min;
	int bi_above_min;
	int bi_above_max;
	int second_max;
	int second_min;
	int save_index;
	int first_bouds[2];
	int second_bouds[2];
	int minBounds[2];
	int maxBounds[2];
	int groupType;
	int extraType;
	int g_log2size[MAX_CU_SIZE_AVS2 + 1];
};
#endif

