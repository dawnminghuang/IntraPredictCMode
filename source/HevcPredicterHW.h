#ifndef __HEVCPREDICTERHW__
#define __HEVCPREDICTERHW__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/HevcCommon.h"
#define CENTRE_OFFSET_HEVC 128
#define LINE16REFER 16
class HevcPredicterHW :public IntraPredicter
{

public:
	HevcPredicterHW();
	~HevcPredicterHW();
	void predict();
	void predPixelIndex(int j, int i);
	void predPixel(int j, int i);
	void predPixelBi(int j ,int i);
	void initDstData();
	void deinitDstData();
	void generateRefer();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	int  computeIntraPredAngle(int uiDirMode);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index);
	void get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset);
	void saveMaxMinIndex(int x, int x1);
	void firstGroupProcess();
	void firstGroupCopySrc(int* dst, int *src);
	void secondGroupProcess();
	void secondGroupCopySrc(int* dst, int *src);
	int convertSrcIndex2RefIndex(int index);
	int convertSrcIndex2LineRefIndex(int index);
	int  biConvertSrcLeftIndex2LineRefIndex(int index);
	int  biConvertSrcAboveIndex2LineRefIndex(int index);
	void initTopLeftRefer(int j, int i, int x_offset, int y_offset);
	void initTopLeft();
	int predIntraGetPredValDC(const int* pSrc, int iSrcStride, int iWidth, int iHeight);
	void predIntraPlanar(DistanceData* distanMatri, int uiDirMode);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraDC(DistanceData* distanMatri, int uiDirMode);
	void predPixelIndexBi(int j, int i);
	void biFirstGroupProcess();
	void biFirstGroupCopySrc(int* dst, int *src);
	void generateBiRefer();
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
	int *hevc_dst;
	int refAbove[2 * MAX_CU_SIZE_HEVC + 1];
	int refLeft[2 * MAX_CU_SIZE_HEVC + 1];
	int *refMain;
	int *refSide;
	int lineRefer[LINE_REFER_SIZE];
	int refer4X4[MATRI_REFER_SIZE];
	int lineRefer16[LINE16REFER];
	int mode;

	int biLeftFirstBouds[2];
	int biLeftSecondBouds[2];
	int biAboveFirstBouds[2];
	int biAboveSecondBouds[2];
	int lineRefer256[LINEREFER256];
	int leftlineRefer4[LINEREFER4];
	int leftlineRefer8[LINEREFER8];
	int AbovelineRefer4[LINEREFER4];
	int AbovelineRefer8[LINEREFER8];
	int bottomLeft;
	int topRight;
	int leftColumn[MAX_CU_SIZE_HEVC + 1];
	int topRow[MAX_CU_SIZE_HEVC + 1];
	int bottomRow[MAX_CU_SIZE_HEVC]; 
	int rightColumn[MAX_CU_SIZE_HEVC];

};

#endif
