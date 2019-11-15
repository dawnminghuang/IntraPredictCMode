#ifndef __H264PREDICTER__
#define __H264PREDICTER__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/H264Common.h"

class H264Predicter :public IntraPredicter
{

public:
	H264Predicter();
	~H264Predicter();
	void predict();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void convertAbovePoints(int* iYnN1, int* iY, int* iYn, int* iYnP2);
	void convertSrc(int* above, int *left);
	void initDstData();
	void deinitDstData();

public:
	int **h264_dst;
	int max_dst_number;

};

#endif

