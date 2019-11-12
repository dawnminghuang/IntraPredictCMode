#ifndef __HEVCPREDICTERCMODE__
#define __HEVCPREDICTERCMODE__
#include "common/HevcCommon.h"
#include "IntraPredicter.h"
typedef       int                 Int;
typedef       bool                Bool;
typedef       int               Short;
typedef       Short           Pel;               ///< pixel type
class HevcPredicterCMode :public IntraPredicter
{

public:
	HevcPredicterCMode();
	~HevcPredicterCMode();

	void predict();
	void initDstData();
	void deinitDstData();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	int  computeIntraPredAngle(int uiDirMode);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index);

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
	Pel *hevc_dst;
};

#endif

