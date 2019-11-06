#ifndef __AVS2PREDICTER__
#define __AVS2PREDICTER__
#include "common/DistanceCalculator.h"
#include "IntraPredicter.h"
#define NUM_INTRA_PMODE_AVS              33        //!< # luma intra prediction modes
#define NUM_MODE_INTRA_AVS              28        //!< # luma intra prediction modes
#define NUM_CU_PMODE_AVS              9        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_AVS      4
#define AVS2_PATH "D:\\work\\IntraPredict\\modeAVS2\\"
#define DEFAULT_CALC_MODE 0

const unsigned int g_aucXYflg[NUM_INTRA_PMODE_AVS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 1, 1
};

const int g_aucDirDx[NUM_INTRA_PMODE_AVS] = {
    0, 0, 0, 11, 2,
    11, 1, 8, 1, 4,
    1, 1, 0, 1, 1,
    4, 1, 8, 1, 11,
    2, 11, 4, 8, 0,
    8, 4, 11, 2, 11,
    1, 8, 1
};

const int g_aucDirDy[NUM_INTRA_PMODE_AVS] = {
    0, 0, 0, -4, -1,
    -8, -1, -11, -2, -11,
    -4, -8, 0, 8, 4,
    11, 2, 11, 1, 8,
    1, 4, 1, 1, 0,
    -1, -1, -4, -1, -8,
    -1, -11, -2
};

const int g_aucSign[NUM_INTRA_PMODE_AVS] = {
    0, 0, 0, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, 0, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 0,
    -1, -1, -1, -1, -1,
    -1, -1, -1
};


const int g_aucDirDxDy[2][NUM_INTRA_PMODE_AVS][2] = {
    {
        // dx/dy
        {0, 0}, {0, 0}, {0, 0}, {11, 2}, {2, 0},
        {11, 3}, {1, 0}, {93, 7}, {1, 1}, {93, 8},
        {1, 2}, {1, 3}, {0, 0}, {1, 3}, {1, 2},
        {93, 8}, {1, 1}, {93, 7}, {1, 0}, {11, 3},
        {2, 0}, {11, 2}, {4, 0}, {8, 0}, {0, 0},
        {8, 0}, {4, 0}, {11, 2}, {2, 0}, {11, 3},
        {1, 0}, {93, 7}, {1, 1},
    },
    {
        // dy/dx
        {0, 0}, {0, 0}, {0, 0}, {93, 8}, {1, 1},
        {93, 7}, {1, 0}, {11, 3}, {2, 0}, {11, 2},
        {4, 0}, {8, 0}, {0, 0}, {8, 0}, {4, 0},
        {11, 2}, {2, 0}, {11, 3}, {1, 0}, {93, 7},
        {1, 1}, {93, 8}, {1, 2}, {1, 3}, {0, 0},
        {1, 3}, {1, 2}, {93, 8}, {1, 1}, {93, 7},
        {1, 0}, {11, 3}, {2, 0}
    }
};

const int g_prdict_mode_avs[NUM_INTRA_PMODE_AVS] = {
    3, 4, 5, 6,
    7, 8, 9, 10,
    11, 13, 14, 15,
    16, 17, 18, 19,
    20,  21, 22, 23,
    25, 26, 27, 28,
    29, 30, 31, 32
};


const int g_cu_size_avs[NUM_CU_PMODE_AVS][2]= {
    // width/height
    {4, 16}, {4, 4}, {8, 32}, {8, 8}, {16, 16},
    {16, 4}, {32, 32}, {32, 8}, {64, 64}
};


class AVS2Predicter:public IntraPredicter
{

public:
    AVS2Predicter();
    ~AVS2Predicter();
    void predict();
    void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
    int getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset);
    void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);

};

#endif

