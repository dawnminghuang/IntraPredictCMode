#ifndef __HEVCPREDICTER__
#define __HEVCPREDICTER__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/HevcCommon.h"

class HevcPredicter :public IntraPredicter
{

public:
	HevcPredicter();
	~HevcPredicter();
	void predict();
	void initDstData();
	void deinitDstData();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraPlanar(DistanceData* distanMatri, int uiDirMode);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index);
	int predIntraGetPredValDC(const int* pSrc, int iSrcStride, int iWidth, int iHeight);
	int  computeIntraPredAngle(int uiDirMode);
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
};

#endif
