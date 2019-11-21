#ifndef __H264PREDICTERHW__
#define __H264PREDICTERHW__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/H264Common.h"
#define CENTRE_OFFSET_H264 129
#define LINE_REFER_SIZE_256 256
#define LINE_REFER_SIZE_32 32
#define LINE_REFER_SIZE_16 16
class H264PredicterHW :public IntraPredicter
{

public:
	H264PredicterHW();
	~H264PredicterHW();
	void predict();
	void predPixelIndex(int j, int i);
	void predPixel(int j, int i);
	void initDstData();
	void deinitDstData();
	void generateRefer();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void convertAbovePoints(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	void convertSrc(int* above, int *left);
	int  computeIntraPredAngle(int uiDirMode);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index);
	void get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset);
	void saveMaxMinIndex(int  iYnN1, int iY, int iYn, int iYnP2);
	void firstGroupProcess();
	void firstGroupCopySrc(int* dst, int *src);
	void secondGroupProcess();
	void secondGroupCopySrc(int* dst, int *src);
	void group256(int index);
	void group128(int left, int right, int index);
	void group64(int left, int right, int index);
	void group32(int left, int right, int index);
	int convertSrcIndex2RefIndex(int index);

	int convertAboveSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	int convertAboveSrcIndex2RefIndex(int index);
	int convertLeftSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	int convertLeftSrcIndex2RefIndex(int index);
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
	int **h264_dst;
	int refAbove[2 * MAX_CU_SIZE_H264 + 1];
	int refLeft[2 * MAX_CU_SIZE_H264 + 1];
	int *refMain;
	int *refSide;
	int lineRefer[LINE_REFER_SIZE];
	int lineRefer256[LINE_REFER_SIZE_256];
	int lineRefer32[LINE_REFER_SIZE_32];
	int lineRefer16[LINE_REFER_SIZE_16];
	int uiDirMode;
};

#endif
