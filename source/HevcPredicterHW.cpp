#include "HevcPredicterHW.h"


HevcPredicterHW::HevcPredicterHW() {
	max_dst_number = MAX_CU_SIZE_HEVC * MAX_CU_SIZE_HEVC;
	hevc_dst = NULL;
	dst_stride_True = MAX_CU_SIZE_HEVC;
	initLog2size();
}

HevcPredicterHW::~HevcPredicterHW() {

}

void HevcPredicterHW::predict() {
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
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			int iWidth = g_cu_size_hevc[j][0];
			int iHeight = g_cu_size_hevc[j][1];
			tu_width = iWidth;
			tu_height = iHeight;
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

void HevcPredicterHW::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	mode = uiDirMode;
	if (uiDirMode == PLANAR_IDX_HEVC) {
		predIntraPlanar(distanMatri, uiDirMode);
	}else if (uiDirMode == DC_IDX_HEVC) {
		predIntraDC(distanMatri, uiDirMode);
	}else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}

void HevcPredicterHW::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
	mode = uiDirMode;
	if (uiDirMode == PLANAR_IDX_HEVC) {
		predIntraPlanar(distanMatri, uiDirMode);
	}
	else if (uiDirMode == DC_IDX_HEVC) {
		predIntraDC(distanMatri, uiDirMode);
	}
	else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}


void HevcPredicterHW::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = tu_width;
	int iHeight = tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;
	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
	int max_index_number = CALCURATIO;
	// chrome when tu size is [4,y] or [x,4]
	if (tu_width < CALCURATIO) {
		x_offset = CALCURATIO / 2;
	}
	if (tu_height < CALCURATIO) {
		y_offset = CALCURATIO / 2;
	}
	int width_number = tu_width / x_offset;
	int heigh_number = tu_height / y_offset;
	int cal_matri_index = 0;
	int distance_index = 0;
	mode = uiDirMode;
	generateRefer();
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_HEVC);
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(i, j, x_offset, y_offset);
			firstGroupProcess();
			firstGroupCopySrc(lineRefer16, refMain);
			secondGroupProcess();
			secondGroupCopySrc(lineRefer, lineRefer16);
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
					//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					predPixel(m, n);
				}
			}
		}
	}
	deinitIndexMatri(max_index_number);
}

void HevcPredicterHW::predIntraPlanar(DistanceData* distanMatri, int uiDirMode) {

	int iWidth = tu_width;
	int iHeight = tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;
	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
	int max_index_number = CALCURATIO;
	// chrome when tu size is [4,y] or [x,4]
	if (tu_width < CALCURATIO) {
		x_offset = CALCURATIO / 2;
	}
	if (tu_height < CALCURATIO) {
		y_offset = CALCURATIO / 2;
	}
	int width_number = tu_width / x_offset;
	int heigh_number = tu_height / y_offset;
	int cal_matri_index = 0;
	int distance_index = 0;
	mode = uiDirMode;
	generateBiRefer();
	initTopLeft();
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_HEVC);
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(i, j, x_offset, y_offset);
			biFirstGroupProcess();
			initTopLeftRefer(i, j, x_offset, y_offset);
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
					//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					predPixelBi(m, n);
				}
			}
		}
	}
	deinitIndexMatri(max_index_number);

}

void HevcPredicterHW::firstGroupProcess() {
	groupType = GROUD_TYPE_MIN;
	extraType = EXTRACT_BOUND_8;
	group256(min_index);
	extraType = EXTRACT_BOUND_8;
	groupType = GROUD_TYPE_MAX;
	group256(max_index);

	first_bouds[0] = minBounds[0];
	if (minBounds[0] <= maxBounds[0]) {
		first_bouds[0] = minBounds[0];
	}
	first_bouds[1] = maxBounds[1];
	if (maxBounds[1] >= minBounds[1]) {
		first_bouds[1] = maxBounds[1];
	}
}


void HevcPredicterHW::firstGroupCopySrc(int* dst, int *src) {
	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		int srcIndex = convertSrcIndex2RefIndex(j);
		dst[refIndex] = src[srcIndex];
	}
}

void HevcPredicterHW::biFirstGroupCopySrc(int* dst, int *src) {

	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		dst[refIndex] = src[j];
	}
}
void HevcPredicterHW::biFirstGroupProcess() {

	//left
	groupType = GROUD_TYPE_MIN;
	extraType = EXTRACT_BOUND_4;
	group256(bi_left_min);
	first_bouds[0] = minBounds[0];
	first_bouds[1] = minBounds[1];
	biFirstGroupCopySrc(leftlineRefer8, lineRefer256);
	biLeftFirstBouds[0] = first_bouds[0];
	first_bouds[0] = bi_left_min - biLeftFirstBouds[0];
	first_bouds[1] = first_bouds[0] + LINEREFER4 - 1;
	biLeftSecondBouds[0] = first_bouds[0];
	biFirstGroupCopySrc(leftlineRefer4, leftlineRefer8);

	//above
	extraType = EXTRACT_BOUND_4;
	groupType = GROUD_TYPE_MIN;
	group256(bi_above_min);
	groupType = GROUD_TYPE_MAX;
	group256(bi_above_max);

	first_bouds[0] = minBounds[0];
	if (minBounds[0] <= maxBounds[0]) {
		first_bouds[0] = minBounds[0];
	}
	first_bouds[1] = maxBounds[1];
	if (maxBounds[1] >= minBounds[1]) {
		first_bouds[1] = maxBounds[1];
	}

	biFirstGroupCopySrc(AbovelineRefer8, lineRefer256);
	biAboveFirstBouds[0] = first_bouds[0];
	first_bouds[0] = bi_above_min - biAboveFirstBouds[0];
	first_bouds[1] = first_bouds[0] + LINEREFER4 - 1;
	biAboveSecondBouds[0] = first_bouds[0];
	biFirstGroupCopySrc(AbovelineRefer4, AbovelineRefer8);
}

void HevcPredicterHW::secondGroupProcess() {
	second_bouds[0] = min_index;
	second_bouds[1] = second_bouds[0] + LINE_REFER_SIZE - 1;
}
void HevcPredicterHW::secondGroupCopySrc(int* dst, int *src) {

	for (int j = second_bouds[0]; j <= second_bouds[1]; j++) {
		int refIndex = j - second_bouds[0];
		int srcIndex = j - first_bouds[0];
		dst[refIndex] = src[srcIndex];
	}
}

int HevcPredicterHW::convertSrcIndex2RefIndex(int index) {
	int indexOut = index - CENTRE_OFFSET;
	return indexOut;
}

void HevcPredicterHW::predPixelIndex(int j, int i) {
	int delta_pos = 0;
	int delta_int = 0;
	int delta_fract = 0;
	if (intra_pred_angle == 0) {
		if (is_mod_ver) {
			saveMaxMinIndex(i + 1, i + 1);
		}
		else {
			saveMaxMinIndex(j + 1, j + 1);
		}
	}
	else {
		if (is_mod_ver) {
			delta_pos = (intra_pred_angle)*(j + 1);
			delta_int = delta_pos >> 5;
			delta_fract = delta_pos & (32 - 1);
			if (delta_fract) {
				int ref_main_index = i + delta_int + 1;
				int ref_side_index = ref_main_index + 1;
				saveMaxMinIndex(ref_main_index, ref_side_index);
			}
			else {
				saveMaxMinIndex(i + delta_int + 1, i + delta_int + 1);
			}
		}
		else {
			delta_pos = (intra_pred_angle)*(i + 1);
			delta_int = delta_pos >> 5;
			delta_fract = delta_pos & (32 - 1);
			if (delta_fract) {
				int ref_main_index = delta_int + j + 1;
				int ref_side_index = ref_main_index + 1;
				saveMaxMinIndex(ref_main_index, ref_side_index);
			}
			else {
				// Just copy the integer samples
				saveMaxMinIndex(delta_int + j + 1, delta_int + j + 1);
			}
		}
	}

}

void HevcPredicterHW::predPixel(int j, int i) {
	int delta_pos = 0;
	int delta_int = 0;
	int delta_fract = 0;
	const int dstStride = is_mod_ver ? dst_stride_True : MAX_CU_SIZE_HEVC;
	int *pDst = hevc_dst;
	if (intra_pred_angle == 0) {
		if (is_mod_ver) {
			int linerefIndex = convertSrcIndex2LineRefIndex(i + 1);
			pDst[j*dstStride + i] = lineRefer[linerefIndex];
		}
		else {
			int linerefIndex = convertSrcIndex2LineRefIndex(j + 1);
			pDst[j*dstStride + i] = lineRefer[linerefIndex];
		}
	}
	else {
		int *pDsty = pDst;
		if (is_mod_ver) {
			delta_pos = (intra_pred_angle)*(j + 1);
			delta_int = delta_pos >> 5;
			delta_fract = delta_pos & (32 - 1);
			if (delta_fract) {
				int ref_main_index = i + delta_int + 1;
				int ref_side_index = ref_main_index + 1;
				int linerefIndexMain = convertSrcIndex2LineRefIndex(ref_main_index);
				int linerefIndexSide = convertSrcIndex2LineRefIndex(ref_side_index);
				pDsty[i + dst_stride_True * j] = ((32 - delta_fract)*lineRefer[linerefIndexMain] + delta_fract * lineRefer[linerefIndexSide] + 16) >> 5;
			}
			else {
				int ref_main_index = i + delta_int + 1;
				int linerefIndexMain = convertSrcIndex2LineRefIndex(ref_main_index);
				pDsty[i + dst_stride_True * j] = lineRefer[linerefIndexMain];
			}
		}
		else {
			delta_pos = (intra_pred_angle)*(i + 1);
			delta_int = delta_pos >> 5;
			delta_fract = delta_pos & (32 - 1);
			if (delta_fract) {
				int ref_main_index = delta_int + j + 1;
				int ref_side_index = ref_main_index + 1;
				int linerefIndexMain = convertSrcIndex2LineRefIndex(ref_main_index);
				int linerefIndexSide = convertSrcIndex2LineRefIndex(ref_side_index);
				pDsty[j*dst_stride_True + i] = ((32 - delta_fract)*lineRefer[linerefIndexMain] + delta_fract * lineRefer[linerefIndexSide] + 16) >> 5;

			}
			else {
				// Just copy the integer samples
				int ref_main_index = delta_int + j + 1;
				int linerefIndexMain = convertSrcIndex2LineRefIndex(ref_main_index);
				pDsty[j*dst_stride_True + i] = lineRefer[linerefIndexMain];
			}
		}
	}

}

int HevcPredicterHW::convertSrcIndex2LineRefIndex(int index) {
	int secondBound = second_bouds[0];
	int indexOut = index + CENTRE_OFFSET - secondBound;
	return indexOut;
}
void HevcPredicterHW::generateRefer() {
	int iWidth = tu_width;
	int iHeight = tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;
	//int *refMain;
	//int *refSide;
	//int refAbove[2 * MAX_CU_SIZE_HEVC + 1];
	//int refLeft[2 * MAX_CU_SIZE_HEVC + 1];
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

}


void HevcPredicterHW::generateBiRefer() {
	int width = tu_width;
	int height = tu_height;
	int bitDepth = 8;
	int srcStride = 2 * width + 1;
	//int *refMain;
	//int *refSide;
	//int refAbove[2 * MAX_CU_SIZE_HEVC + 1];
	//int refLeft[2 * MAX_CU_SIZE_HEVC + 1];
	int shift1Dver = g_log2size[height] + 2;
	int *pSrc = NULL;
	int leftColumn[MAX_CU_SIZE_HEVC + 1], topRow[MAX_CU_SIZE_HEVC + 1];

	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}
	// Get left and above reference column and row
	for (int k = 0; k < width ; k++)
	{
		topRow[k] = pSrc[k - srcStride];
		lineRefer256[k + 1 + CENTRE_OFFSET_HEVC] = topRow[k];
	}
	topRight = pSrc[width- srcStride];
	for (int k = 0; k < height; k++)
	{
		leftColumn[k] = pSrc[k*srcStride - 1];
		lineRefer256[-k -1 + CENTRE_OFFSET_HEVC] = leftColumn[k];
	}
    bottomLeft = pSrc[height*srcStride - 1];

}
void HevcPredicterHW::get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset) {

	int x = i * x_offset;
	int x4 = x_offset * (i + 1) - 1;
	int y = j * y_offset;
	int y4 = y_offset * (j + 1) - 1;
	if (mode == PLANAR_IDX_HEVC) {
		save_index = 0;
		predPixelIndexBi(y, x);
		save_index++;
		predPixelIndexBi(y4, x);
		save_index++;
		predPixelIndexBi(y, x4);
		save_index++;
		predPixelIndexBi(y4, x4);
		bi_left_min = max_min_indexs[1][1];
		bi_above_min = max_min_indexs[0][0];
		bi_above_max = max_min_indexs[2][0];
	}else {
		save_index = 0;
		predPixelIndex(y, x);
		save_index++;
		predPixelIndex(y4, x);
		save_index++;
		predPixelIndex(y, x4);
		save_index++;
		predPixelIndex(y4, x4);
		int max_index_number = save_index + 1;
		max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_HEVC, max_index_number);
		min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_HEVC, max_index_number);
	}
}


void HevcPredicterHW::predPixelBi(int j, int i) {
	int width = tu_width;
	int height = tu_height;
	int srcStride = 2 * tu_width + 1;
	int dstStride = dst_stride_True;
	//int leftColumn[MAX_CU_SIZE_HEVC + 1], topRow[MAX_CU_SIZE_HEVC + 1], bottomRow[MAX_CU_SIZE_HEVC], rightColumn[MAX_CU_SIZE_HEVC];
	int shift1Dhor = g_log2size[width] + 2;
	int shift1Dver = g_log2size[height] + 2;
	int *pSrc = NULL;
	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}

	// Prepare intermediate variables used in interpolation

	//printf("topRow[i]:%d ", topRow[i]);
	//printf("leftColumn[j]:%d ", leftColumn[j]);
	//printf("\n");

	const int topRowShift = 0;

	// Generate prediction signal

	int horPred = leftColumn[j] + width;
	horPred = horPred +  rightColumn[j]*(i+1);
	topRow[i] = topRow[i] +  bottomRow[i];
	int vertPred = ((topRow[i] + topRowShift) >> topRowShift);
	//printf("horPred:%d, vertPred:%d, shift1Dhor:%d \n", horPred, vertPred, shift1Dhor);
	hevc_dst[j*dstStride + i] = (horPred + vertPred) >> (shift1Dhor + 1);
		


}

void HevcPredicterHW::predIntraDC(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;

	int *pSrc = NULL;
	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}
	int width = iWidth;
	int height = iHeight;

	const bool modeDC = uiDirMode == DC_IDX_HEVC;

	// Do the DC prediction
	if (modeDC)
	{
		const int  dcval = predIntraGetPredValDC(pSrc, srcStride, width, height);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++) // width is always a multiple of 4.
			{
				hevc_dst[x + y * dst_stride_True] = dcval;
			}
		}

	}
}
int HevcPredicterHW::predIntraGetPredValDC(const int* pSrc, int iSrcStride, int iWidth, int iHeight)
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

void HevcPredicterHW::predPixelIndexBi(int j, int i) {
	int iX, iY;
	int iWidth = tu_width;
	int iHeight = tu_height;

	iX = i;
	iY = j;
	// refer to x  need plus 1 + 128 transfer to [0 256]
	iX = iX + 1;
	// refer to y  need minus 1 + 128 transfer to [0 256]
	iY = -iY - 1;
	max_min_indexs[save_index][0] = iX + CENTRE_OFFSET;
	max_min_indexs[save_index][1] = iY + CENTRE_OFFSET;


}

void HevcPredicterHW::initTopLeftRefer(int i, int j, int x_offset, int y_offset) {
	int shift1Dhor = g_log2size[tu_width] + 2;
	int shift1Dver = g_log2size[tu_height] + 2;
	for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
		int aboveIndex = biConvertSrcAboveIndex2LineRefIndex(n + 1);
		if (topRow[n] == -1) {
			topRow[n] = AbovelineRefer4[aboveIndex];
			bottomRow[n] = bottomLeft - topRow[n];
			topRow[n] <<= shift1Dver;
		}
	}
	for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
		int leftIndex = biConvertSrcLeftIndex2LineRefIndex(-1 - m);
		if (leftColumn[m] == -1) {
			leftColumn[m] = leftlineRefer4[leftIndex];
			rightColumn[m] = topRight - leftColumn[m];
			leftColumn[m] <<= shift1Dhor;
		}
	}


}

void HevcPredicterHW::initTopLeft() {
	for (int i = 0; i < MAX_CU_SIZE_HEVC; i++) {
		bottomRow[i] = -1;
		rightColumn[i] = -1;
	}
	for (int i = 0; i <= MAX_CU_SIZE_HEVC; i++) {
		topRow[i] = -1;
		leftColumn[i] = -1;
	}

}
int HevcPredicterHW::biConvertSrcLeftIndex2LineRefIndex(int index) {
	int firstBound = biLeftFirstBouds[0];
	int secondBound = biLeftSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut;
}
int HevcPredicterHW::biConvertSrcAboveIndex2LineRefIndex(int index) {
	int firstBound = biAboveFirstBouds[0];
	int secondBound = biAboveSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut;
}
int HevcPredicterHW::computeIntraPredAngle(int uiDirMode) {
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


void HevcPredicterHW::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index)
{
	distanMatri->distance_matri[j][i][0] = ref_main_index;
	distanMatri->distance_matri[j][i][1] = ref_side_index;

}

void HevcPredicterHW::saveMaxMinIndex(int x, int x1)
{
	max_min_indexs[save_index][0] = x + CENTRE_OFFSET_HEVC;
	max_min_indexs[save_index][1] = x1 + CENTRE_OFFSET_HEVC;
}

void HevcPredicterHW::initDstData() {
	hevc_dst = new int[max_dst_number]();
}


void HevcPredicterHW::deinitDstData() {
	if (hevc_dst) {
		delete [] hevc_dst;
		hevc_dst = NULL;
	}
}