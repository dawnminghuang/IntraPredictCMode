#include "HevcPredicter.h"


HevcPredicter::HevcPredicter() {
	max_dst_number = MAX_CU_SIZE_HEVC * MAX_CU_SIZE_HEVC;
	hevc_dst = NULL;
	dst_stride_True = MAX_CU_SIZE_HEVC;
	initLog2size();
}

HevcPredicter::~HevcPredicter() {

}

void HevcPredicter::predict() {
	int mode_number = NUM_INTRA_PMODE_HEVC;
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
		int iWidth;
		int iHeight;
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			for (int k = 0; k < NUM_COLOR_SPACE_SIZE; k++) {
				if (k == COLOR_SPACE_LUMA) {
					iWidth = g_cu_size_hevc[j][0];
					iHeight = g_cu_size_hevc[j][1];
					tu_width = iWidth;
					tu_height = iHeight;
					initDstData();
					DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
					predIntraLumaAdi(distanMatri, uiDirMode);
					outPutWriter->writeModeInfoToFile(distanMatri);
					distanceCalculator->calcuDistance(distanMatri);
					outPutWriter->writeDstDataToFile(hevc_dst, iWidth, iHeight, dst_stride_True);
					deinitDstData();
					delete distanMatri;
				}
				else {
					iWidth = g_cu_size_hevc[j][0] / 2;
					iHeight = g_cu_size_hevc[j][1] / 2;
					tu_width = iWidth;
					tu_height = iHeight;
					initDstData();
					DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
					predIntraChromaAdi(distanMatri, uiDirMode);
					outPutWriter->writeDstDataToFile(hevc_dst, iWidth, iHeight, dst_stride_True);
					deinitDstData();
					delete distanMatri;

				}
			}
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
	int width = iWidth;
	int height = iHeight;

	const bool modeDC = uiDirMode == DC_IDX_HEVC;

	// Do the DC prediction
	if (modeDC){
		const int  dcval = predIntraGetPredValDC(pSrc, srcStride, width, height);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++) // width is always a multiple of 4.
			{
				hevc_dst[x + y * dst_stride_True] = dcval;
			}
		}
	}
	else {
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
			if (is_mod_ver) {
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
							int ref_main_index = i + delta_int + 1;
							int ref_side_index = ref_main_index + 1;
							pDsty[i + dst_stride_True * j] = ((32 - delta_fract)*refMain[ref_main_index] + delta_fract * refMain[ref_side_index] + 16) >> 5;

							saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
						}
					}
					else {
						// Just copy the integer samples
						for (int i = 0; i < iWidth; i++) {
							pDsty[i + dst_stride_True * j] = refMain[i + delta_int + 1];
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
							pDsty[j*dst_stride_True + i] = (((32 - delta_fract)*refMain[ref_main_index] + delta_fract * refMain[ref_side_index]) + 16) >> 5;
							saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
						}
					}
					else {
						// Just copy the integer samples
						for (int j = 0; j < iHeight; j++) {
							pDsty[j*dst_stride_True + i] = refMain[delta_int + j + 1];
							saveDistanceMatri(distanMatri, i, j, delta_int + j + 1, delta_int + j + 1);
						}
					}
				}
			}
		}
	}
}

void HevcPredicter::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	if (uiDirMode == PLANAR_IDX_HEVC) {
		predIntraPlanar(distanMatri, uiDirMode);
	}
	else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}


void HevcPredicter::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
	if (uiDirMode == PLANAR_IDX_HEVC) {
		predIntraPlanar(distanMatri, uiDirMode);
	}
	else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}
void HevcPredicter::predIntraPlanar(DistanceData* distanMatri, int uiDirMode) {

	int width = tu_width;
	int height = tu_height;
	int srcStride = 2 * tu_width + 1;
	int dstStride = dst_stride_True;
	int leftColumn[MAX_CU_SIZE_HEVC + 1], topRow[MAX_CU_SIZE_HEVC + 1], bottomRow[MAX_CU_SIZE_HEVC], rightColumn[MAX_CU_SIZE_HEVC];
	int shift1Dhor = g_log2size[width] + 2;
	int shift1Dver = g_log2size[height] + 2;
	int *pSrc = NULL;
	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}
	// Get left and above reference column and row
	for (int k = 0; k < width + 1; k++)
	{
		topRow[k] = pSrc[k - srcStride];
	}

	for (int k = 0; k < height + 1; k++)
	{
		leftColumn[k] = pSrc[k*srcStride - 1];
	}

	// Prepare intermediate variables used in interpolation
	int bottomLeft = leftColumn[height];
	int topRight = topRow[width];

	for (int k = 0; k < width; k++)
	{
		bottomRow[k] = bottomLeft - topRow[k];
		topRow[k] <<= shift1Dver;
	}

	for (int k = 0; k < height; k++)
	{
		rightColumn[k] = topRight - leftColumn[k];
		leftColumn[k] <<= shift1Dhor;
	}

	const int topRowShift = 0;

	// Generate prediction signal
	for (int y = 0; y < height; y++)
	{
		int horPred = leftColumn[y] + width;
		for (int x = 0; x < width; x++)
		{
			horPred += rightColumn[y];
			topRow[x] += bottomRow[x];

			int vertPred = ((topRow[x] + topRowShift) >> topRowShift);
			hevc_dst[y*dstStride + x] = (horPred + vertPred) >> (shift1Dhor + 1);
		}
	}

}

int HevcPredicter::predIntraGetPredValDC(const int* pSrc, int iSrcStride, int iWidth, int iHeight)
{
	int iInd, iSum = 0;
	int pDcVal;

	for (iInd = 0; iInd < iWidth; iInd++)
	{
		iSum += pSrc[iInd - iSrcStride];
	}
	for (iInd = 0; iInd < iHeight; iInd++)
	{
		iSum += pSrc[iInd*iSrcStride - 1];
	}

	pDcVal = (iSum + iWidth) / (iWidth + iHeight);

	return pDcVal;
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
		delete [] hevc_dst;
		hevc_dst = NULL;
	}
}