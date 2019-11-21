#ifndef __SRCDATA__
#define __SRCDATA__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  
#include <stdint.h>
#define MAX_CU_SIZE_AVS2    64
#define EXTENT_SIZE_AVS2    4
#define CU_SIZ_LOG_AVS2     6
#define MAX_CU_SIZE_HEVC    64
#define SRC_EXTEND_SIZE_HEVC 2
#define MAX_CU_SIZE_H264 8
#define NUM_EXTEND_SIZE_H264 2
#define MAX_CU_SIZE_VP9 32
#define NUM_EXTEND_SIZE_VP9 2
#define MAX_PIXEL 255
typedef enum  {
	MODE_RANDOM = 0,
	MODE_NORMAL = 1,
}GenerateModes;

class SrcData {
public:
	SrcData(int generate_mode);
	~SrcData();
	void initSrcData(char *value);
	void initAvs2SrcData();
	void initHevcSrcData();
	void initH264SrcData();
	void initVp9SrcData();
public:
	int  *avs2_src;
	int  avs2_max_cu_size;
	int  avs2_max_src_number;

	int  *hevc_src;
	int  hevc_src_number;

	int  *h264_src;
	int  h264_src_number;

	uint8_t  *vp9_src;
	int  vp9_src_number;

	int generate_mode;
};
#endif