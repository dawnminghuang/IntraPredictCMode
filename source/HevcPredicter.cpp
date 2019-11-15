#include "HevcPredicter.h"


HevcPredicter::HevcPredicter() {
	max_dst_number = MAX_CU_SIZE_HEVC * MAX_CU_SIZE_HEVC;
	hevc_dst = NULL;
	dst_stride_True = MAX_CU_SIZE_HEVC;
}

HevcPredicter::~HevcPredicter() {

}

void HevcPredicter::predict() {
	int mode_number = NUM_INTRA_PMODE_HEVC + START_INDEX_HEVC;
	int max_cu_size = 64;
	generateOutPath(HEVC_PATH, calc_mode);
	generateDigOutPath(HEVC_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_HEVC; i++) {
		int uiDirMode = g_prdict_mode_hevc[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		outPutWriter->initDstDataFp(HEVC_DATA_PATH, uiDirMode);
		outPutWriter->initDigPostionInfoFp(digOutPath, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		computeIntraPredAngle(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			int iWidth = g_cu_size_hevc[j][0];
			int iHeight = g_cu_size_hevc[j][1];
			initDstData();
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			outPutWriter->writeModeInfoToFile(distanMatri);
			outPutWriter->writeDstDataToFile(hevc_dst, iWidth, iHeight, dst_stride_True);
			deinitDstData();
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}

void HevcPredicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;
	int *refMain;
	int *refSide;
	int refAbove[2 * MAX_CU_SIZE_HEVC + 1];
	int refLeft[2 * MAX_CU_SIZE_HEVC + 1];
	int *pSrc = NULL;
	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}
	int delta_pos = 0;
	int delta_int = 0;
	int delta_fract = 0;
	if (intra_pred_angle < 0) {
		const int refMainOffsetPreScale = (is_mod_ver ? iHeight : iWidth) - 1;
		const int refMainOffset = iHeight - 1;
		for (int x = 0; x < iWidth + 1; x++)
		{
			refAbove[x + refMainOffset] = pSrc[x - srcStride - 1];
		}
		for (int y = 0; y < iHeight + 1; y++)
		{
			refLeft[y + refMainOffset] = pSrc[(y - 1)*srcStride - 1];
		}
		refMain = (is_mod_ver ? refAbove : refLeft) + refMainOffset;
		refSide = (is_mod_ver ? refLeft : refAbove) + refMainOffset;

		// Extend the Main reference to the left.
		int invAngleSum = 128;       // rounding for (shift by 8)
		for (int k = -1; k > (refMainOffsetPreScale + 1)*intra_pred_angle >> 5; k--)
		{
			invAngleSum += inv_angle;
			refMain[k] = refSide[invAngleSum >> 8];
		}

	}
	else {
		for (int x = 0; x < 2 * iWidth + 1; x++)
		{
			refAbove[x] = pSrc[x - srcStride - 1];
		}
		for (int y = 0; y < 2 * iHeight + 1; y++)
		{
			refLeft[y] = pSrc[(y - 1)*srcStride - 1];
		}
		refMain = is_mod_ver ? refAbove : refLeft;
		refSide = is_mod_ver ? refLeft : refAbove;
	}
	const int dstStride = is_mod_ver ? dst_stride_True : MAX_CU_SIZE_HEVC;
	int *pDst = hevc_dst;
	if (intra_pred_angle == 0) {
		if(is_mod_ver){
			for (int y = 0; y < iHeight; y++)
			{
				for (int x = 0; x < iWidth; x++)
				{
					pDst[y*dstStride + x] = refMain[x + 1];
					saveDistanceMatri(distanMatri, x, y, x + 1, x + 1);
				}
			}
		}
		else {
			for (int x = 0; x < iWidth; x++)
			{
				for (int y = 0; y < iHeight; y++)
				{
					pDst[y*dstStride + x] = refMain[y + 1];
					saveDistanceMatri(distanMatri, x, y, y + 1, y + 1);
				}
			}
		
		}

	}
	else {
		int *pDsty = pDst;
		if (is_mod_ver) {
			for (int j = 0; j < iHeight; j++) {
				delta_pos = delta_pos + intra_pred_angle;
				delta_int = delta_pos >> 5;
				delta_fract = delta_pos & (32 - 1);
				if (delta_fract) {
					for (int i = 0; i < iWidth; i++) {
						int ref_main_index = i + delta_int +1;
						int ref_side_index = ref_main_index + 1;
						pDsty[i + dst_stride_True*j] = ((32 - delta_fract)*refMain[ref_main_index] + delta_fract * refMain[ref_side_index] + 16) >> 5;

						saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
					}
				}
				else {
					// Just copy the integer samples
					for (int i = 0; i < iWidth; i++) {
						pDsty[i+ dst_stride_True * j] = refMain[i + delta_int + 1];
						saveDistanceMatri(distanMatri, i, j, i + delta_int + 1, i + delta_int + 1);
					}
				}
			}
		}
		else {
			for (int i = 0; i < iWidth; i++) {
				delta_pos = delta_pos + intra_pred_angle;
				delta_int = delta_pos >> 5;
				delta_fract = delta_pos & (32 - 1);
				if (delta_fract) {
					for (int j = 0; j < iHeight; j++) {
						int ref_main_index = delta_int + j + 1;
						int ref_side_index = ref_main_index + 1;
						pDsty[j*dst_stride_True + i] = (((32 - delta_fract)*refMain[ref_main_index] + delta_fract * refMain[ref_side_index])+16 )>> 5;
						saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
					}
				}
				else {
					// Just copy the integer samples
					for (int j = 0; j < iHeight; j++) {
						pDsty[j*dst_stride_True + i] = refMain[delta_int +j + 1];
						saveDistanceMatri(distanMatri, i, j, delta_int + j + 1, delta_int + j + 1);
					}
				}
			}
		}
	}
}



int HevcPredicter::computeIntraPredAngle(int uiDirMode) {
	is_mod_ver = (uiDirMode >= VER_HOR_IDX);
	if (is_mod_ver) {
		intra_pred_angle_mode = uiDirMode - VER_IDX;
	}
	else {
		intra_pred_angle_mode = -(uiDirMode - HOR_IDX);
	}
	abs_ang_mode = abs(intra_pred_angle_mode);
	if (intra_pred_angle_mode < 0) {
		sign_ang = -1;
	}
	else {
		sign_ang = 1;
	}
	abs_ang = g_ang_table[abs_ang_mode];
	inv_angle = g_inv_ang_table[abs_ang_mode];
	intra_pred_angle = sign_ang * abs_ang;
	return intra_pred_angle;
}


void HevcPredicter::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index)
{
	distanMatri->distance_matri[j][i][0] = ref_main_index;
	distanMatri->distance_matri[j][i][1] = ref_side_index;

}


void HevcPredicter::initDstData() {
	hevc_dst = new int[max_dst_number]();
}

void HevcPredicter::deinitDstData() {
	if (hevc_dst) {
		delete[] hevc_dst;
		hevc_dst = NULL;
	}
}