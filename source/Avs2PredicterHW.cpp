#include "AVS2PredicterHW.h"


AVS2PredicterHW::AVS2PredicterHW() {
	avs_dst = NULL;
	tu_width = 0;
	tu_height = 0;
	pSrc = NULL;
	max_min_indexs = NULL;
	save_index = 0;
	uiDirMode = 0;
	max_index = 0;
	min_index = 0;
	second_max = 0;
	second_min = 0;
	groupType = GROUD_TYPE_MIN;
	scanType = SCAN_TYPE_ROW;
	sample_bit_depth = 8;
	bLeftAvail = 1;
	bAboveAvail = 1;
	cu_size_log = CU_SIZ_LOG_AVS2;
	initLog2size();
}

AVS2PredicterHW::~AVS2PredicterHW() {

}


void AVS2PredicterHW::predict() {
	int mode_number = NUM_INTRA_PMODE_AVS;
	int max_cu_size = 64;
	generateOutPath(AVS2_PATH, calc_mode);
	for (int i = 0; i < NUM_MODE_INTRA_AVS; i++) {
		uiDirMode = g_prdict_mode_avs[i];
		outPutWriter->initDstDataFp(AVS2_DATA_PATH_CMODE, uiDirMode);
		for (int j = 0; j < NUM_CU_PMODE_AVS; j++) {
			tu_width = g_cu_size_avs[j][0];
			tu_height = g_cu_size_avs[j][1];
			for (int k = 0; k < NUM_COLOR_SPACE_SIZE; k++) {
				if (k == COLOR_SPACE_LUMA) {
					tu_width = g_cu_size_avs[j][0];
					tu_height = g_cu_size_avs[j][1];
					initDstData();
					DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
					predIntraLumaAdi(distanMatri, uiDirMode);
					outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
					deinitDstData();
					delete distanMatri;
				} else {
					tu_width = g_cu_size_avs[j][0]/2;
					tu_height = g_cu_size_avs[j][1]/2;
					initDstData();
					DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
					predIntraChromaAdi(distanMatri, uiDirMode);
					outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
					deinitDstData();
					delete distanMatri;
				}

			}
		}
	}
}


void AVS2PredicterHW::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	int bAbove = 1;
	int bLeft = 1;
	iHeight = tu_height;
	iWidth = tu_width;

	switch (uiDirMode) {
	//case VERT_PRED_AVS2:   // Vertical
		//xPredIntraVertAdi(distanMatri, uiDirMode);
		//break;
	//case HOR_PRED_AVS2:    // Horizontal
		//xPredIntraHorAdi(distanMatri, uiDirMode);
		//break;
	case DC_PRED_AVS2:     // DC
		xPredIntraDCAdi(distanMatri, uiDirMode);
		break;
	case PLANE_PRED_AVS2:  // Plane
		xPredIntraPlaneAdi(distanMatri, uiDirMode);
		break;
	case BI_PRED_AVS2:     // bi
		PredIntraBiAdi(distanMatri, uiDirMode);
		break;
	default:
		PredIntraAngAdi(distanMatri, uiDirMode);
		break;
	}
}
void AVS2PredicterHW::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode)
{
	int bAbove = 1;
	int bLeft = 1;
	iHeight = tu_height;
	iWidth = tu_width;
	switch (uiDirMode) {
	//case VERT_PRED_AVS2:   // Vertical
		//xPredIntraVertAdi(distanMatri, uiDirMode);
		//break;
	//case HOR_PRED_AVS2:    // Horizontal
		//xPredIntraHorAdi(distanMatri, uiDirMode);
		//break;
	case DC_PRED_AVS2:     // DC
		xPredIntraDCAdi(distanMatri, uiDirMode);
		break;
	case PLANE_PRED_AVS2:  // Plane
		xPredIntraPlaneAdi(distanMatri, uiDirMode);
		break;
	case BI_PRED_AVS2:     // bi
		PredIntraBiAdi(distanMatri, uiDirMode);
		break;
	default:
		PredIntraAngAdi(distanMatri, uiDirMode);
		break;
	}
}

void AVS2PredicterHW::xPredIntraDCAdi(DistanceData* distanMatri, int uiDirMode)
{
	int   x, y;
	int   iDCValue = 0;
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	int  *rpSrc = pSrc - 1;

	if (bLeftAvail) {
		for (y = 0; y < iHeight; y++) {
			iDCValue += rpSrc[-y];
		}

		rpSrc = pSrc + 1;
		if (bAboveAvail) {
			for (x = 0; x < iWidth; x++) {
				iDCValue += rpSrc[x];
			}

			iDCValue += ((iWidth + iHeight) >> 1);
			iDCValue = (iDCValue * (512 / (iWidth + iHeight))) >> 9;

		}
		else {
			iDCValue += iHeight / 2;
			iDCValue /= iHeight;
		}
	}
	else {
		rpSrc = pSrc + 1;
		if (bAboveAvail) {
			for (x = 0; x < iWidth; x++) {
				iDCValue += rpSrc[x];
			}

			iDCValue += iWidth / 2;
			iDCValue /= iWidth;
		}
		else {
			iDCValue = 1 << (sample_bit_depth - 1);
		}
	}

	for (y = 0; y < iHeight; y++) {
		for (x = 0; x < iWidth; x++) {
			avs_dst[y][x] = iDCValue;
		}
	}
}

void AVS2PredicterHW::xPredIntraPlaneAdi(DistanceData* distanMatri, int uiDirMode)
{
	int iH = 0;
	int iV = 0;
	int iA, iB, iC;
	int x, y;
	int iW2 = iWidth >> 1;
	int iH2 = iHeight >> 1;
	int ib_mult[5] = { 13, 17, 5, 11, 23 };
	int ib_shift[5] = { 7, 10, 11, 15, 19 };

	int im_h = ib_mult[g_log2size[iWidth] - 2];
	int is_h = ib_shift[g_log2size[iWidth] - 2];
	int im_v = ib_mult[g_log2size[iHeight] - 2];
	int is_v = ib_shift[g_log2size[iHeight] - 2];
	int iTmp, iTmp2;

	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	int  *rpSrc = pSrc;

	rpSrc = pSrc + 1;
	rpSrc += (iW2 - 1);
	for (x = 1; x < iW2 + 1; x++) {
		iH += x * (rpSrc[x] - rpSrc[-x]);
	}

	rpSrc = pSrc - 1;
	rpSrc -= (iH2 - 1);

	for (y = 1; y < iH2 + 1; y++) {
		iV += y * (rpSrc[-y] - rpSrc[y]);
	}

	rpSrc = pSrc;
	iA = (rpSrc[-1 - (iHeight - 1)] + rpSrc[1 + iWidth - 1]) << 4;
	iB = ((iH << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	iC = ((iV << 5) * im_v + (1 << (is_v - 1))) >> is_v;

	iTmp = iA - (iH2 - 1) * iC - (iW2 - 1) * iB + 16;
	for (y = 0; y < iHeight; y++) {
		iTmp2 = iTmp;
		for (x = 0; x < iWidth; x++) {
			//img->mprr[PLANE_PRED][y][x] = Clip( iTmp2 >> 5 );
			avs_dst[y][x] = Clip3(0, (1 << sample_bit_depth) - 1, iTmp2 >> 5);
			iTmp2 += iB;
		}
		iTmp += iC;
	}
}


void AVS2PredicterHW::PredIntraBiAdi(DistanceData* distanMatri, int mode)
{

	int iWidth2 = tu_width << 1;
	int iHeight2 = tu_height << 1;
	uiDirMode = mode;
	scanType = g_prdict_mode_row[uiDirMode];

	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
	// chrome when tu size is [4,y] or [x,4]
	if (tu_width < CALCURATIO) {
		x_offset = CALCURATIO / 2;
	}
	if (tu_height < CALCURATIO) {
		y_offset = CALCURATIO / 2;
	}
	int max_index_number = CALCURATIO;
	int width_number = tu_width / x_offset;
	int heigh_number = tu_height / y_offset;
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_AVS);
	int cal_matri_index = 0;
	int distance_index = 0;
	initReferAB();
	initTopLeft();
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(i, j, x_offset, y_offset);
			biFirstGroupProcess();
			initTopLeftRefer(i, j, x_offset, y_offset);
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					predPixelBi(m, n);
				}
			}
		}
	}
	deinitIndexMatri(max_index_number);
}

void AVS2PredicterHW::PredIntraAngAdi(DistanceData* distanMatri, int mode)
{
	int  uixyflag = 0; // 0 for x axis 1 for y axis
	int iDxy;
	int iWidth2 = tu_width << 1;
	int iHeight2 = tu_height << 1;

	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src; // + ((1 << CU_SIZ_LOG_AVS2) * 2);
	}
	uiDirMode = mode;
	scanType = g_prdict_mode_row[uiDirMode];

	uixyflag = g_aucXYflg[uiDirMode];
	iDxy = g_aucSign[uiDirMode];


	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
	// chrome when tu size is [4,y] or [x,4]
	if (tu_width < CALCURATIO) {
		x_offset = CALCURATIO / 2;
	}
	if (tu_height < CALCURATIO) {
		y_offset = CALCURATIO / 2;
	}
	int max_index_number = CALCURATIO;
	int width_number = tu_width /x_offset;
	int heigh_number = tu_height /y_offset;

	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_AVS);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(i,j, x_offset, y_offset);
			firstGroupProcess();
			firstGroupCopySrc(lineRefer64, pSrc);
			firstGroupCopySrc32(lineRefer32, lineRefer64);
			if (scanType == SCAN_TYPE_ROW) {
				scanByRow(i,j, x_offset, y_offset);
			}else {
				scanByCol(i, j, x_offset, y_offset);
			}
		}
	}
	deinitIndexMatri(max_index_number);

}

void AVS2PredicterHW::scanByRow(int i, int j, int x_offset, int y_offset) {
	for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
		getLineMaxMinIndex(i, j, m, x_offset, y_offset);
		secondGroupProcess();
		secondGroupCopySrc(lineRefer, lineRefer32);
		//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
			//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
		for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
			if ((uiDirMode == VERT_PRED_AVS2) || (uiDirMode == HOR_PRED_AVS2)) {
				predPixelVerHor(m, n);
			}else {
				predPixel(m, n);
			}
		}
	}
}

void AVS2PredicterHW::scanByCol(int i, int j, int x_offset, int y_offset) {
	for (int m = i* x_offset; m < x_offset*(i + 1); m++) {
		getLineMaxMinIndex(i,j,m,x_offset, y_offset);
		secondGroupProcess();
		secondGroupCopySrc(lineRefer, lineRefer32);
		for (int n = j * x_offset; n < y_offset*(j + 1); n++) {
			predPixel(n, m);
		}
	}


}
void AVS2PredicterHW::getLineMaxMinIndex(int i, int j, int m,int x_offset, int y_offset) {
	if (scanType == SCAN_TYPE_ROW) {
		save_index = 0;
		for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
			if ((uiDirMode == VERT_PRED_AVS2) || (uiDirMode == HOR_PRED_AVS2)) {
				predPixelIndexVerHor(m, n);
			}else {
				predPixelIndex(m, n);
			}
			save_index++;
		}
	}else {
		save_index = 0;
		for (int n = j * y_offset; n < y_offset*(j + 1); n++) {
			if ((uiDirMode == VERT_PRED_AVS2) || (uiDirMode == HOR_PRED_AVS2)) {
				predPixelIndexVerHor(n, m);
			}else {
				predPixelIndex(n, m);
			}
			save_index++;
		}
	}
	int offset = CALCURATIO;
	second_min = max_min_indexs[0][0];
	int ii, jj;
	for (ii = 0; ii <= offset; ii += (offset - 1)) {
		for (jj = 0; jj < NUM_DISTANCE_SIZE_AVS; jj++) {
			if (max_min_indexs[ii][jj] < second_min) {
				second_min = max_min_indexs[ii][jj];
			}
		}
	}

}
void AVS2PredicterHW::get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset) {

	int x = i * x_offset;
	int x4 = x_offset * (i + 1)-1;
	int y = j * y_offset;
	int y4 = y_offset * (j + 1)-1;
	save_index = 0;
	if ((uiDirMode == VERT_PRED_AVS2) || (uiDirMode == HOR_PRED_AVS2)) {
		predPixelIndexVerHor(y, x);
		save_index++;
		predPixelIndexVerHor(y4, x);
		save_index++;
		predPixelIndexVerHor(y, x4);
		save_index++;
		predPixelIndexVerHor(y4, x4);
		int max_index_number = save_index + 1;
		max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
		min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
	}else if (uiDirMode == BI_PRED_AVS2) {
		predPixelIndexBi(y, x);
		save_index++;
		predPixelIndexBi(y4, x);
		save_index++;
		predPixelIndexBi(y, x4);
		save_index++;
		predPixelIndexBi(y4, x4);
		bi_left_min = max_min_indexs[1][3];
		bi_above_min = max_min_indexs[0][0];
		bi_above_max = max_min_indexs[2][0];
	}else {
		predPixelIndex(y, x);
		save_index++;
		predPixelIndex(y4, x);
		save_index++;
		predPixelIndex(y, x4);
		save_index++;
		predPixelIndex(y4, x4);
		int max_index_number = save_index + 1;
		max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
		min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
	}

}

void AVS2PredicterHW::biFirstGroupProcess() {

	//left
	groupType = GROUD_TYPE_MIN;
	extraType = EXTRACT_BOUND_4;
	group256(bi_left_min);
	first_bouds[0] = minBounds[0];
	first_bouds[1] = minBounds[1];
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src;
	}
	firstGroupCopySrc(leftlineRefer8, pSrc);
	biLeftFirstBouds[0] = first_bouds[0];
	first_bouds[0] = bi_left_min - biLeftFirstBouds[0];
	first_bouds[1] = first_bouds[0] + LINEREFER4 -1;
	biLeftSecondBouds[0] = first_bouds[0];
	firstGroupCopySrc(leftlineRefer4, leftlineRefer8);

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

	firstGroupCopySrc(AbovelineRefer8, pSrc);
	biAboveFirstBouds[0] = first_bouds[0];
	first_bouds[0] = bi_above_min - biAboveFirstBouds[0];
	first_bouds[1] = first_bouds[0] + LINEREFER4 - 1;
	biAboveSecondBouds[0] = first_bouds[0];
	firstGroupCopySrc(AbovelineRefer4, AbovelineRefer8);
}

void AVS2PredicterHW::firstGroupProcess() {
	groupType = GROUD_TYPE_MIN;
    extraType = EXTRACT_BOUND_32;
	group256(min_index);
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

void AVS2PredicterHW::secondGroupProcess() {
	avs2Group32(second_min);
	second_bouds[0] = second_min - first_bouds[0];
	int boud = second_bouds[0] + LINE_REFER_SIZE - 1;
	second_bouds[1] = ( boud <= second_bouds[1]) ? boud : second_bouds[1];
}

void AVS2PredicterHW::copyIndex(int** dst, int **src, int dstIndex, int srcIndex) {
	dst[dstIndex][0] = src[srcIndex][0];
	dst[dstIndex][1] = src[srcIndex][1];
	dst[dstIndex][2] = src[srcIndex][2];
	dst[dstIndex][3] = src[srcIndex][3];
}

void AVS2PredicterHW::firstGroupCopySrc32(int* dst, int *src) {

	int first_bouds64 = first_bouds[0];
	first_bouds[0] = min_index;
	int boud = first_bouds[0] + SECOND_GROUP_STEP;
	first_bouds[1] = (boud <= first_bouds[1]) ? boud: first_bouds[1];
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		int ref64Index = j - first_bouds64;
		dst[refIndex] = src[ref64Index];
	}
}

void AVS2PredicterHW::firstGroupCopySrc(int* dst, int *src) {

	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		dst[refIndex] = src[j];
	}
}
void AVS2PredicterHW::secondGroupCopySrc(int* dst, int *src) {
	for (int j = second_bouds[0]; j <= second_bouds[1]; j++) {
		//int srcIndex = convertSrcIndex2RefIndex(second_bouds[0] + j);
		int refIndex = j - second_bouds[0];
		dst[refIndex] = src[j];
	}
}
int AVS2PredicterHW::convertSrcIndex2RefIndex(int index) {
	int indexOut = index - CENTRE_OFFSET;
	return indexOut;
}


void AVS2PredicterHW::avs2Group32(int index) {
	int groupStep = SECOND_GROUP_STEP / 2;
	int boud0 = first_bouds[0];
	int boud8 = boud0 + LINE_REFER_SIZE ;
	int boud16 = boud0 + LINE_REFER_SIZE * 2;//  no need minus 1, start from 0, 8, 16
	if (index >= boud16) {
		second_bouds[0] = boud16;
		second_bouds[1] = boud16 + groupStep;
	}else if (index >= boud8) {
		second_bouds[0] = boud8;
		second_bouds[1] = boud8 + groupStep;
	}else if (index >= boud0) {
		second_bouds[0] = boud0;
		second_bouds[1] = boud0 + groupStep;
	}else {
		printf("something happend group32:%d \n", index);
	}

}
void AVS2PredicterHW::predPixelIndex(int j, int i) {
	int  iDx, iDy, iTempDx, iTempDy, iXx, iXy, iYx, iYy;
	int  uixyflag = 0; // 0 for x axis 1 for y axis
	int offset, offsetx, offsety;
	int iX, iY, iXn, iYn, iXnN1, iYnN1, iXnP2, iYnP2;
	int iDxy;
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iWidth2 = iWidth << 1;
	int iHeight2 = iHeight << 1;
	iDx = g_aucDirDx[uiDirMode];
	iDy = g_aucDirDy[uiDirMode];
	uixyflag = g_aucXYflg[uiDirMode];
	iDxy = g_aucSign[uiDirMode];

	if (iDxy < 0) {   // select context pixel based on uixyflag
		if (uixyflag == 0) {
			// case x-line
			iTempDy = j - (-1);
			iTempDx = getContextPixel(uiDirMode, 0, iTempDy, &offset);
			iX = i + iTempDx;
			iY = -1;
		}
		else {
			// case y-line
			iTempDx = i - (-1);
			iTempDy = getContextPixel(uiDirMode, 1, iTempDx, &offset);
			iX = -1;
			iY = j + iTempDy;
		}
	}
	else { // select context pixel based on the distance
		iTempDy = j - (-1);
		iTempDx = getContextPixel(uiDirMode, 0, iTempDy, &offsetx);
		iTempDx = -iTempDx;
		iXx = i + iTempDx;
		iYx = -1;

		iTempDx = i - (-1);
		iTempDy = getContextPixel(uiDirMode, 1, iTempDx, &offsety);
		iTempDy = -iTempDy;
		iXy = -1;
		iYy = j + iTempDy;

		if (iYy <= -1) {
			iX = iXx;
			iY = iYx;
			offset = offsetx;
		}
		else {
			iX = iXy;
			iY = iYy;
			offset = offsety;
		}
	}

	if (iY == -1) {
		if (iDxy < 0) {
			iXnN1 = iX - 1;
			iXn = iX + 1;
			iXnP2 = iX + 2;
		}
		else {
			iXnN1 = iX + 1;
			iXn = iX - 1;
			iXnP2 = iX - 2;
		}

		iXnN1 = min(iWidth2 - 1, iXnN1);
		iX = min(iWidth2 - 1, iX);
		iXn = min(iWidth2 - 1, iXn);
		iXnP2 = min(iWidth2 - 1, iXnP2);
		saveMaxMinIndex(i, j, iXnN1, iX, iXn, iXnP2, 0);

	}
	else if (iX == -1) {
		if (iDxy < 0) {
			iYnN1 = iY - 1;
			iYn = iY + 1;
			iYnP2 = iY + 2;
		}
		else {
			iYnN1 = iY + 1;
			iYn = iY - 1;
			iYnP2 = iY - 2;
		}

		iYnN1 = min(iHeight2 - 1, iYnN1);
		iY = min(iHeight2 - 1, iY);
		iYn = min(iHeight2 - 1, iYn);
		iYnP2 = min(iHeight2 - 1, iYnP2);
		saveMaxMinIndex(i, j, iYnN1, iY, iYn, iYnP2, -1);
	}
}


void AVS2PredicterHW::predPixelIndexBi(int j, int i) {
	int iY, iYn, iYnN1, iYnP2;
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iWidth2 = iWidth << 1;
	int iHeight2 = iHeight << 1;

	iYnN1 = i;
	iY = i;
	iYn = j;
	iYnP2 = j;
	// refer to x  need plus 1 + 128 transfer to [0 256]
	iYnN1 = iYnN1 + 1;
	iY = iY + 1;
	// refer to y  need minus 1 + 128 transfer to [0 256]
	iYn = -iYn - 1;
	iYnP2 = -iYnP2 - 1;
	max_min_indexs[save_index][0] = iYnN1 + CENTRE_OFFSET;
	max_min_indexs[save_index][1] = iY + CENTRE_OFFSET;
	max_min_indexs[save_index][2] = iYn + CENTRE_OFFSET;
	max_min_indexs[save_index][3] = iYnP2 + CENTRE_OFFSET;

}

void AVS2PredicterHW::predPixelIndexVerHor(int j, int i) {
	int iYnN1, iY, iYn, iYnP2;
	int isiX = 0;
	if (uiDirMode == VERT_PRED_AVS2) {
		iYnN1 = i;
		iY = i;
		iYn = i;
		iYnP2 = i;
		isiX = 1;
		saveMaxMinIndex(i, j, iYnN1, iY, iYn, iYnP2, isiX);
	}
	else if (uiDirMode == HOR_PRED_AVS2) {
		iYnN1 = j;
		iY = j;
		iYn = j;
		iYnP2 = j;
		isiX = -1;
		saveMaxMinIndex(i, j, iYnN1, iY, iYn, iYnP2, isiX);
	}

}
void AVS2PredicterHW::predPixel(int j, int i) {
	int  iDx, iDy, iTempDx, iTempDy, iXx, iXy, iYx, iYy;
	int  uixyflag = 0; // 0 for x axis 1 for y axis
	int offset, offsetx, offsety;
	int iX, iY, iXn, iYn, iXnN1, iYnN1, iXnP2, iYnP2;
	int iDxy;
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iWidth2 = iWidth << 1;
	int iHeight2 = iHeight << 1;
	iDx = g_aucDirDx[uiDirMode];
	iDy = g_aucDirDy[uiDirMode];
	uixyflag = g_aucXYflg[uiDirMode];
	iDxy = g_aucSign[uiDirMode];

	if (iDxy < 0) {   // select context pixel based on uixyflag
		if (uixyflag == 0) {
			// case x-line
			iTempDy = j - (-1);
			iTempDx = getContextPixel(uiDirMode, 0, iTempDy, &offset);
			iX = i + iTempDx;
			iY = -1;
		}
		else {
			// case y-line
			iTempDx = i - (-1);
			iTempDy = getContextPixel(uiDirMode, 1, iTempDx, &offset);
			iX = -1;
			iY = j + iTempDy;
		}
	}
	else { // select context pixel based on the distance
		iTempDy = j - (-1);
		iTempDx = getContextPixel(uiDirMode, 0, iTempDy, &offsetx);
		iTempDx = -iTempDx;
		iXx = i + iTempDx;
		iYx = -1;

		iTempDx = i - (-1);
		iTempDy = getContextPixel(uiDirMode, 1, iTempDx, &offsety);
		iTempDy = -iTempDy;
		iXy = -1;
		iYy = j + iTempDy;

		if (iYy <= -1) {
			iX = iXx;
			iY = iYx;
			offset = offsetx;
		}
		else {
			iX = iXy;
			iY = iYy;
			offset = offsety;
		}
	}

	if (iY == -1) {
		if (iDxy < 0) {
			iXnN1 = iX - 1;
			iXn = iX + 1;
			iXnP2 = iX + 2;
		}
		else {
			iXnN1 = iX + 1;
			iXn = iX - 1;
			iXnP2 = iX - 2;
		}

		iXnN1 = min(iWidth2 - 1, iXnN1);
		iX = min(iWidth2 - 1, iX);
		iXn = min(iWidth2 - 1, iXn);
		iXnP2 = min(iWidth2 - 1, iXnP2);
		finalPred(i,j, iXnN1, iX, iXn, iXnP2, offset, 0);

	}
	else if (iX == -1) {
		if (iDxy < 0) {
			iYnN1 = iY - 1;
			iYn = iY + 1;
			iYnP2 = iY + 2;
		}
		else {
			iYnN1 = iY + 1;
			iYn = iY - 1;
			iYnP2 = iY - 2;
		}

		iYnN1 = min(iHeight2 - 1, iYnN1);
		iY = min(iHeight2 - 1, iY);
		iYn = min(iHeight2 - 1, iYn);
		iYnP2 = min(iHeight2 - 1, iYnP2);
		finalPred(i, j, iYnN1, iY, iYn, iYnP2, offset, 1);
	}
}
void AVS2PredicterHW::finalPred(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int offset, int isiY) {
	if (isiY) {
		iYnN1 = -iYnN1 - 1;
		iY = -iY - 1;
		iYn = -iYn - 1;
		iYnP2 = -iYnP2 - 1;
	}
	else {
		iYnN1 = iYnN1 + 1;
		iY = iY + 1;
		iYn = iYn + 1;
		iYnP2 = iYnP2 + 1;
	}
	int iXnN1 = convertSrcIndex2LineRefIndex(iYnN1);
	int iX = convertSrcIndex2LineRefIndex(iY);
	int iXn = convertSrcIndex2LineRefIndex(iYn);
	int iXnP2 = convertSrcIndex2LineRefIndex(iYnP2);
	avs_dst[j][i] = (lineRefer[iXnN1] * (32 - offset) + lineRefer[iX] * (64 - offset) +
		lineRefer[iXn] * (32 + offset) + lineRefer[iXnP2] * offset + 64) >> 7;

}
void AVS2PredicterHW::predPixelVerHor(int j, int i) {
	int iYnN1, iY, iYn, iYnP2;
	int isiY = 0;
	if (uiDirMode == VERT_PRED_AVS2) {
		iYnN1 = i;
		iY = i;
		iYn = i;
		iYnP2 = i;
		isiY = 0;
		finalPredVerHor(i, j, iYnN1, iY, iYn, iYnP2, isiY);
	}
	else if (uiDirMode == HOR_PRED_AVS2) {
		iYnN1 = j;
		iY = j;
		iYn = j;
		iYnP2 = j;
		isiY = 1;
		finalPredVerHor(i, j, iYnN1, iY, iYn, iYnP2, isiY);
	}

}

void AVS2PredicterHW::finalPredVerHor(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int isiY) {
	if (isiY) {
		iYnN1 = -iYnN1 - 1;
		iY = -iY - 1;
		iYn = -iYn - 1;
		iYnP2 = -iYnP2 - 1;
	}
	else {
		iYnN1 = iYnN1 + 1;
		iY = iY + 1;
		iYn = iYn + 1;
		iYnP2 = iYnP2 + 1;
	}
	int iXnN1 = convertSrcIndex2LineRefIndex(iYnN1);
	int iX = convertSrcIndex2LineRefIndex(iY);
	int iXn = convertSrcIndex2LineRefIndex(iYn);
	int iXnP2 = convertSrcIndex2LineRefIndex(iYnP2);
	avs_dst[j][i] = lineRefer[iX];
}
void AVS2PredicterHW::predPixelBi(int j, int i)
{
	int ishift_x = g_log2size[iWidth];
	int ishift_y = g_log2size[iHeight];
	int ishift = min(ishift_x, ishift_y);
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int c, w, wxy, tmp;
	int predx;
	int wy[MAX_CU_SIZE_AVS2];
	//int aboveIndex = biConvertSrcAboveIndex2LineRefIndex(i + 1);
	//int leftIndex = biConvertSrcLeftIndex2LineRefIndex(-1-j);

    //pTop[i] = AbovelineRefer4[aboveIndex];
	//pLeft[j] = leftlineRefer4[leftIndex];
	//printf("pTop[i]:%d ", pTop[i]);
	//printf("pLeft[j]:%d ", pLeft[j]);
	//printf("\n");

	c = (iWidth == iHeight) ? (a + b + 1) >> 1 :
		(((a << ishift_x) + (b << ishift_y)) * 13 + (1 << (ishift + 5))) >> (ishift + 6);
	w = (c << 1) - a - b;
	//printf("pT[x]:%d, pTop[x]:%d \n ", pT[i], pTop[i]);
	tmp = 0;

	tmp = w * j ;
	wy[j] = tmp;
	wxy = 0;
	predx = pLeft[j] + pL[j]*(i+1);
	int tempTop = pTop[i];
	pTop[i] = tempTop +  pT[i];
	wxy = wy[j] * (i);
	//printf("predx:%d, pT[i]:%d, pTop[x]:%d, ishift_x:%d, wxy:%d, offset:%d, ishift_xy:%d \n", predx, pT[i], pTop[i], wxy, ishift_x, offset, ishift_xy);
	avs_dst[j][i] = Clip3(0, (1 << sample_bit_depth) - 1,
		(((predx << ishift_y) + (pTop[i] << ishift_x) + wxy + offset) >> ishift_xy));

}
void AVS2PredicterHW::initTopLeftRefer(int i, int j, int x_offset, int y_offset) {
	int ishift_y = g_log2size[iHeight];
	int ishift_x = g_log2size[iWidth];
	for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
		int aboveIndex = biConvertSrcAboveIndex2LineRefIndex(n + 1);
		if(pTop[n] == -1){
			pTop[n] = AbovelineRefer4[aboveIndex];
			pT[n] = b - pTop[n];
			pTop[n] <<= ishift_y;
		}
	}
	for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
		int leftIndex = biConvertSrcLeftIndex2LineRefIndex(-1 - m);
		if(pLeft[m] == -1){
			pLeft[m] = leftlineRefer4[leftIndex];
			pL[m] = a - pLeft[m];
			pLeft[m] <<= ishift_x;
		}
	}


}
void AVS2PredicterHW::initTopLeft() {
	for (int i = 0; i < MAX_CU_SIZE_AVS2;i++) {
		pTop[i] = -1;
		pLeft[i] = -1;
	}

}
void AVS2PredicterHW::initReferAB() {
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << CU_SIZ_LOG_AVS2) * 2);
	}
	a = pSrc[iWidth];
	b = pSrc[-iHeight];

}
int AVS2PredicterHW::convertSrcIndex2LineRefIndex(int index) {
	int firstBound = first_bouds[0];
	int secondBound = second_bouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut ;
}

int AVS2PredicterHW::biConvertSrcLeftIndex2LineRefIndex(int index) {
	int firstBound = biLeftFirstBouds[0];
	int secondBound = biLeftSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut;
}
int AVS2PredicterHW::biConvertSrcAboveIndex2LineRefIndex(int index) {
	int firstBound = biAboveFirstBouds[0];
	int secondBound = biAboveSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut;
}
int AVS2PredicterHW::getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset)
{
	int imult = g_aucDirDxDy[uiXYflag][uiDirMode][0];
	int ishift = g_aucDirDxDy[uiXYflag][uiDirMode][1];
	int iTempDn = iTempD * imult >> ishift;
	*offset = ((iTempD * imult * 32) >> ishift) - iTempDn * 32;
	return iTempDn;
}

void AVS2PredicterHW::saveMaxMinIndex(int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX)
{
	if (iX == -1) {
		iYnN1 = -iYnN1 - 1;
		iY = -iY - 1;
		iYn = -iYn - 1;
		iYnP2 = -iYnP2 - 1;
	}
	else {
		iYnN1 = iYnN1 + 1;
		iY = iY + 1;
		iYn = iYn + 1;
		iYnP2 = iYnP2 + 1;
	}
	max_min_indexs[save_index][0] = iYnN1 + CENTRE_OFFSET;
	max_min_indexs[save_index][1] = iY + CENTRE_OFFSET;
	max_min_indexs[save_index][2] = iYn + CENTRE_OFFSET;
	max_min_indexs[save_index][3] = iYnP2 + CENTRE_OFFSET;
}


void AVS2PredicterHW::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX)
{
	if (iX == -1) {
		iYnN1 = -iYnN1 - 1;
		iY = -iY - 1;
		iYn = -iYn - 1;
		iYnP2 = -iYnP2 - 1;
	}
	else {
		iYnN1 = iYnN1 + 1;
		iY = iY + 1;
		iYn = iYn + 1;
		iYnP2 = iYnP2 + 1;
	}
	distanMatri->distance_matri[j][i][0] = iYnN1;
	distanMatri->distance_matri[j][i][1] = iY;
	distanMatri->distance_matri[j][i][2] = iYn;
	distanMatri->distance_matri[j][i][3] = iYnP2;
}

void AVS2PredicterHW::initDstData() {
	avs_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		avs_dst[i] = new int[tu_width]();
	}
}

void AVS2PredicterHW::deinitDstData() {
	if (avs_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] avs_dst[i];
		}
		delete[] avs_dst;
		avs_dst = NULL;
	}
}
