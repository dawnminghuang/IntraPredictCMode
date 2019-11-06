#ifndef __H264PREDICTER__
#define __H264PREDICTER__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#define NUM_INTRA_PMODE_264             6        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_264     4
#define NUM_CU_SIZE_264          2 
#define START_INDEX_264          3 
#define H264_PATH "D:\\work\\IntraPredict\\modeH264\\"

const char g_prdict_mode_264[NUM_INTRA_PMODE_264] = {
    3, 4, 5, 6,
    7, 8
};

const char g_cu_size_264[NUM_DISTANCE_SIZE_264][2]= {
    // width/height
    {4, 4},  {8, 8}
};


class H264Predicter:public IntraPredicter
{

public:
    H264Predicter();
    ~H264Predicter();
    void predict();
    void  predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
};

#endif

