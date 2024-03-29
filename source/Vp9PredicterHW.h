#ifndef __VP9PREDICTERHW__
#define __VP9PREDICTERHW__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/Vp9Common.h"
#define CENTRE_OFFSET_VP9 129
#define LINE_REFER_SIZE_256 256
#define LINE_REFER_SIZE_32 32
#define LINE_REFER_SIZE_16 16
class Vp9PredicterHW :public IntraPredicter
{

public:
	Vp9PredicterHW();
	~Vp9PredicterHW();
	void predict();
	void predPixelIndex(DistanceData* distanMatri, int j, int i);
	void predIndexAll(DistanceData* distanMatri, int uiDirMode);
	void predPixel(DistanceData* distanMatri, int j, int i);
	void predPixelBi(DistanceData* distanMatri, int j, int i);
	void initDstData();
	void deinitDstData();
	void generateRefer();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraTM(DistanceData* distanMatri, int uiDirMode);
	void convertSrc(int* above, int *left);
	void get4X4MaxMinIndex(DistanceData* distanMatri, int i, int j, int x_offset, int y_offset);
	void saveMaxMinIndex(int  iYnN1, int iY, int iYn, int iYnP2);
	void firstGroupProcess();
	void firstGroupCopySrc(int* dst, int *src);
	void secondGroupProcess();
	void secondGroupCopySrc(int* dst, int *src);
	int convertSrcIndex2RefIndex(int index);

	int convertAboveSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	int convertAboveSrcIndex2RefIndex(int index);
	int convertLeftSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	int convertLeftSrcIndex2RefIndex(int index);
	int  biConvertSrcLeftIndex2LineRefIndex(int index);
	int  biConvertSrcAboveIndex2LineRefIndex(int index);
	void initVp9Matri(int width, int height, int distanceSize);
	void saveVp9Matri(int ** vp9Matri, int index, int iYnN1, int iY, int iYn, int iYnP2, int predictData);
	void deinitVp9Matri();

	void biFirstGroupProcess();
	void biFirstGroupCopySrc(int* dst, int *src);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
public:
	bool is_mod_ver;
	int intra_pred_angle_mode;
	int abs_ang_mode;
	int intra_pred_angle;
	int sign_ang;
	int abs_ang;
	int inv_angle;
	int max_dst_number;
	int dst_stride_True;
	int **vp9_dst;
	int refAbove[2 * MAX_CU_SIZE_VP9 + 1];
	int refLeft[2 * MAX_CU_SIZE_VP9 + 1];
	int *refMain;
	int *refSide;
	int lineRefer[LINE_REFER_SIZE];
	int lineRefer256[LINE_REFER_SIZE_256];
	int lineRefer32[LINE_REFER_SIZE_32];
	int lineRefer16[LINE_REFER_SIZE_16];

	int biLeftFirstBouds[2];
	int biLeftSecondBouds[2];
	int biAboveFirstBouds[2];
	int biAboveSecondBouds[2];
	int leftlineRefer4[LINEREFER4];
	int leftlineRefer8[LINEREFER8];
	int AbovelineRefer4[LINEREFER4];
	int AbovelineRefer8[LINEREFER8];
	int uiDirMode;
	// VP9
	int bs;
	int **vp9_Matri;
	int point_number;
	int mode;
	int ytop_left;
};

#endif
