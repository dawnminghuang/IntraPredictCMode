#ifndef __VP9PREDICTER__
#define __VP9PREDICTER__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/Vp9Common.h"


class Vp9Predicter :public IntraPredicter
{

public:
	Vp9Predicter();
	~Vp9Predicter();
	void predict();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void initVp9Matri(int width, int height, int distanceSize);
	void deinitVp9Matri();
	void saveVp9Matri(int ** vp9Matri, int index, int iYnN1, int iY, int iYn, int iYnP2, int predictData);
	void convertSrc(int* above, int *left);
	void initDstData();
	void deinitDstData();
public:
	int bs;
	int **vp9_Matri;
	int **vp9_dst;
	int point_number;
	int max_dst_number;
	int tu_width;
	int tu_height;
};

#endif


