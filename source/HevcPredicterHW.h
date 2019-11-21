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
	void group256(int index);
	void group128(int left, int right, int index);
	void group64(int left, int right, int index);
	void group32(int left, int right, int index);
	void group16(int left, int right, int index);
	int convertSrcIndex2RefIndex(int index);
	int convertSrcIndex2LineRefIndex(int index);
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
};

#endif
