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
	int mode_max_index = NUM_INTRA_PMODE_264;
	int max_cu_size = 64;
	generateOutPath(H264_PATH, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_264; i++) {
		int uiDirMode = g_prdict_mode_264[i];
		outPutWriter->initDstDataFp(H264_DATA_PATH_CMODE, uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_264; j++) {
			for (int k = 0; k < NUM_COLOR_SPACE_SIZE; k++) {
				if (k == COLOR_SPACE_LUMA) {
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
				else {
					int iWidth = g_cu_size_264[j][0]/2;
					int iHeight = g_cu_size_264[j][1]/2;
					tu_width = iWidth;
					tu_height = iHeight;
					block_size = iWidth;
					initDstData();
					DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
					predIntraChromaAdi(distanMatri, uiDirMode);
					outPutWriter->writeDstDataToFile(h264_dst, iWidth, iHeight);
					deinitDstData();
					delete distanMatri;
				}
			}
		}
	}
}
void H264PredicterCMode::intra4x4_dc_pred(int *refAbove, int *refLeft) {
	int s0 = 0;
	// form predictor pels

	imgpel *curpel = NULL;
	curpel = refAbove;
	s0 += *curpel++;
	s0 += *curpel++;
	s0 += *curpel++;
	s0 += *curpel;

	curpel = refLeft;
	s0 += *curpel++;
	s0 += *curpel++;
	s0 += *curpel++;
	s0 += *curpel;

	// no edge
	s0 = (s0 + 4) >> 3;
	int joff = 0;
	int ioff = 0;
	for (int j = joff; j < joff + block_size; ++j)
	{
		// store DC prediction
		h264_dst[j][ioff] = (imgpel)s0;
		h264_dst[j][ioff + 1] = (imgpel)s0;
		h264_dst[j][ioff + 2] = (imgpel)s0;
		h264_dst[j][ioff + 3] = (imgpel)s0;
	}
}

void H264PredicterCMode::intra4x4_vert_pred(int *refAbove, int *refLeft) {
	int joff = 0;
	int ioff = 0;
	int *imgY = refAbove;
	memcpy(&(h264_dst[joff++][ioff]), imgY, block_size * sizeof(imgpel));
	memcpy(&(h264_dst[joff++][ioff]), imgY, block_size * sizeof(imgpel));
	memcpy(&(h264_dst[joff++][ioff]), imgY, block_size * sizeof(imgpel));
	memcpy(&(h264_dst[joff][ioff]), imgY, block_size * sizeof(imgpel));
}
void H264PredicterCMode::intra4x4_hor_pred(int *refAbove, int *refLeft) {
	int ioff = 0;
	imgpel *predrow, prediction;
	for (int j = 0; j < block_size; ++j)
	{
		predrow = h264_dst[j];
		prediction = refLeft[j];
		/* store predicted 4x4 block */
		predrow[ioff] = prediction;
		predrow[ioff + 1] = prediction;
		predrow[ioff + 2] = prediction;
		predrow[ioff + 3] = prediction;
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


void H264PredicterCMode::intra8x8_dc_pred(int *refAbove, int *refLeft) {
	imgpel PredArray[16];
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels
	int s0 = 0;
	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));
	// P_Z
	PredPel[0] = refLeft[-1];

	s0 = (PP_A + PP_B + PP_C + PP_D + PP_E + PP_F + PP_G + PP_H + PP_Q + PP_R + PP_S + PP_T + PP_U + PP_V + PP_W + PP_X + 8) >> 4;

	// no edge
	int ioff = 0;
	int joff = 0;
	for (int i = ioff; i < ioff + BLOCK_SIZE_8x8; i++)
		h264_dst[joff][i] = (imgpel)s0;

	for (int j = joff + 1; j < joff + BLOCK_SIZE_8x8; j++)
		memcpy(&h264_dst[j][ioff], &h264_dst[j - 1][ioff], BLOCK_SIZE_8x8 * sizeof(imgpel));
}

void H264PredicterCMode::intra8x8_vert_pred(int *refAbove, int *refLeft) {
	imgpel PredArray[16];
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels
	int s0 = 0;
	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));
	// P_Z
	PredPel[0] = refLeft[-1];
	int joff = 0;
	int ioff = 0;
	for (int i = joff; i < joff + BLOCK_SIZE_8x8; i++)
	{
		memcpy(&h264_dst[i][ioff], &PredPel[1], BLOCK_SIZE_8x8 * sizeof(imgpel));
	}
}
void H264PredicterCMode::intra8x8_hor_pred(int *refAbove, int *refLeft) {
	imgpel PredArray[16];
	imgpel PredPel[25];
	imgpel *Pred = &PredArray[0];
	// form predictor pels
	int s0 = 0;
	// P_A through P_H
	memcpy(&PredPel[1], refAbove, block_size * sizeof(imgpel));
	refAbove = refAbove + block_size;
	// P_I through P_P
	memcpy(&PredPel[9], refAbove, block_size * sizeof(imgpel));

	// P_Q through P_X
	memcpy(&PredPel[17], refLeft, block_size * sizeof(imgpel));
	// P_Z
	PredPel[0] = refLeft[-1];
	int ioff = 0;
	int joff = 0;
	int jpos;
	int ipos0 = ioff, ipos1 = ioff + 1, ipos2 = ioff + 2, ipos3 = ioff + 3;
	int ipos4 = ioff + 4, ipos5 = ioff + 5, ipos6 = ioff + 6, ipos7 = ioff + 7;

	for (int j = 0; j < BLOCK_SIZE_8x8; j++)
	{
		jpos = j + joff;

		h264_dst[jpos][ipos0] =
		h264_dst[jpos][ipos1] =
		h264_dst[jpos][ipos2] =
		h264_dst[jpos][ipos3] =
		h264_dst[jpos][ipos4] =
		h264_dst[jpos][ipos5] =
		h264_dst[jpos][ipos6] =
		h264_dst[jpos][ipos7] = (imgpel)(&PP_Q)[j];

	}
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

void H264PredicterCMode::intra16x16_dc_pred(int *refAbove, int *refLeft) {
	int s0 = 0;
	int s1 = 0;
	int s2 = 0;
	// form predictor pels
	int *pel = refAbove;
	for (int i = 0; i < MB_BLOCK_SIZE; ++i)
	{
		s1 += *pel++;
	}
	pel = refLeft;
	for (int i = 0; i < MB_BLOCK_SIZE; ++i)
	{
		s2 += *pel++;
	}
	// no edge
	s0 = (s1 + s2 + 16) >> 5;       // no edge
	int joff = 0;
	int ioff = 0;
	for (int j = 0; j < MB_BLOCK_SIZE; ++j)
	{
		for (int i = 0; i < MB_BLOCK_SIZE; i += 4)
		{
			h264_dst[j][i] = (imgpel)s0;
			h264_dst[j][i + 1] = (imgpel)s0;
			h264_dst[j][i + 2] = (imgpel)s0;
			h264_dst[j][i + 3] = (imgpel)s0;
		}
	}
}

void H264PredicterCMode::intra16x16_vert_pred(int *refAbove, int *refLeft) {
	int joff = 0;
	int ioff = 0;
	int *src = refAbove;
	int **dst = h264_dst;
	for (int j = 0; j < MB_BLOCK_SIZE; j += 4)
	{
		memcpy(*dst++, src, MB_BLOCK_SIZE * sizeof(imgpel));
		memcpy(*dst++, src, MB_BLOCK_SIZE * sizeof(imgpel));
		memcpy(*dst++, src, MB_BLOCK_SIZE * sizeof(imgpel));
		memcpy(*dst++, src, MB_BLOCK_SIZE * sizeof(imgpel));
	}
}
void H264PredicterCMode::intra16x16_hor_pred(int *refAbove, int *refLeft) {
	int prediction;
	int pos_y = 0;
	for (int j = 0; j < MB_BLOCK_SIZE; ++j)
	{
		int i;
		imgpel *prd = h264_dst[j];
		prediction = refLeft[pos_y++];

		for (i = 0; i < MB_BLOCK_SIZE; i += 4)
		{
			*prd++ = prediction; // store predicted 16x16 block
			*prd++ = prediction; // store predicted 16x16 block
			*prd++ = prediction; // store predicted 16x16 block
			*prd++ = prediction; // store predicted 16x16 block
		}

	}
}
void H264PredicterCMode::intra16x16_plane_pred(int *refAbove, int *refLeft) {
	int i, j;

	int ih = 0, iv = 0;
	int ib, ic, iaa;
	int *mpr_line = refAbove + 7;

	int *imgY = refLeft + 7;
	for (int i = 1; i < 8; ++i)
	{
		ih += i * (mpr_line[i] - mpr_line[-i]);
		iv += i * (imgY[i]- imgY[-i]);
	}

	ih += 8 * (mpr_line[8] - refLeft[-1]);
	iv += 8 * (imgY[8] - refLeft[-1]);

	ib = (5 * ih + 32) >> 6;
	ic = (5 * iv + 32) >> 6;
	iaa = 16 * (mpr_line[8] + imgY[8]);
	for (j = 0; j < MB_BLOCK_SIZE; ++j)
	{
		int ibb = iaa + (j - 7) * ic + 16;
		imgpel *prd = h264_dst[j];
		for (i = 0; i < MB_BLOCK_SIZE; i += 4)
		{
			*prd++ = (imgpel)iClip1(MAX_PIXEL_VALUE, ((ibb + (i - 7) * ib) >> 5));
			*prd++ = (imgpel)iClip1(MAX_PIXEL_VALUE, ((ibb + (i - 6) * ib) >> 5));
			*prd++ = (imgpel)iClip1(MAX_PIXEL_VALUE, ((ibb + (i - 5) * ib) >> 5));
			*prd++ = (imgpel)iClip1(MAX_PIXEL_VALUE, ((ibb + (i - 4) * ib) >> 5));
		}
	}// store plane prediction
}
void H264PredicterCMode::intrapred_chroma_plane(int *refAbove, int *refLeft) {
	int ih, iv, ib, ic, iaa;
	int i, j;
	int cr_MB_x =tu_width;
	int cr_MB_y = tu_height;
	int cr_MB_y2 = (cr_MB_y >> 1);
	int cr_MB_x2 = (cr_MB_x >> 1);

	//imgpel **predU1 = &imgUV[pos_y1];

	ih = cr_MB_x2 * (refAbove[cr_MB_x - 1] - refLeft[-1]);

	for (i = 0; i < cr_MB_x2 - 1; ++i)
		ih += (i + 1) * (refAbove[cr_MB_x2 + i] - refAbove[cr_MB_x2 - 2 - i]);

	iv = cr_MB_y2 * (refLeft[cr_MB_y - 1] - refLeft[-1]);

	for (i = 0; i < cr_MB_y2 - 1; ++i)
	{
		iv += (i + 1)*((refLeft[cr_MB_y2 + i] - refLeft[cr_MB_y2 - 2 - i]));
	}

	ib = ((cr_MB_x == 8 ? 17 : 5) * ih + 2 * cr_MB_x) >> (cr_MB_x == 8 ? 5 : 6);
	ic = ((cr_MB_y == 8 ? 17 : 5) * iv + 2 * cr_MB_y) >> (cr_MB_y == 8 ? 5 : 6);

	iaa = ((refAbove[cr_MB_x - 1] + refLeft[cr_MB_y - 1]) << 4);

	for (j = 0; j < cr_MB_y; ++j)
	{
		int plane = iaa + (j - cr_MB_y2 + 1) * ic + 16 - (cr_MB_x2 - 1) * ib;
		for (i = 0; i < cr_MB_x; ++i)
			h264_dst[j][i] = (imgpel)iClip1(MAX_PIXEL_VALUE, ((i * ib + plane) >> 5));
	}

}
void H264PredicterCMode::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	iWidth = distanMatri->tu_width;
    iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above;
	int *refLeft = Left + 1;
	if (iWidth == 4 && iHeight == 4) {
		switch (uiDirMode) {
		case DC_PRED:
			return (intra4x4_dc_pred(refAbove, refLeft));
			break;
		case VERT_PRED:
			return (intra4x4_vert_pred(refAbove, refLeft));
			break;
		case HOR_PRED:
			return (intra4x4_hor_pred(refAbove, refLeft));
			break;
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
			//printf("Error: illegal intra_4x4 prediction mode: %d\n", (int)uiDirMode);
			break;
		}
	}

	if (iWidth == 8 && iHeight == 8) {
		switch (uiDirMode) {
		case DC_PRED:
			return (intra8x8_dc_pred(refAbove, refLeft));
			break;
		case VERT_PRED:
			return (intra8x8_vert_pred(refAbove, refLeft));
			break;
		case HOR_PRED:
			return (intra8x8_hor_pred(refAbove, refLeft));
			break;
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
			//printf("Error: illegal intra_8x8 prediction mode: %d\n", (int)uiDirMode);
			break;
		}
	}
    if (iWidth == 16 && iHeight == 16) {
			switch (uiDirMode) {
			case DC_PRED:
				return (intra16x16_dc_pred(refAbove, refLeft));
				break;
			case VERT_PRED:
				return (intra16x16_vert_pred(refAbove, refLeft));
				break;
			case HOR_PRED:
				return (intra16x16_hor_pred(refAbove, refLeft));
				break;
			case PLANE_16:
				return intra16x16_plane_pred(refAbove, refLeft);
				break;
			}
	    }
}

void H264PredicterCMode::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	predIntraAngAdi(distanMatri, uiDirMode);
}

void H264PredicterCMode::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above;
	int *refLeft = Left + 1;
	if (iWidth == 4 && iHeight == 4) {
		switch (uiDirMode) {
		case DC_PRED_CHROMA:
			return (intra4x4_dc_pred(refAbove, refLeft));
			break;
		case VERT_PRED_CHROMA:
			return (intra4x4_vert_pred(refAbove, refLeft));
			break;
		case HOR_PRED_CHROMA:
			return (intra4x4_hor_pred(refAbove, refLeft));
			break;
		case PLANE_PRED_CHROMA:
			intrapred_chroma_plane(refAbove, refLeft);
			break;
		default:
			//printf("Error: illegal intra_4x4 prediction mode: %d\n", (int)uiDirMode);
			break;
		}
	}

	if (iWidth == 8 && iHeight == 8) {
		switch (uiDirMode) {
		case DC_PRED_CHROMA:
			return (intra8x8_dc_pred(refAbove, refLeft));
			break;
		case VERT_PRED_CHROMA:
			return (intra8x8_vert_pred(refAbove, refLeft));
			break;
		case HOR_PRED_CHROMA:
			return (intra8x8_hor_pred(refAbove, refLeft));
			break;
		case PLANE_PRED_CHROMA:
			intrapred_chroma_plane(refAbove, refLeft);
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
void H264PredicterCMode::setBlockSize(int  size) {
	block_size = size;
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
		h264_dst = NULL;
	}
}


