#ifndef __AVS2COMMON__
#define __AVS2COMMON__
// luma intra prediction modes

#define DC_PRED_AVS2                     0
#define PLANE_PRED_AVS2                  1
#define BI_PRED_AVS2                     2
#define VERT_PRED_AVS2                   12
#define HOR_PRED_AVS2                    24
#define NUM_INTRA_PMODE_AVS              33        //!< # luma intra prediction modes
#define NUM_MODE_INTRA_AVS               33        //!< # luma intra prediction modes
#define NUM_CU_PMODE_AVS                 9        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_AVS            4
#define AVS2_PATH "D:\\work\\IntraPredict\\modeAVS2\\"
#define AVS2_DATA_PATH "D:\\work\\IntraPredict\\modeAVS2\\dstData\\"
#define AVS2_DATA_PATH_CMODE "D:\\work\\IntraPredict\\modeAVS2\\dstDataCMode\\"
#define DEFAULT_CALC_MODE 0
#define Clip1(a)            ((a)>255?255:((a)<0?0:(a)))
#define Clip3(min,max,val)  (((val)<(min))?(min):(((val)>(max))?(max):(val)))
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

const int g_prdict_mode_row[NUM_INTRA_PMODE_AVS] = {
	0, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0,
	0, 0, 0, 1, 1, 
	1, 1, 1, 1, 1, 
	0, 0, 0, 0, 0,
	0, 0, 0
};

const int g_prdict_mode_avs[NUM_INTRA_PMODE_AVS] = {
	0,1,2, 3, 4, 5, 6,
	7, 8, 9, 10, 11,12,
	13,14,15,16, 17,18, 
	19,20,21,22, 23,24,
	25, 26, 27, 28,
	29, 30, 31, 32
};

const int g_cu_size_avs[NUM_CU_PMODE_AVS][2] = {
	// width/height
	{4, 16}, {4, 4}, {8, 32}, {8, 8}, {16, 16},
	{16, 4}, {32, 32}, {32, 8}, {64, 64}
};

//const int g_color_space[NUM_COLOR_SPACE_SIZE] = {
	//0, 1
//s};

#endif

