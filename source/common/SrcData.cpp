#include"SrcData.h"


SrcData::SrcData(int mode) {
	avs2_max_cu_size = MAX_CU_SIZE_AVS2;
	avs2_src = NULL;
	avs2_max_src_number = MAX_CU_SIZE_AVS2 * EXTENT_SIZE_AVS2 + 1;

	// (2*width + 2)*(2*height)
	hevc_src = NULL;
	hevc_src_number = (MAX_CU_SIZE_HEVC * SRC_EXTEND_SIZE_HEVC) *(MAX_CU_SIZE_HEVC * SRC_EXTEND_SIZE_HEVC + SRC_EXTEND_SIZE_HEVC);


	h264_src = NULL;
	h264_src_number = (MAX_CU_SIZE_H264 * NUM_EXTEND_SIZE_H264 + 1) * (MAX_CU_SIZE_H264 + 1);

	vp9_src = NULL;
	vp9_src_number = (MAX_CU_SIZE_VP9 * NUM_EXTEND_SIZE_VP9 + 1) * (MAX_CU_SIZE_VP9 + 1);

	generate_mode = mode;
}

void SrcData::initSrcData(char *value) {
	if (strcmp(value, "avs2") == 0) {
		initAvs2SrcData();
	}
	else if (strcmp(value, "hevc") == 0) {
		initHevcSrcData();
	}
	else if (strcmp(value, "h264") == 0) {
		initH264SrcData();
	}
	else if (strcmp(value, "vp9") == 0) {
		initVp9SrcData();
	}

}

void SrcData::initAvs2SrcData() {
	srand((unsigned)time(NULL));
	avs2_src = new int[avs2_max_src_number]();
	int pixel = 0;
	for (int i = 0; i < avs2_max_src_number; i++) {
		if(generate_mode == MODE_RANDOM){
		    avs2_src[i] = (rand() % (1 << 8));
		}else {
			avs2_src[i] = pixel;
			pixel++;
			if (pixel > MAX_PIXEL) {
				pixel = 0;
			}
		}
	}
}


void SrcData::initHevcSrcData() {
	srand((unsigned)time(NULL));
	hevc_src = new int[hevc_src_number]();
	int pixel = 0;
	for (int i = 0; i < hevc_src_number; i++) {
		if (generate_mode == MODE_RANDOM) {
			hevc_src[i] = (rand() % (1 << 8));
		} else {
			hevc_src[i] = pixel;
			pixel++;
			if (pixel > MAX_PIXEL) {
				pixel = 0;
			}
		}
	}
}

void SrcData::initH264SrcData() {
	srand((unsigned)time(NULL));
	h264_src = new int[h264_src_number]();
	int pixel = 0;
	for (int i = 0; i < h264_src_number; i++) {
		if (generate_mode == MODE_RANDOM) {
			h264_src[i] = (rand() % (1 << 8));
		}
		else {
			h264_src[i] = pixel;
			pixel++;
			if (pixel > MAX_PIXEL) {
				pixel = 0;
			}
		}
	}
}

void SrcData::initVp9SrcData() {
	srand((unsigned)time(NULL));
	vp9_src = new uint8_t[vp9_src_number]();
	int pixel = 0;
	for (int i = 0; i < vp9_src_number; i++) {
		if (generate_mode == MODE_RANDOM) {
			vp9_src[i] = (rand() % (1 << 8));
		}
		else {
			vp9_src[i] = pixel;
			pixel++;
			if (pixel > MAX_PIXEL) {
				pixel = 0;
			}
		}
	}
}

SrcData::~SrcData() {
	if (avs2_src) {
		delete[] avs2_src;
		avs2_src = NULL;
	}

	if (hevc_src) {
		delete[] hevc_src;
		hevc_src = NULL;
	}

	if (h264_src) {
		delete[] h264_src;
		h264_src = NULL;
	}

	if (vp9_src) {
		delete[] vp9_src;
		vp9_src = NULL;
	}
}



