#ifndef __H264COMMON__
#define __H264COMMON__
#define NUM_INTRA_PMODE_264           9        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_264     4
#define NUM_CU_SIZE_264          3 
#define START_INDEX_264          3 
#define MAX_CU_SIZE_H264         16
#define NUM_EXTEND_SIZE_H264     2
#define BLOCK_SIZE_8x8           8
#define MB_BLOCK_SIZE           16
#define MAX_PIXEL_VALUE        255
#define PLANE_16                 3
#define MODE_DC_H264             2
#define H264_PATH "D:\\work\\IntraPredict\\modeH264\\"
#define H264_DATA_PATH "D:\\work\\IntraPredict\\modeH264\\dstData\\"
#define H264_DATA_PATH_CMODE "D:\\work\\IntraPredict\\modeH264\\dstDataCMode\\"
const char g_prdict_mode_264[NUM_INTRA_PMODE_264] = {
	0, 1, 2, 3, 4, 5, 6,
	7, 8
};

const char g_cu_size_264[NUM_CU_SIZE_264][2] = {
	// width/height
	{4, 4},  {8, 8}, {16, 16}
};



#endif

