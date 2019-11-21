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
			initDstData();
			DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
			PredIntraAngAdi(distanMatri, uiDirMode);
			outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
			deinitDstData();
			delete distanMatri;
		}
	}
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
	scanType = g_prdict_mode_row[mode];

	uixyflag = g_aucXYflg[uiDirMode];
	iDxy = g_aucSign[uiDirMode];


	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
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
			firstGroupCopySrc64(lineRefer64,pSrc);
			firstGroupCopySrc(refer4X4, lineRefer64);
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
		secondGroupCopySrc(lineRefer, refer4X4);
		//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
			//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
		for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
			predPixel(m, n);
		}
	}
}

void AVS2PredicterHW::scanByCol(int i, int j, int x_offset, int y_offset) {
	for (int m = i* x_offset; m < x_offset*(i + 1); m++) {
		getLineMaxMinIndex(i,j,m,x_offset, y_offset);
		secondGroupProcess();
		secondGroupCopySrc(lineRefer, refer4X4);
		for (int n = j * x_offset; n < y_offset*(j + 1); n++) {
			predPixel(n, m);
		}
	}


}
void AVS2PredicterHW::getLineMaxMinIndex(int i, int j, int m,int x_offset, int y_offset) {
	
	if (scanType == SCAN_TYPE_ROW) {
		save_index = 0;
		for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
			predPixelIndex(m, n);
			save_index++;
		}
	}else {
		save_index = 0;
		for (int n = j * x_offset; n < y_offset*(j + 1); n++) {
			predPixelIndex(n, m);
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
	predPixelIndex(y,x);
	save_index++;
	predPixelIndex(y4,x);
	save_index++;
	predPixelIndex(y,x4);
	save_index++;
	predPixelIndex(y4,x4);
	int max_index_number = save_index + 1;
	max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
	min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_AVS, max_index_number);
}

void AVS2PredicterHW::firstGroupProcess() {
	groupType = GROUD_TYPE_MIN;
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
void AVS2PredicterHW::group256(int index) {
	int left128 = 0;
	int right128 = 0;
	int boud128 = MAX_INDEX/2;
	if (index <= boud128) {
		left128 = 0;
		right128 = boud128;
	}else {
		left128 = boud128 + 1;
		right128 = MAX_INDEX;
	}
	group128(left128, right128, index);

}

void AVS2PredicterHW::secondGroupProcess() {
	group32(second_min);
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

void AVS2PredicterHW::firstGroupCopySrc(int* dst, int *src) {

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

void AVS2PredicterHW::firstGroupCopySrc64(int* dst, int *src) {

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

void AVS2PredicterHW::group128(int left, int right, int index) {
	int left64 = 0;
	int right64 = 0;
	int boud64 = left + (right - left)/2 ;
	if (index <= boud64) {
	    left64 = left;
		right64 = boud64;
	}else {
		left64 = boud64 + 1;
		right64 = right;
	}
	group64(left64, right64, index);
}

void AVS2PredicterHW::group64(int left, int right, int index) {
	int bounds[2] = { 0 };
	int boud32 = left + (right - left) / 2;
	if (index <= boud32) {
		bounds[0] = left;
		bounds[1] = boud32;
	}else {
		bounds[0] = boud32 + 1;
		bounds[1] = right;
	}
	if (groupType == GROUD_TYPE_MIN) {
		minBounds[0] = bounds[0];
		minBounds[1] = bounds[1];
	}else {
		maxBounds[0] = bounds[0];
		maxBounds[1] = bounds[1];
	}
}

void AVS2PredicterHW::group32(int index) {
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
void AVS2PredicterHW::finalPred(int i , int j, int  iYnN1, int iY, int iYn, int iYnP2, int offset, int isiY) {
	if (isiY) {
		iYnN1 = -iYnN1 - 1;
		iY = -iY - 1;
		iYn = -iYn - 1;
		iYnP2 = -iYnP2 - 1;
	}else {
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

int AVS2PredicterHW::convertSrcIndex2LineRefIndex(int index) {
	int firstBound = first_bouds[0];
	int secondBound = second_bouds[0];
	int indexOut = index + CENTRE_OFFSET - firstBound - secondBound;
	return indexOut ;
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
