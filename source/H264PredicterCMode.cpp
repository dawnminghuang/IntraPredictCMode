#include "H264PredicterCMode.h"


H264PredicterCMode::H264PredicterCMode() {
	max_dst_number = (MAX_CU_SIZE_H264 *NUM_EXTEND_SIZE_H264 + 1)* MAX_CU_SIZE_H264;
	h264_dst = NULL;
	tu_width = MAX_CU_SIZE_H264;
	tu_height = MAX_CU_SIZE_H264;
	block_size = MAX_CU_SIZE_H264;

}

H264PredicterCMode::~H264PredicterCMode() {

}

void H264PredicterCMode::predict() {
	int mode_max_index = NUM_INTRA_PMODE_264 + START_INDEX_264;
	int max_cu_size = 64;
	generateOutPath(H264_PATH, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_264; i++) {
		int uiDirMode = g_prdict_mode_264[i];
		outPutWriter->initDstDataFp(H264_DATA_PATH_CMODE, uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_264; j++) {
			int iWidth = g_cu_size_264[j][0];
			int iHeight = g_cu_size_264[j][1];
			tu_width = iWidth;
			tu_height = iHeight;
			block_size = iWidth;
			initDstData();
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
			predIntraAngAdi(distanMatri, uiDirMode);
			outPutWriter->writeDstDataToFile(h264_dst, iWidth, iHeight);
			deinitDstData();
			delete distanMatri;
		}
	}
}

void H264PredicterCMode::intra4x4_diag_down_right_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[7];
	imgpel PredPel[13];

	// form predictor pels
	// P_A through P_D
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));


	// P_I through P_L
	memcpy(&PredPel[9], refLeft, block_size * sizeof(imgpel));

	// P_X
	PredPel[0] = refLeft[-1];

	PredPixel[0] = (imgpel)((P_L + (P_K << 1) + P_J + 2) >> 2);
	PredPixel[1] = (imgpel)((P_K + (P_J << 1) + P_I + 2) >> 2);
	PredPixel[2] = (imgpel)((P_J + (P_I << 1) + P_X + 2) >> 2);
	PredPixel[3] = (imgpel)((P_I + (P_X << 1) + P_A + 2) >> 2);
	PredPixel[4] = (imgpel)((P_X + 2 * P_A + P_B + 2) >> 2);
	PredPixel[5] = (imgpel)((P_A + 2 * P_B + P_C + 2) >> 2);
	PredPixel[6] = (imgpel)((P_B + 2 * P_C + P_D + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[3], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[2], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[1], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
}


void H264PredicterCMode::intra4x4_diag_down_left_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[8];
	imgpel PredPel[25];

	// form predictor pels
	// P_A through P_D
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));

	refAbove = refAbove + block_size;
	// P_E through P_H
	memcpy(&PredPel[5], refAbove, block_size * sizeof(imgpel));

	PredPixel[0] = (imgpel)((P_A + P_C + 2 * (P_B)+2) >> 2);
	PredPixel[1] = (imgpel)((P_B + P_D + 2 * (P_C)+2) >> 2);
	PredPixel[2] = (imgpel)((P_C + P_E + 2 * (P_D)+2) >> 2);
	PredPixel[3] = (imgpel)((P_D + P_F + 2 * (P_E)+2) >> 2);
	PredPixel[4] = (imgpel)((P_E + P_G + 2 * (P_F)+2) >> 2);
	PredPixel[5] = (imgpel)((P_F + P_H + 2 * (P_G)+2) >> 2);
	PredPixel[6] = (imgpel)((P_G + 3 * (P_H)+2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[1], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[2], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[3], 4 * sizeof(imgpel));
}


void H264PredicterCMode::intra4x4_vert_right_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[10];
	imgpel PredPel[13];

	// form predictor pels

	// P_A through P_D
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	// P_I through P_L
	memcpy(&PredPel[9], refLeft, block_size * sizeof(imgpel));
	// P_X
	PredPel[0] = refLeft[-1];

	PredPixel[0] = (imgpel)((P_X + (P_I << 1) + P_J + 2) >> 2);
	PredPixel[1] = (imgpel)((P_X + P_A + 1) >> 1);
	PredPixel[2] = (imgpel)((P_A + P_B + 1) >> 1);
	PredPixel[3] = (imgpel)((P_B + P_C + 1) >> 1);
	PredPixel[4] = (imgpel)((P_C + P_D + 1) >> 1);
	PredPixel[5] = (imgpel)((P_I + (P_J << 1) + P_K + 2) >> 2);
	PredPixel[6] = (imgpel)((P_I + (P_X << 1) + P_A + 2) >> 2);
	PredPixel[7] = (imgpel)((P_X + 2 * P_A + P_B + 2) >> 2);
	PredPixel[8] = (imgpel)((P_A + 2 * P_B + P_C + 2) >> 2);
	PredPixel[9] = (imgpel)((P_B + 2 * P_C + P_D + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[1], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[6], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[5], 4 * sizeof(imgpel));
}

void H264PredicterCMode::intra4x4_vert_left_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[10];
	imgpel PredPel[13];

	// P_A through P_D
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));

	refAbove = refAbove + block_size;
	// P_E through P_H
	memcpy(&PredPel[5], refAbove, block_size * sizeof(imgpel));

	PredPixel[0] = (imgpel)((P_A + P_B + 1) >> 1);
	PredPixel[1] = (imgpel)((P_B + P_C + 1) >> 1);
	PredPixel[2] = (imgpel)((P_C + P_D + 1) >> 1);
	PredPixel[3] = (imgpel)((P_D + P_E + 1) >> 1);
	PredPixel[4] = (imgpel)((P_E + P_F + 1) >> 1);
	PredPixel[5] = (imgpel)((P_A + 2 * P_B + P_C + 2) >> 2);
	PredPixel[6] = (imgpel)((P_B + 2 * P_C + P_D + 2) >> 2);
	PredPixel[7] = (imgpel)((P_C + 2 * P_D + P_E + 2) >> 2);
	PredPixel[8] = (imgpel)((P_D + 2 * P_E + P_F + 2) >> 2);
	PredPixel[9] = (imgpel)((P_E + 2 * P_F + P_G + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[5], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[1], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[6], 4 * sizeof(imgpel));
}


void H264PredicterCMode::intra4x4_hor_up_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[10];
	imgpel PredPel[13];

	// form predictor pels

	// P_I through P_L
	memcpy(&PredPel[9], refLeft, block_size * sizeof(imgpel));

	PredPixel[0] = (imgpel)((P_I + P_J + 1) >> 1);
	PredPixel[1] = (imgpel)((P_I + (P_J << 1) + P_K + 2) >> 2);
	PredPixel[2] = (imgpel)((P_J + P_K + 1) >> 1);
	PredPixel[3] = (imgpel)((P_J + (P_K << 1) + P_L + 2) >> 2);
	PredPixel[4] = (imgpel)((P_K + P_L + 1) >> 1);
	PredPixel[5] = (imgpel)((P_K + (P_L << 1) + P_L + 2) >> 2);
	PredPixel[6] = (imgpel)P_L;
	PredPixel[7] = (imgpel)P_L;
	PredPixel[8] = (imgpel)P_L;
	PredPixel[9] = (imgpel)P_L;

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[2], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[4], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[6], 4 * sizeof(imgpel));
}


void H264PredicterCMode::intra4x4_hor_down_pred(int *refAbove, int *refLeft) {

	imgpel PredPixel[10];
	imgpel PredPel[13];

	// form predictor pels

	// P_A through P_D
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	// P_I through P_L
	memcpy(&PredPel[9], refLeft, block_size * sizeof(imgpel));
	// P_X
	PredPel[0] = refLeft[-1];

	PredPixel[0] = (imgpel)((P_K + P_L + 1) >> 1);
	PredPixel[1] = (imgpel)((P_J + (P_K << 1) + P_L + 2) >> 2);
	PredPixel[2] = (imgpel)((P_J + P_K + 1) >> 1);
	PredPixel[3] = (imgpel)((P_I + (P_J << 1) + P_K + 2) >> 2);
	PredPixel[4] = (imgpel)((P_I + P_J + 1) >> 1);
	PredPixel[5] = (imgpel)((P_X + (P_I << 1) + P_J + 2) >> 2);
	PredPixel[6] = (imgpel)((P_X + P_I + 1) >> 1);
	PredPixel[7] = (imgpel)((P_I + (P_X << 1) + P_A + 2) >> 2);
	PredPixel[8] = (imgpel)((P_X + 2 * P_A + P_B + 2) >> 2);
	PredPixel[9] = (imgpel)((P_A + 2 * P_B + P_C + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[6], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[4], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredPixel[2], 4 * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredPixel[0], 4 * sizeof(imgpel));
}


void H264PredicterCMode::intra8x8_diag_down_right_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[16];
	imgpel PredPel[25];

	// form predictor pels

	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));
	// P_Z
	PredPel[0] = refLeft[-1];

	// Mode DIAG_DOWN_RIGHT_PRED
	PredArray[0] = (imgpel)((PP_X + PP_V + ((PP_W) << 1) + 2) >> 2);
	PredArray[1] = (imgpel)((PP_W + PP_U + ((PP_V) << 1) + 2) >> 2);
	PredArray[2] = (imgpel)((PP_V + PP_T + ((PP_U) << 1) + 2) >> 2);
	PredArray[3] = (imgpel)((PP_U + PP_S + ((PP_T) << 1) + 2) >> 2);
	PredArray[4] = (imgpel)((PP_T + PP_R + ((PP_S) << 1) + 2) >> 2);
	PredArray[5] = (imgpel)((PP_S + PP_Q + ((PP_R) << 1) + 2) >> 2);
	PredArray[6] = (imgpel)((PP_R + PP_Z + ((PP_Q) << 1) + 2) >> 2);
	PredArray[7] = (imgpel)((PP_Q + PP_A + ((PP_Z) << 1) + 2) >> 2);
	PredArray[8] = (imgpel)((PP_Z + PP_B + ((PP_A) << 1) + 2) >> 2);
	PredArray[9] = (imgpel)((PP_A + PP_C + ((PP_B) << 1) + 2) >> 2);
	PredArray[10] = (imgpel)((PP_B + PP_D + ((PP_C) << 1) + 2) >> 2);
	PredArray[11] = (imgpel)((PP_C + PP_E + ((PP_D) << 1) + 2) >> 2);
	PredArray[12] = (imgpel)((PP_D + PP_F + ((PP_E) << 1) + 2) >> 2);
	PredArray[13] = (imgpel)((PP_E + PP_G + ((PP_F) << 1) + 2) >> 2);
	PredArray[14] = (imgpel)((PP_F + PP_H + ((PP_G) << 1) + 2) >> 2);
	imgpel *pred_pels;
	pred_pels = &PredArray[7];
	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels--, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], pred_pels, block_size * sizeof(imgpel));
}


void H264PredicterCMode::intra8x8_diag_down_left_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[16];
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels

	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));
	// P_Z
	PredPel[0] = refLeft[-1];

	// Mode DIAG_DOWN_LEFT_PRED
	*Pred++ = (imgpel)((PP_A + PP_C + ((PP_B) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_B + PP_D + ((PP_C) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_C + PP_E + ((PP_D) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_D + PP_F + ((PP_E) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_E + PP_G + ((PP_F) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_F + PP_H + ((PP_G) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_G + PP_I + ((PP_H) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_H + PP_J + ((PP_I) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_I + PP_K + ((PP_J) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_J + PP_L + ((PP_K) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_K + PP_M + ((PP_L) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_L + PP_N + ((PP_M) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_M + PP_O + ((PP_N) << 1) + 2) >> 2);
	*Pred++ = (imgpel)((PP_N + PP_P + ((PP_O) << 1) + 2) >> 2);
	*Pred = (imgpel)((PP_O + PP_P + ((PP_P) << 1) + 2) >> 2);

	Pred = &PredArray[0];
	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], Pred++, block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], Pred, block_size * sizeof(imgpel));
}

void H264PredicterCMode::intra8x8_vert_right_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[22];  // array of final prediction values
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels

	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));

	// P_Z
	PredPel[0] = refLeft[-1];

	imgpel *pred_pels;
	pred_pels = PredArray;
	*pred_pels++ = (imgpel)((PP_V + PP_T + ((PP_U) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_T + PP_R + ((PP_S) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_R + PP_Z + ((PP_Q) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_Z + PP_A + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_A + PP_B + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_B + PP_C + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_C + PP_D + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_D + PP_E + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_E + PP_F + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_F + PP_G + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_G + PP_H + 1) >> 1);

	*pred_pels++ = (imgpel)((PP_W + PP_U + ((PP_V) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_U + PP_S + ((PP_T) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_S + PP_Q + ((PP_R) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_Q + PP_A + ((PP_Z) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_Z + PP_B + ((PP_A) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_A + PP_C + ((PP_B) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_B + PP_D + ((PP_C) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_C + PP_E + ((PP_D) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_D + PP_F + ((PP_E) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_E + PP_G + ((PP_F) << 1) + 2) >> 2);
	*pred_pels = (imgpel)((PP_F + PP_H + ((PP_G) << 1) + 2) >> 2);

	Pred = &PredArray[0];
	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[3], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[14], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[2], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[13], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[1], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[12], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[0], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredArray[11], block_size * sizeof(imgpel));
}

void H264PredicterCMode::intra8x8_vert_left_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[22];  // array of final prediction values
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels

	// PP_A through PP_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// PP_I through PP_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// PP_Q through PP_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));

	// PP_Z
	PredPel[0] = refLeft[-1];

	imgpel *pred_pel = &PredArray[0];
	*pred_pel++ = (imgpel)((PP_A + PP_B + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_B + PP_C + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_C + PP_D + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_D + PP_E + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_E + PP_F + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_F + PP_G + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_G + PP_H + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_H + PP_I + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_I + PP_J + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_J + PP_K + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_K + PP_L + 1) >> 1);
	*pred_pel++ = (imgpel)((PP_A + PP_C + ((PP_B) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_B + PP_D + ((PP_C) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_C + PP_E + ((PP_D) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_D + PP_F + ((PP_E) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_E + PP_G + ((PP_F) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_F + PP_H + ((PP_G) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_G + PP_I + ((PP_H) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_H + PP_J + ((PP_I) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_I + PP_K + ((PP_J) << 1) + 2) >> 2);
	*pred_pel++ = (imgpel)((PP_J + PP_L + ((PP_K) << 1) + 2) >> 2);
	*pred_pel = (imgpel)((PP_K + PP_M + ((PP_L) << 1) + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[0], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[11], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[1], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[12], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[2], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[13], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[3], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredArray[14], block_size * sizeof(imgpel));
}

void H264PredicterCMode::intra8x8_hor_up_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[22];  // array of final prediction values
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels

	// PP_A through PP_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// PP_I through PP_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// PP_Q through PP_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));

	// PP_Z
	PredPel[0] = refLeft[-1];

	PredArray[0] = (imgpel)((PP_Q + PP_R + 1) >> 1);
	PredArray[1] = (imgpel)((PP_S + PP_Q + ((PP_R) << 1) + 2) >> 2);
	PredArray[2] = (imgpel)((PP_R + PP_S + 1) >> 1);
	PredArray[3] = (imgpel)((PP_T + PP_R + ((PP_S) << 1) + 2) >> 2);
	PredArray[4] = (imgpel)((PP_S + PP_T + 1) >> 1);
	PredArray[5] = (imgpel)((PP_U + PP_S + ((PP_T) << 1) + 2) >> 2);
	PredArray[6] = (imgpel)((PP_T + PP_U + 1) >> 1);
	PredArray[7] = (imgpel)((PP_V + PP_T + ((PP_U) << 1) + 2) >> 2);
	PredArray[8] = (imgpel)((PP_U + PP_V + 1) >> 1);
	PredArray[9] = (imgpel)((PP_W + PP_U + ((PP_V) << 1) + 2) >> 2);
	PredArray[10] = (imgpel)((PP_V + PP_W + 1) >> 1);
	PredArray[11] = (imgpel)((PP_X + PP_V + ((PP_W) << 1) + 2) >> 2);
	PredArray[12] = (imgpel)((PP_W + PP_X + 1) >> 1);
	PredArray[13] = (imgpel)((PP_W + PP_X + ((PP_X) << 1) + 2) >> 2);
	PredArray[14] = (imgpel)PP_X;
	PredArray[15] = (imgpel)PP_X;
	PredArray[16] = (imgpel)PP_X;
	PredArray[17] = (imgpel)PP_X;
	PredArray[18] = (imgpel)PP_X;
	PredArray[19] = (imgpel)PP_X;
	PredArray[20] = (imgpel)PP_X;
	PredArray[21] = (imgpel)PP_X;

	int start_offx = 0;
	int start_offy = 0;
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[0], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[2], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[4], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[6], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[8], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[10], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy++][start_offx], &PredArray[12], block_size * sizeof(imgpel));
	memcpy(&h264_dst[start_offy][start_offx], &PredArray[14], block_size * sizeof(imgpel));
}

void H264PredicterCMode::intra8x8_hor_down_pred(int *refAbove, int *refLeft) {

	imgpel PredArray[22];  // array of final prediction values
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels

	// PP_A through PP_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// PP_I through PP_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// PP_Q through PP_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));

	// PP_Z
	PredPel[0] = refLeft[-1];
	imgpel *pred_pels;
	pred_pels = PredArray;

	*pred_pels++ = (imgpel)((PP_X + PP_W + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_V + PP_X + (PP_W << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_W + PP_V + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_U + PP_W + ((PP_V) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_V + PP_U + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_T + PP_V + ((PP_U) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_U + PP_T + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_S + PP_U + ((PP_T) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_T + PP_S + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_R + PP_T + ((PP_S) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_S + PP_R + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_Q + PP_S + ((PP_R) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_R + PP_Q + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_Z + PP_R + ((PP_Q) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_Q + PP_Z + 1) >> 1);
	*pred_pels++ = (imgpel)((PP_Q + PP_A + ((PP_Z) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_Z + PP_B + ((PP_A) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_A + PP_C + ((PP_B) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_B + PP_D + ((PP_C) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_C + PP_E + ((PP_D) << 1) + 2) >> 2);
	*pred_pels++ = (imgpel)((PP_D + PP_F + ((PP_E) << 1) + 2) >> 2);
	*pred_pels = (imgpel)((PP_E + PP_G + ((PP_F) << 1) + 2) >> 2);

	int start_offx = 0;
	int start_offy = 0;
	pred_pels = &PredArray[14];
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy++][start_offx], pred_pels, block_size * sizeof(imgpel));
	pred_pels -= 2;
	memcpy(&h264_dst[start_offy][start_offx], pred_pels, block_size * sizeof(imgpel));
}

void H264PredicterCMode::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above;
	int *refLeft = Left + 1;
	if (iWidth == 4 && iHeight == 4) {
		switch (uiDirMode) {
		case DIAG_DOWN_RIGHT_PRED:
			intra4x4_diag_down_right_pred(refAbove, refLeft);
			break;
		case DIAG_DOWN_LEFT_PRED:
			intra4x4_diag_down_left_pred(refAbove, refLeft);
			break;
		case VERT_RIGHT_PRED:
			intra4x4_vert_right_pred(refAbove, refLeft);
			break;
		case VERT_LEFT_PRED:
			intra4x4_vert_left_pred(refAbove, refLeft);
			break;
		case HOR_UP_PRED:
			intra4x4_hor_up_pred(refAbove, refLeft);
			break;
		case HOR_DOWN_PRED:
			intra4x4_hor_down_pred(refAbove, refLeft);
			break;
		default:
			printf("Error: illegal intra_4x4 prediction mode: %d\n", (int)uiDirMode);
			break;
		}
	}

	if (iWidth == 8 && iHeight == 8) {
		switch (uiDirMode) {
		case DIAG_DOWN_RIGHT_PRED:
			intra8x8_diag_down_right_pred(refAbove, refLeft);
			break;
		case DIAG_DOWN_LEFT_PRED:
			intra8x8_diag_down_left_pred(refAbove, refLeft);
			break;
		case VERT_RIGHT_PRED:
			intra8x8_vert_right_pred(refAbove, refLeft);
			break;
		case VERT_LEFT_PRED:
			intra8x8_vert_left_pred(refAbove, refLeft);
			break;
		case HOR_UP_PRED:
			intra8x8_hor_up_pred(refAbove, refLeft);
			break;
		case HOR_DOWN_PRED:
			intra8x8_hor_down_pred(refAbove, refLeft);
			break;
		default:
			printf("Error: illegal intra_8x8 prediction mode: %d\n", (int)uiDirMode);
			break;
		}
	}
}

void H264PredicterCMode::convertSrc(int* above, int *left) {
	if ((src_data && src_data->h264_src)) {
		int *h264_src_ptr = src_data->h264_src + 1;
		int src_stride = MAX_CU_SIZE_H264 * NUM_EXTEND_SIZE_H264 + 1;
		int max_width = MAX_CU_SIZE_H264 * NUM_EXTEND_SIZE_H264;
		for (int i = 0; i < max_width; i++) {
			above[i] = h264_src_ptr[i];
		}

		for (int i = 0; i <= MAX_CU_SIZE_H264; i++) {
			left[i] = src_data->h264_src[i*src_stride];
		}
	}
}


void H264PredicterCMode::initDstData() {
	h264_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		h264_dst[i] = new int[tu_width]();
	}
}

void H264PredicterCMode::deinitDstData() {
	if (h264_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] h264_dst[i];
		}
		delete[] h264_dst;
	}
}


