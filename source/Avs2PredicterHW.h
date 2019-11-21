#ifndef __AVS2PredicterHW__
#define __AVS2PredicterHW__
#include "common/DistanceCalculator.h"
#include "common/Avs2Common.h"
#include "IntraPredicter.h"
#define  LINEREFER64 64
enum ScanType {
	SACN_TYPE_COL = 0,
	SCAN_TYPE_ROW = 1
};

class AVS2PredicterHW :public IntraPredicter
{

public:
	AVS2PredicterHW();
	~AVS2PredicterHW();


	void predict();
	void initDstData();
	void deinitDstData();
	void predPixelIndex(int j, int i);
	void predPixel(int j, int i);
	void finalPred(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int offset, int isiY);
	void scanByRow(int i, int j, int x_offset, int y_offset);
	void scanByCol(int i, int j, int x_offset, int y_offset);
	void get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset);
	void getLineMaxMinIndex(int i, int j, int m, int x_offset, int y_offset);
	void firstGroupProcess();
	void firstGroupCopySrc(int* dst, int *src);
	void firstGroupCopySrc64(int* dst, int *src);
	void secondGroupProcess();
	void secondGroupCopySrc(int* dst, int *src);
	int  convertSrcIndex2RefIndex(int index);
	int  convertSrcIndex2LineRefIndex(int index);
	void copyIndex(int** dst, int **src, int dstIndex, int srcIndex);

	void PredIntraAngAdi(DistanceData* distanMatri, int mode);
	int  getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset);
	void saveMaxMinIndex(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);
	void group256(int min_index);
	void group128(int left, int right, int index);
	void group64(int left, int right, int index);
	void group32(int index);
public:
	int **avs_dst;
	int *pSrc;
	int lineRefer[LINE_REFER_SIZE];
	int lineRefer64[LINEREFER64];
	int refer4X4[MATRI_REFER_SIZE];
	int groupType;
	int scanType;
	int uiDirMode;


};

#endif

