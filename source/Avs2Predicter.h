#ifndef __AVS2PREDICTER__
#define __AVS2PREDICTER__
#include "common/DistanceCalculator.h"
#include "common/Avs2Common.h"
#include "IntraPredicter.h"


class AVS2Predicter :public IntraPredicter
{

public:
	AVS2Predicter();
	~AVS2Predicter();


	void predict();
	void initDstData();
	void deinitDstData();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	int  getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);

public:
	int **avs_dst;

};

#endif

