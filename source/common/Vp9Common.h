#ifndef __VP9COMMON__
#define __VP9COMMON__
#define NUM_INTRA_PMODE_VP9              9        //!< # luma intra prediction modes
#define NUM_DISTANCE_SIZE_VP9       4
#define NUM_CU_SIZE_VP9              4 
#define START_INDEX_VP9         1 
#define VP9_PATH "D:\\work\\IntraPredict\\modeVP9\\"
#define VP9_DATA_PATH "D:\\work\\IntraPredict\\modeVP9\\dstData\\"
#define VP9_DATA_PATH_CMODE "D:\\work\\IntraPredict\\modeVP9\\dstDataCMode\\"
#define MODE_TM    9
//  d207  d63  d45  d117 d135 d153 V H TM 
const char g_prdict_mode_vp9[NUM_INTRA_PMODE_VP9] = {
	 1, 2 ,3, 4, 5, 6, 7, 8, 9
};

const char g_cu_size_vp9[NUM_CU_SIZE_VP9][2] = {
	// width/height
	{4, 4},  {8, 8}, {16, 16}, {32,32}
};

#define AVG3(a, b, c) (((a) + 2 * (b) + (c) + 2) >> 2)
#define AVG2(a, b) (((a) + (b) + 1) >> 1)

static inline uint8_t clip_pixel(int val) {
	return (val > 255) ? 255 : (val < 0) ? 0 : val;
}
#endif

