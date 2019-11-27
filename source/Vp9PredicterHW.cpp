#include "Vp9PredicterHW.h"


Vp9PredicterHW::Vp9PredicterHW() {
	point_number = 0;
	bs = 0;
}

Vp9PredicterHW::~Vp9PredicterHW() {

}
void Vp9PredicterHW::initVp9Matri(int width, int height, int distanceSize) {
	bs = width;
	point_number = width * height;
	vp9_Matri = new int *[point_number];
	for (int i = 0; i < point_number; i++) {
		vp9_Matri[i] = new int[distanceSize + 1]();
	}

}

void Vp9PredicterHW::deinitVp9Matri() {
	for (int i = 0; i < point_number; i++) {
		delete[] vp9_Matri[i];
	}
	delete[] vp9_Matri;
}

void Vp9PredicterHW::predict() {
	int mode_number = NUM_INTRA_PMODE_VP9;
	int max_cu_size = 64;
	generateOutPath(VP9_PATH, calc_mode);
	generateDigOutPath(VP9_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_VP9; i++) {
		int uiDirMode = g_prdict_mode_vp9[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		outPutWriter->initDstDataFp(VP9_DATA_PATH, uiDirMode);
		outPutWriter->initDigPostionInfoFp(digOutPath, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_VP9; j++) {
			int iWidth = g_cu_size_vp9[j][0];
			int iHeight = g_cu_size_vp9[j][1];
			tu_width = iWidth;
			tu_height = iHeight;
			initDstData();
			initVp9Matri(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			outPutWriter->writeModeInfoToFile(distanMatri);
			outPutWriter->writeDstDataToFile(vp9_dst, iWidth, iHeight);
			deinitDstData();
			deinitVp9Matri();
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}
void Vp9PredicterHW::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	mode = uiDirMode;
	if (uiDirMode == MODE_TM) {
		predIntraTM(distanMatri, uiDirMode);
	}else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}
void Vp9PredicterHW::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
	mode = uiDirMode;
	if (uiDirMode == MODE_TM) {
		predIntraTM(distanMatri, uiDirMode);
	}
	else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}

void Vp9PredicterHW::predIntraAngAdi(DistanceData* distanMatri, int mode) {
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
	uiDirMode = mode;
	generateRefer();
	predIndexAll(distanMatri, mode);
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_VP9);
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(distanMatri, i, j, x_offset, y_offset);
			firstGroupProcess();
			firstGroupCopySrc(lineRefer32, lineRefer256);
			secondGroupProcess();
			secondGroupCopySrc(lineRefer16, lineRefer32);
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
					//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					predPixel(distanMatri,m, n);
				}
			}
		}
	}
	deinitIndexMatri(max_index_number);
}

void Vp9PredicterHW::predIntraTM(DistanceData* distanMatri, int uiDirMode) {

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
	predIndexAll(distanMatri, mode);
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_VP9);
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(distanMatri,i, j, x_offset, y_offset);
			biFirstGroupProcess();
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				//printf("scanByRow: first_bouds[0]:%d, first_bouds[1]:%d, second_bouds[0]:%d, second_bouds[1]:%d \n", 
					//first_bouds[0], first_bouds[1], second_bouds[0], second_bouds[1]);
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					predPixelBi(distanMatri,m, n);
				}
			}
		}
	}
	deinitIndexMatri(max_index_number);

}

void Vp9PredicterHW::biFirstGroupCopySrc(int* dst, int *src) {

	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		dst[refIndex] = src[j];
	}
}
void Vp9PredicterHW::biFirstGroupProcess() {

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
void Vp9PredicterHW::firstGroupProcess() {
	groupType = GROUD_TYPE_MIN;
	extraType = EXTRACT_BOUND_16;
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


void Vp9PredicterHW::firstGroupCopySrc(int* dst, int *src) {
	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		dst[refIndex] = src[j];
	}
}

void Vp9PredicterHW::secondGroupProcess() {
	second_bouds[0] = min_index;
	second_bouds[1] = second_bouds[0] + LINE_REFER_SIZE_16 - 1;
}
void Vp9PredicterHW::secondGroupCopySrc(int* dst, int *src) {

	for (int j = second_bouds[0]; j <= second_bouds[1]; j++) {
		int refIndex = j - second_bouds[0];
		int srcIndex = j - first_bouds[0];
		dst[refIndex] = src[srcIndex];
	}
}
int Vp9PredicterHW::convertSrcIndex2RefIndex(int index) {
	int indexOut = index - CENTRE_OFFSET;
	return indexOut;
}

void Vp9PredicterHW::predPixelIndex(DistanceData* distanMatri, int j, int i) {
	
	int iXnN1 = distanMatri->distance_matri[j][i][0];
	int iX =    distanMatri->distance_matri[j][i][1];
	int iXn =   distanMatri->distance_matri[j][i][2];
	int iXnP2 = distanMatri->distance_matri[j][i][3];
    saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
}
void Vp9PredicterHW::predIndexAll(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int delta_pos = 0;
	int iXnN1 = 0;
	int iX = 0;
	int iXn = 0;
	int iXnP2 = 0;
	int predictPixel;
	if (uiDirMode == 1) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((i == 0) && (j != iHeight - 1)) {
					iXnN1 = j;
					iX = j + 1;
					iXn = j;
					iXnP2 = j;
					predictPixel = AVG2(refLeft[iXnN1], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((i == 1) && (j != iHeight - 1) && (j != iHeight - 2)) {
					iXnN1 = j;
					iX = j + 1;
					iXn = j + 2;
					iXnP2 = j + 2;
					predictPixel = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((i == 1) && (j == iHeight - 2)) {
					iXnN1 = bs - 2;
					iX = bs - 1;
					iXn = bs - 1;
					iXnP2 = bs - 1;
					predictPixel = (refLeft[iXnN1] + refLeft[iX] * 3 + 2) >> 2;
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if (j == iHeight - 1) {
					iXnN1 = bs - 1;
					iX = bs - 1;
					iXn = bs - 1;
					iXnP2 = bs - 1;
					predictPixel = refLeft[iX];
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, (bs - 1)*bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
		for (int i = 2; i < iWidth; i++) {
			for (int j = 0; j < iHeight - 1; j++) {
				iXnN1 = vp9_Matri[((j + 1)*bs + i - 2)][0];
				iX = vp9_Matri[((j + 1)*bs + i - 2)][1];
				iXn = vp9_Matri[((j + 1)*bs + i - 2)][2];
				iXnP2 = vp9_Matri[((j + 1)*bs + i - 2)][3];
				predictPixel = vp9_Matri[((j + 1)*bs + i - 2)][4];
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}


	if (uiDirMode == 2) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((j % 2) == 1) {
					iXnN1 = (j / 2) + i;
					iX = (j / 2) + i + 1;
					iXn = (j / 2) + i + 2;
					iXnP2 = (j / 2) + i + 2;
					predictPixel = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
				}
				else {
					iXnN1 = (j / 2) + i;
					iX = (j / 2) + i + 1;
					iXn = (j / 2) + i;
					iXnP2 = (j / 2) + i;
					predictPixel = AVG2(refAbove[iXnN1], refAbove[iX]);
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}

	if (uiDirMode == 3) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((i + j + 2) < 2 * bs) {
					iXnN1 = j + i;
					iX = j + i + 1;
					iXn = j + i + 2;
					iXnP2 = j + i + 2;
					predictPixel = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
				}
				else {
					iXnN1 = bs * 2 - 1;
					iX = bs * 2 - 1;
					iXn = bs * 2 - 1;
					iXnP2 = bs * 2 - 1;
					predictPixel = refAbove[iX];
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
	if (uiDirMode == 4) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((j == 0)) {
					iXnN1 = i - 1;
					iX = i;
					iXn = i-1;
					iXnP2 = i-1;
					predictPixel = AVG2(refAbove[iXnN1], refAbove[iX]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 1) && (i != 0)) {
					iXnN1 = i - 2;
					iX = i - 1;
					iXn = i;
					iXnP2 = i;
					predictPixel = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 1) && (i == 0)) {
					iXnN1 = -2;
					iX = -1;
					iXn = 0;
					iXnP2 = 0; // left[ 0 ] + top[ -1 ] * 2 + top[ 0 ] + 2
					predictPixel = AVG3(refLeft[iXnN1+2], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 2) && (i == 0)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					predictPixel = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, 2 * bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if (i == 0) {
					iXnN1 = j - 3;
					iX = j - 2;
					iXn = j - 1;
					iXnP2 = j - 1;
					predictPixel = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
		for (int j = 2; j < iHeight; j++) {
			for (int i = 1; i < iWidth; i++) {
				iXnN1 = vp9_Matri[(j - 2)*bs + i - 1][0];
				iX = vp9_Matri[(j - 2)*bs + i - 1][1];
				iXn = vp9_Matri[(j - 2)*bs + i - 1][2];
				iXnP2 = vp9_Matri[(j - 2)*bs + i - 1][3];
				predictPixel = vp9_Matri[(j - 2)*bs + i - 1][4];
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}

	if (uiDirMode == 5) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((j == 0) && (i == 0)) {
					iXnN1 = -2;
					iX = -1;
					iXn = 0;
					iXnP2 = 0;
					predictPixel = AVG3(refLeft[iXnN1 + 2], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, 0, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 0) && (i != 0)) {
					iXnN1 = i - 2;
					iX = i - 1;
					iXn = i;
					iXnP2 = i;
					predictPixel = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 1) && (i == 0)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					predictPixel = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j >= 2) && (i == 0)) {
					iXnN1 = j - 2;
					iX = j - 1;
					iXn = j;
					iXnP2 = j;
					predictPixel = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
		for (int j = 1; j < iHeight; j++) {
			for (int i = 1; i < iWidth; i++) {
				iXnN1 = vp9_Matri[(j - 1)*bs + i - 1][0];
				iX = vp9_Matri[(j - 1)*bs + i - 1][1];
				iXn = vp9_Matri[(j - 1)*bs + i - 1][2];
				iXnP2 = vp9_Matri[(j - 1)*bs + i - 1][3];
				predictPixel = vp9_Matri[(j - 1)*bs + i - 1][4];
				predictPixel = predictPixel;
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
	if (uiDirMode == 6) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if ((j == 0) && (i == 0)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -1;
					iXnP2 = -1;
					predictPixel = AVG2(refAbove[iXnN1], refLeft[iX + 2]);
					saveVp9Matri(vp9_Matri, 0, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j != 0) && (i == 0)) {
					iXnN1 = j - 1;
					iX = j;
					iXn = j - 1;
					iXnP2 = j - 1;
					predictPixel = AVG2(refLeft[iXnN1], refLeft[iX]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 0) && (i == 1)) {
					iXnN1 = -2;
					iX = -1;
					iXn = 0;
					iXnP2 = 0;
					predictPixel = AVG3(refLeft[iXn], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, 1, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j == 1) && (i == 1)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					predictPixel = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, bs + 1, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				else if ((j >= 2) && (i == 1)) {
					iXnN1 = j - 2;
					iX = j - 1;
					iXn = j;
					iXnP2 = j;
					predictPixel = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((i >= 2) && (j == 0)) {
					iXnN1 = i - 3;
					iX = i - 2;
					iXn = i - 1;
					iXnP2 = i - 1;
					predictPixel = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, predictPixel);
				}
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
		for (int j = 1; j < iHeight; j++) {
			for (int i = 2; i < iWidth; i++) {
				iXnN1 = vp9_Matri[(j - 1)*bs + i - 2][0];
				iX = vp9_Matri[(j - 1)*bs + i - 2][1];
				iXn = vp9_Matri[(j - 1)*bs + i - 2][2];
				iXnP2 = vp9_Matri[(j - 1)*bs + i - 2][3];
				predictPixel = vp9_Matri[(j - 1)*bs + i - 2][4];
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, predictPixel);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
	if (uiDirMode == 7) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				iXnN1 = i;
				iX = i;
				iXn = i;
				iXnP2 = i;
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
	if (uiDirMode == 8) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				iXnN1 = j;
				iX = j;
				iXn = j;
				iXnP2 = j;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
	if (uiDirMode == 9) {
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				iXnN1 = i;
				iX = i;
				iXn = -j- 2;
				iXnP2 = -j-2;
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
}
void Vp9PredicterHW::saveVp9Matri(int ** vp9Matri, int index, int iYnN1, int iY, int iYn, int iYnP2, int predictData)
{
	vp9Matri[index][0] = iYnN1;
	vp9Matri[index][1] = iY;
	vp9Matri[index][2] = iYn;
	vp9Matri[index][3] = iYnP2;
	vp9Matri[index][4] = predictData;
}
void Vp9PredicterHW::predPixel(DistanceData* distanMatri, int j, int i) {
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iXnN1 = distanMatri->distance_matri[j][i][0];
	int iX = distanMatri->distance_matri[j][i][1];
	int iXn = distanMatri->distance_matri[j][i][2];
	int iXnP2 = distanMatri->distance_matri[j][i][3];
	int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
	if (uiDirMode == 1) {
		if ((i + 2 * j) == (2 * bs - 3)) {
			vp9_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] * 3 + 2) >> 2;;
		}
		else if ((i + 2 * j) > (2 * bs - 3)) {
			vp9_dst[j][i] = lineRefer16[iX];
		}
		else {
			if (i % 2 == 1) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
		}
	}

	if (uiDirMode == 2) {
		if ((j % 2) == 1) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
		else {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
	}

	if (uiDirMode == 3) {
		if ((i + j + 2) < 2 * bs) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
		else {
			vp9_dst[j][i] = lineRefer16[iX];
		}

	}
	if (uiDirMode == 4) {
		if (j % 2 == 1) {
			if (i == ((j - 1) / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else if (i > ((j - 1) / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
		}
		else {
			if ((i + 1) == (j / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else if ((i + 1) > (j / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}

		}
		saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
	}

	if (uiDirMode == 5) {
		if (j < i) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
		else if (j == i) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
		else if (j == (i + 1)) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
		}
		else if (j > (i + 1)) {
			vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);

		}
	}
	if (uiDirMode == 6) {
		if (i % 2 == 1) {
			if (j < ((i - 1) / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else if (j == ((i - 1) / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else if (j == ((i - 1) / 2 + 1)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
		}
		else {
			if (j < (i / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else if (j == (i / 2)) {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
			else {
				vp9_dst[j][i] = AVG3(lineRefer16[iXnN1], lineRefer16[iX], lineRefer16[iXn]);
			}
		}
	}
	if (uiDirMode == 7) {
	    vp9_dst[j][i] = lineRefer16[iX];
	}

	if (uiDirMode == 8) {
		vp9_dst[j][i] = lineRefer16[iX];
	}

}
void Vp9PredicterHW::predPixelBi(DistanceData* distanMatri, int j, int i)
{
	int iXnN1 = distanMatri->distance_matri[j][i][0];
	int iX = distanMatri->distance_matri[j][i][1];
	int iXn = distanMatri->distance_matri[j][i][2];
	int iXnP2 = distanMatri->distance_matri[j][i][3];
	int aboveIndex = biConvertSrcAboveIndex2LineRefIndex(iX);
	int top =AbovelineRefer4[aboveIndex];
	int leftIndex = biConvertSrcLeftIndex2LineRefIndex(iXn);
	int left  = leftlineRefer4[leftIndex];
	//printf("predx:%d, pT[i]:%d, pTop[x]:%d, ishift_x:%d, wxy:%d, offset:%d, ishift_xy:%d \n", predx, pT[i], pTop[i], wxy, ishift_x, offset, ishift_xy);
	vp9_dst[j][i] = clip_pixel(top  + left - ytop_left);

}
int Vp9PredicterHW::convertLeftSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2) {
	int secondBouds = second_bouds[0];
	*iYnN1 = -*iYnN1 - 2 + CENTRE_OFFSET_VP9 - secondBouds;
	*iY = -*iY - 2 + CENTRE_OFFSET_VP9 - secondBouds;
	*iYn = -*iYn - 2 + CENTRE_OFFSET_VP9 - secondBouds;
	*iYnP2 = -*iYnP2 - 2 + CENTRE_OFFSET_VP9 - secondBouds;
	return 0;
}


int Vp9PredicterHW::convertAboveSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2) {
	int secondBouds = second_bouds[0];
	*iYnN1 = *iYnN1 + CENTRE_OFFSET_VP9 - secondBouds;
	*iY = *iY + CENTRE_OFFSET_VP9 - secondBouds;
	*iYn = *iYn + CENTRE_OFFSET_VP9 - secondBouds;
	*iYnP2 = *iYnP2 + CENTRE_OFFSET_VP9 - secondBouds;
	return 0;
}

int Vp9PredicterHW::biConvertSrcLeftIndex2LineRefIndex(int index) {
	int firstBound = biLeftFirstBouds[0];
	int secondBound = biLeftSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET_VP9 - firstBound - secondBound;
	return indexOut;
}
int Vp9PredicterHW::biConvertSrcAboveIndex2LineRefIndex(int index) {
	int firstBound = biAboveFirstBouds[0];
	int secondBound = biAboveSecondBouds[0];
	int indexOut = index + CENTRE_OFFSET_VP9 - firstBound - secondBound;
	return indexOut;
}

int Vp9PredicterHW::convertLeftSrcIndex2RefIndex(int index) {
	
	int indexOut = -index -  2 + CENTRE_OFFSET_VP9;
	return indexOut;
}
int Vp9PredicterHW::convertAboveSrcIndex2RefIndex(int index) {
	int indexOut = index + CENTRE_OFFSET_VP9;
	return indexOut;
}
void Vp9PredicterHW::generateRefer() {
	int  Above[2 * MAX_CU_SIZE_VP9 + 1];
	int  Left[MAX_CU_SIZE_VP9 + 1];
	int *refLeft = Left + 1;
	int *refAbove = Above + 1;
	convertSrc(Above, Left);
	for (int i = 0; i < 2 * MAX_CU_SIZE_VP9; i++) {
		int lineRefIndex = convertAboveSrcIndex2RefIndex(i);
		lineRefer256[lineRefIndex] = refAbove[i];
	}
	for (int i = 0; i < MAX_CU_SIZE_VP9; i++) {
		int lineRefIndex = convertLeftSrcIndex2RefIndex(i);
		lineRefer256[lineRefIndex] = refLeft[i];
	}
	lineRefer256[CENTRE_OFFSET_VP9 - 1] = Above[0];// lineRefer[128] = refLeft[-1] = refAbove[-1]
	ytop_left = Above[0];
}

void Vp9PredicterHW::get4X4MaxMinIndex(DistanceData* distanMatri, int i, int j, int x_offset, int y_offset) {

	int x = i * x_offset;
	int x4 = x_offset * (i + 1) - 1;
	int y = j * y_offset;
	int y4 = y_offset * (j + 1) - 1;
	save_index = 0;
	predPixelIndex(distanMatri,y, x);
	save_index++;
	predPixelIndex(distanMatri,y4, x);
	save_index++;
	predPixelIndex(distanMatri,y, x4);
	save_index++;
	predPixelIndex(distanMatri,y4, x4);
	if (mode == MODE_TM) {
		bi_left_min = max_min_indexs[1][3];
		bi_above_min = max_min_indexs[0][0];
		bi_above_max = max_min_indexs[2][0];
	}else{ 
		int max_index_number = save_index + 1;
		max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_VP9, max_index_number);
		min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_VP9, max_index_number);
	}
}


void Vp9PredicterHW::saveMaxMinIndex(int  iYnN1, int iY, int iYn, int iYnP2)
{
	max_min_indexs[save_index][0] = iYnN1 + CENTRE_OFFSET_VP9;
	max_min_indexs[save_index][1] = iY + CENTRE_OFFSET_VP9;
	max_min_indexs[save_index][2] = iYn + CENTRE_OFFSET_VP9;
	max_min_indexs[save_index][3] = iYnP2 + CENTRE_OFFSET_VP9;
}


void Vp9PredicterHW::convertSrc(int* above, int *left) {
	if ((src_data && src_data->vp9_src)) {
		int src_stride = MAX_CU_SIZE_VP9 * NUM_EXTEND_SIZE_VP9 + 1;
		int max_width = MAX_CU_SIZE_VP9 * NUM_EXTEND_SIZE_VP9 + 1;
		for (int i = 0; i < bs; i++) {
			above[i] = src_data->vp9_src[i];
		}
		for (int i = bs; i < max_width; i++) {
			above[i] = src_data->vp9_src[bs];
		}
		for (int i = 0; i <= MAX_CU_SIZE_VP9; i++) {
			left[i] = src_data->vp9_src[i*src_stride];
		}
	}
}


void Vp9PredicterHW::initDstData() {
	vp9_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		vp9_dst[i] = new int[tu_width]();
	}
}

void Vp9PredicterHW::deinitDstData() {
	if (vp9_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] vp9_dst[i];
		}
		delete[] vp9_dst;
		vp9_dst = NULL;
	}
}