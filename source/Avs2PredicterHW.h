#ifndef __AVS2PredicterHW__
#define __AVS2PredicterHW__
#include "common/DistanceCalculator.h"
#include "common/Avs2Common.h"
#include "IntraPredicter.h"

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
	void biFirstGroupProcess();
	void firstGroupCopySrc32(int* dst, int *src);
	void secondGroupProcess();
	void secondGroupCopySrc(int* dst, int *src);
	int  convertSrcIndex2RefIndex(int index);
	int  convertSrcIndex2LineRefIndex(int index);
	int  biConvertSrcLeftIndex2LineRefIndex(int index);
	int  biConvertSrcAboveIndex2LineRefIndex(int index);
	void copyIndex(int** dst, int **src, int dstIndex, int srcIndex);
	void initTopLeftRefer(int j, int i, int x_offset, int y_offset);
	void initReferAB();
	void initTopLeft();
	void PredIntraAngAdi(DistanceData* distanMatri, int mode);
	void PredIntraBiAdi(DistanceData* distanMatri, int mode);
	int  getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset);
	void saveMaxMinIndex(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);
	void xPredIntraDCAdi(DistanceData* distanMatri, int uiDirMode);
	void xPredIntraPlaneAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predPixelIndexVerHor(int j, int i);
	void predPixelIndexBi(int j, int i);
	void predPixelVerHor(int j, int i);
	void predPixelBi(int j, int i);
	void finalPredVerHor(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int isiY);
	void avs2Group32(int index);
public:
	int **avs_dst;
	int *pSrc;
	int lineRefer[LINE_REFER_SIZE];
	int lineRefer64[LINEREFER64];
	int lineRefer32[LINEREFER32];
	int leftlineRefer4[LINEREFER4];
	int leftlineRefer8[LINEREFER8];
	int AbovelineRefer4[LINEREFER4];
	int AbovelineRefer8[LINEREFER8];
	int scanType;
	int uiDirMode;
	int iHeight;
	int iWidth;
	int sample_bit_depth;
	int bLeftAvail;
	int bAboveAvail;
	int cu_size_log;
	int biLeftFirstBouds[2];
	int biLeftSecondBouds[2];
	int biAboveFirstBouds[2];
	int biAboveSecondBouds[2];
	int pTop[MAX_CU_SIZE_AVS2];
	int pLeft[MAX_CU_SIZE_AVS2];
	int pT[MAX_CU_SIZE_AVS2];
	int pL[MAX_CU_SIZE_AVS2];
	int a;
	int b;
};

#endif

