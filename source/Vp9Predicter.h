#ifndef __Vp9PREDICTER__
#define __Vp9PREDICTER__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#define NUM_INTRA_PMODE_VP9              6        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_VP9       4
#define NUM_CU_SIZE_VP9              4 
#define START_INDEX_VP9         1 
#define VP9_PATH "D:\\work\\IntraPredict\\modeVP9\\"
const char g_prdict_mode_vp9[NUM_INTRA_PMODE_VP9] = {
     1, 2 ,3, 4, 5, 6
};

const char g_cu_size_vp9[NUM_CU_SIZE_VP9][2]= {
    // width/height
    {4, 4},  {8, 8}, {16, 16}, {32,32}
};


class Vp9Predicter:public IntraPredicter
{

public:
    Vp9Predicter();
    ~Vp9Predicter();
    void predict();
    void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
    void initVp9Matri(int width, int height, int distanceSize);
	void deinitVp9Matri();
    void saveVp9Matri(int ** vp9Matri, int index, int iYnN1, int iY, int iYn, int iYnP2);
public:
    int bs;
    int **vp9_Matri;
	int point_number;
};

#endif


