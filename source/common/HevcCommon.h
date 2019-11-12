#ifndef __HEVCCOMMON__
#define __HEVCCOMMON__
#define NUM_INTRA_PMODE_HEVC              33        //!< # luma intra prediction modes
#define NUM_ANG_TABLE              9        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_HEVC      2
#define NUM_CU_SIZE_HEVC             5 
#define START_INDEX_HEVC             2
#define VER_IDX 26
#define HOR_IDX 10
#define VER_HOR_IDX 18
#define MAX_CU_SIZE_HEVC  64
#define NUM_EXTEND_SIZE_HEVC  4
#define HEVC_PATH "D:\\work\\IntraPredict\\modeHEVC\\"
#define HEVC_DATA_PATH "D:\\work\\IntraPredict\\modeHEVC\\dstData\\"
#define HEVC_DATA_PATH_CMODE "D:\\work\\IntraPredict\\modeHEVC\\dstDataCMode\\"
const int g_prdict_mode_hevc[NUM_INTRA_PMODE_HEVC] = {
	2, 3, 4, 5, 6,
	7, 8, 9, 10,
	11, 12, 13, 14, 15,
	16, 17, 18, 19,
	20, 21, 22, 23, 24,
	25, 26, 27, 28, 29,
	30, 31, 32, 33, 34
};


const int g_ang_table[NUM_ANG_TABLE] = {
	0, 2, 5, 9, 13,
	17, 21, 26, 32
};

const int  invAngTable[9] = { 
	0, 4096, 1638, 910, 630,
	482, 390, 315, 256 
};

const int g_offset[NUM_INTRA_PMODE_HEVC] = {
	32, 26, 21, 17, 13, 9, 5, 2, 0,
	-2,  -5,  -9, -13, -17, -21, -26,
	-32, -26, -21, -17, -13, -9, -5, -2,
	0, 2, 5 ,9, 13, 17, 21, 26, 32
};


const int g_inv_ang_table[NUM_ANG_TABLE] = {
	0, 4096, 1638, 910,
	630, 482, 390, 315, 256
};


const int g_cu_size_hevc[NUM_CU_SIZE_HEVC][2] = {
	// width/height
	{4, 4},  {8, 8}, {16, 16},
	{32, 32}, {64, 64}
};
/*const int g_cu_size_hevc[NUM_CU_SIZE_HEVC][2] = {
	// width/height
	{64, 64}
};*/

#endif

