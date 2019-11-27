#include "H264PredicterHW.h"


H264PredicterHW::H264PredicterHW() {
	max_dst_number = (MAX_CU_SIZE_H264 *NUM_EXTEND_SIZE_H264 + 1)* (MAX_CU_SIZE_H264);
	h264_dst = NULL;
	tu_width = MAX_CU_SIZE_H264;
	tu_height = MAX_CU_SIZE_H264;
	uiDirMode = 0;
}

H264PredicterHW::~H264PredicterHW() {

}

void H264PredicterHW::predict() {
	int mode_max_index = NUM_INTRA_PMODE_264;
	int max_cu_size = 64;
	generateOutPath(H264_PATH, calc_mode);
	generateDigOutPath(H264_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_max_index, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_264; i++) {
		int uiDirMode = g_prdict_mode_264[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		outPutWriter->initDstDataFp(H264_DATA_PATH, uiDirMode);
		outPutWriter->initDigPostionInfoFp(digOutPath, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_264; j++) {
			int iWidth = g_cu_size_264[j][0];
			int iHeight = g_cu_size_264[j][1];
			tu_width = iWidth;
			tu_height = iHeight;
			initDstData();
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			outPutWriter->writeModeInfoToFile(distanMatri);
			outPutWriter->writeDstDataToFile(h264_dst, iWidth, iHeight);
			deinitDstData();
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}
void H264PredicterHW::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	if ((uiDirMode == PLANE_16) && (tu_width == 16 && tu_height == 16)) {
		predIntraPlane(distanMatri, uiDirMode);
	}
	else if (uiDirMode == MODE_DC_H264) {
		predIntraDC(distanMatri, uiDirMode);
	}
	else {
		predIntraAngAdi(distanMatri, uiDirMode);
	}
}
void H264PredicterHW::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264 + 1];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above + 1;
	int *refLeft = Left + 1;
	int iXnN1, iX, iXn, iXnP2;
	if (((iWidth == 4) && (iHeight == 4)) || ((iWidth == 8) && (iHeight == 8))) {
		if (uiDirMode == 0) {
			int lumaMode = 2;
			predIntraDC(distanMatri, lumaMode);
		}
		if (uiDirMode == 1) {
			int lumaMode = 0;
			predIntraAngAdi(distanMatri, lumaMode);
		}
		if (uiDirMode == 2) {
			int lumaMode = 1;
			predIntraAngAdi(distanMatri, lumaMode);
		}
		if (uiDirMode == 3) {
			int ih, iv, ib, ic, iaa;
			int i, j;
			int cr_MB_x = iWidth;
			int cr_MB_y = iHeight;
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
					h264_dst[j][i] = iClip1(MAX_PIXEL_VALUE, ((i * ib + plane) >> 5));
			}
		}
	}
}

void H264PredicterHW::predIntraDC(DistanceData* distanMatri, int mode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264 + 1];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above + 1;
	int *refLeft = Left + 1;

	if (mode == 2) {
		int  sumPixel = 0;
		for (int i = 0; i < iWidth; i++) {
			sumPixel += refAbove[i];
		}
		for (int j = 0; j < iHeight; j++) {
			sumPixel += refLeft[j];
		}
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				if((iWidth == 4) && (iHeight == 4)){
					h264_dst[j][i] = (sumPixel + 4) >> 3;
				}
				else if ((iWidth == 8) && (iHeight == 8)) {
					h264_dst[j][i] = (sumPixel + 8) >> 4;
				}
				else if ((iWidth == 16) && (iHeight == 16)) {
					h264_dst[j][i] = (sumPixel + 16) >> 5;;
				}
			}
		}

	}

}

void H264PredicterHW::predIntraPlane(DistanceData* distanMatri, int mode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264 + 1];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above + 1;
	int *refLeft = Left + 1;

	if (uiDirMode == 3) {
		int  H = 0;
		int  V = 0;
		int a, b, c;
		for (int i = 0; i < iWidth / 2; i++) {
			H += (i + 1)*(refAbove[8 + i] - refAbove[6 - i]);
		}
		for (int j = 0; j < iHeight / 2; j++) {
			V += (j + 1)*(refLeft[8 + j] - refLeft[6 - j]);
		}

		a = 16 * (refAbove[iWidth - 1] + refLeft[iHeight - 1]);
		b = (5 * H + 32) >> 6;
		c = (5 * V + 32) >> 6;
		for (int j = 0; j < iHeight; j++) {
			for (int i = 0; i < iWidth; i++) {
				int x = (a + b * (i - 7) + c * (j - 7) + 16) >> 5;
				int height = 255;
				h264_dst[j][i] = iClip1(height, x);
			}
		}
	}
}

void H264PredicterHW::predIntraAngAdi(DistanceData* distanMatri, int mode) {
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
	initIndexMatri(max_index_number, NUM_DISTANCE_SIZE_264);
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			get4X4MaxMinIndex(i, j, x_offset, y_offset);
			firstGroupProcess();
			firstGroupCopySrc(lineRefer32, lineRefer256);
			secondGroupProcess();
			secondGroupCopySrc(lineRefer16, lineRefer32);
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

void H264PredicterHW::firstGroupProcess() {
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


void H264PredicterHW::firstGroupCopySrc(int* dst, int *src) {
	//  case 256 point
	if (first_bouds[1] == MAX_INDEX) {
		first_bouds[1] = MAX_INDEX + 1;
	}
	for (int j = first_bouds[0]; j <= first_bouds[1]; j++) {
		int refIndex = j - first_bouds[0];
		dst[refIndex] = src[j];
	}
}

void H264PredicterHW::secondGroupProcess() {
	second_bouds[0] = min_index;
	second_bouds[1] = second_bouds[0] + LINE_REFER_SIZE_16 - 1;
}
void H264PredicterHW::secondGroupCopySrc(int* dst, int *src) {

	for (int j = second_bouds[0]; j <= second_bouds[1]; j++) {
		int refIndex = j - second_bouds[0];
		int srcIndex = j - first_bouds[0];
		dst[refIndex] = src[srcIndex];
	}
}
int H264PredicterHW::convertSrcIndex2RefIndex(int index) {
	int indexOut = index - CENTRE_OFFSET;
	return indexOut;
}

void H264PredicterHW::predPixelIndex(int j, int i) {
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iXnN1, iX, iXn, iXnP2;
	if ((iWidth == 4) && (iHeight == 4)) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 3) {
			if ((i == 3) && (j == 3)) {
				iXnN1 = 6;
				iX = 6;
				iXn = 7;
				iXnP2 = 7;
			}else {
				iXnN1 = i + j;
				iX = i + j + 1;
				iXn = i + j + 2;
				iXnP2 = i + j + 2;
			}
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 4) {
			if (i > j) {
				iXnN1 = i - j - 2;
				iX = i - j - 1;
				iXn = i - j;
				iXnP2 = i - j;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if (i < j) {
				iXnN1 = j - i - 2;
				iX = j - i - 1;
				iXn = j - i;
				iXnP2 = j - i;
			}
			else {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 5) {
			int zVR = 2 * i - j;
			if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6)) {
				iXnN1 = i - (j >> 1) - 1;
				iX = i - (j >> 1);
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if ((zVR == 1) || (zVR == 3) || (zVR == 5)) {
				iXnN1 = i - (j >> 1) - 2;
				iX = i - (j >> 1) - 1;
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if (zVR == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			else {
				iXnN1 = j - 1;
				iX = j - 2;
				iXn = j - 3;
				iXnP2 = j - 3;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 6) {
			int zHD = 2 * j - i;
			if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6)) {
				iXnN1 = j - (i >> 1) - 1;
				iX = j - (i >> 1);
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
			}
			else if ((zHD == 1) || (zHD == 3) || (zHD == 5)) {
				iXnN1 = j - (i >> 1) - 2;
				iX = j - (i >> 1) - 1;
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
			}
			else if (zHD == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			else {
				iXnN1 = i - 1;
				iX = i - 2;
				iXn = i - 3;
				iXnP2 = i - 3;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 7) {
			if ((j == 0) || (j == 2)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 1;
				iXnP2 = i + (j >> 1) + 1;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if ((j == 1) || (j == 3)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 2;
				iXnP2 = i + (j >> 1) + 2;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 8) {
			int  zHU = 2 * j + i;
			if ((zHU == 0) || (zHU == 2) || (zHU == 4)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 1;
				iXnP2 = j + (i >> 1) + 1;
			}
			else if ((zHU == 1) || (zHU == 3)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 2;
				iXnP2 = j + (i >> 1) + 2;
			}
			else if (zHU == 5) {
				iXnN1 = 2;
				iX = 3;
				iXn = 3;
				iXnP2 = 3;
			}
			else {
				iXnN1 = 3;
				iX = 3;
				iXn = 3;
				iXnP2 = 3;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
	}

	if (iHeight == 8 && iWidth == 8) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 3) {
			if ((i == 7) && (j == 7)) {
				iXnN1 = 14;
				iX = 14;
				iXn = 15;
				iXnP2 = 15;
			}
			else {
				iXnN1 = i + j;
				iX = i + j + 1;
				iXn = i + j + 2;
				iXnP2 = i + j + 2;
			}
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 4) {
			if (i > j) {
				iXnN1 = i - j - 2;
				iX = i - j - 1;
				iXn = i - j;
				iXnP2 = i - j;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if (i < j) {
				iXnN1 = j - i - 2;
				iX = j - i - 1;
				iXn = j - i;
				iXnP2 = j - i;
			}
			else {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 5) {
			int zVR = 2 * i - j;
			if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6) || (zVR == 8) || (zVR == 10) || (zVR == 12) || (zVR == 14)) {
				iXnN1 = i - (j >> 1) - 1;
				iX = i - (j >> 1);
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if ((zVR == 1) || (zVR == 3) || (zVR == 5) || (zVR == 7) || (zVR == 9) || (zVR == 11) || (zVR == 13)) {
				iXnN1 = i - (j / 2) - 2;
				iX = i - (j / 2) - 1;
				iXn = i - (j / 2);
				iXnP2 = i - (j / 2);
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			else if (zVR == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			else {
				iXnN1 = j - 2 * i - 1;
				iX = j - 2 * i - 2;
				iXn = j - 2 * i - 3;
				iXnP2 = j - 2 * i - 3;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 6) {
			int zHD = 2 * j - i;
			if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6) || (zHD == 8) || (zHD == 10) || (zHD == 12) || (zHD == 14)) {
				iXnN1 = j - (i >> 1) - 1;
				iX = j - (i >> 1);
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
			}
			else if ((zHD == 1) || (zHD == 3) || (zHD == 5) || (zHD == 7) || (zHD == 9) || (zHD == 11) || (zHD == 13)) {
				iXnN1 = j - (i >> 1) - 2;
				iX = j - (i >> 1) - 1;
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
			}
			else if (zHD == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
			}
			else {
				iXnN1 = i - 2 * j - 1;
				iX = i - 2 * j - 2;
				iXn = i - 2 * j - 3;
				iXnP2 = i - 2 * j - 3;
				convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 7) {
			if ((j == 0) || (j == 2) || (j == 4) || (j == 6)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 1;
				iXnP2 = i + (j >> 1) + 1;
			}
			else if ((j == 1) || (j == 3) || (j == 5) || (j == 7)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 2;
				iXnP2 = i + (j >> 1) + 2;
			}
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}

		if (uiDirMode == 8) {
			int zHU = 2 * j + i;
			if ((zHU == 0) || (zHU == 2) || (zHU == 4) || (zHU == 6) || (zHU == 8) || (zHU == 10) || (zHU == 12)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 1;
				iXnP2 = j + (i >> 1) + 1;
			}
			else if ((zHU == 1) || (zHU == 3) || (zHU == 5) || (zHU == 7) || (zHU == 9) || (zHU == 11)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 2;
				iXnP2 = j + (i >> 1) + 2;
			}
			else if (zHU == 13) {
				iXnN1 = 6;
				iX = 7;
				iXn = 7;
				iXnP2 = 7;
			}
			else {
				iXnN1 = 7;
				iX = 7;
				iXn = 7;
				iXnP2 = 7;
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
	}
	if (iHeight == 16 && iWidth == 16) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
	}

}

void H264PredicterHW::predPixel(int j, int i) {
	int iWidth = tu_width;
	int iHeight = tu_height;
	int iXnN1, iX, iXn, iXnP2;
	if ((iWidth == 4) && (iHeight == 4)) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}

		if (uiDirMode == 3) {
			if ((i == 3) && (j == 3)) {
				iXnN1 = 6;
				iX = 6;
				iXn = 7;
				iXnP2 = 7;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1,&iX,&iXn,&iXnP2);
				h264_dst[j][i] = (lineRefer16[iX] + 3 * lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = i + j;
				iX = i + j + 1;
				iXn = i + j + 2;
				iXnP2 = i + j + 2;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 4) {
			if (i > j) {
				iXnN1 = i - j - 2;
				iX = i - j - 1;
				iXn = i - j;
				iXnP2 = i - j;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (i < j) {
				iXnN1 = j - i - 2;
				iX = j - i - 1;
				iXn = j - i;
				iXnP2 = j - i;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 5) {
			int zVR = 2 * i - j;
			if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6)) {
				iXnN1 = i - (j >> 1) - 1;
				iX = i - (j >> 1);
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zVR == 1) || (zVR == 3) || (zVR == 5)) {
				iXnN1 = i - (j >> 1) - 2;
				iX = i - (j >> 1) - 1;
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (zVR == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = j - 1;
				iX = j - 2;
				iXn = j - 3;
				iXnP2 = j - 3;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 6) {
			int zHD = 2 * j - i;
			if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6)) {
				iXnN1 = j - (i >> 1) - 1;
				iX = j - (i >> 1);
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zHD == 1) || (zHD == 3) || (zHD == 5)) {
				iXnN1 = j - (i >> 1) - 2;
				iX = j - (i >> 1) - 1;
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (zHD == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXnP2] + 2) >> 2;
			}
			else {
				iXnN1 = i - 1;
				iX = i - 2;
				iXn = i - 3;
				iXnP2 = i - 3;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}
		if (uiDirMode == 7) {
			if ((j == 0) || (j == 2)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 1;
				iXnP2 = i + (j >> 1) + 1;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((j == 1) || (j == 3)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 2;
				iXnP2 = i + (j >> 1) + 2;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 8) {
			int  zHU = 2 * j + i;
			if ((zHU == 0) || (zHU == 2) || (zHU == 4)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 1;
				iXnP2 = j + (i >> 1) + 1;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zHU == 1) || (zHU == 3)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 2;
				iXnP2 = j + (i >> 1) + 2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;;
			}
			else if (zHU == 5) {
				iXnN1 = 2;
				iX = 3;
				iXn = 3;
				iXnP2 = 3;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 3 * lineRefer16[iX] + 2) >> 2;
			}
			else {
				iXnN1 = 3;
				iX = 3;
				iXn = 3;
				iXnP2 = 3;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = lineRefer16[iXnN1];
			}
		}
	}

	if (iHeight == 8 && iWidth == 8) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}
		if (uiDirMode == 3) {
			if ((i == 7) && (j == 7)) {
				iXnN1 = 14;
				iX = 14;
				iXn = 15;
				iXnP2 = 15;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iX] + 3 * lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = i + j;
				iX = i + j + 1;
				iXn = i + j + 2;
				iXnP2 = i + j + 2;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 4) {
			if (i > j) {
				iXnN1 = i - j - 2;
				iX = i - j - 1;
				iXn = i - j;
				iXnP2 = i - j;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (i < j) {
				iXnN1 = j - i - 2;
				iX = j - i - 1;
				iXn = j - i;
				iXnP2 = j - i;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}
		if (uiDirMode == 5) {
			int zVR = 2 * i - j;
			if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6) || (zVR == 8) || (zVR == 10) || (zVR == 12) || (zVR == 14)) {
				iXnN1 = i - (j >> 1) - 1;
				iX = i - (j >> 1);
				iXn = i - (j >> 1);
				iXnP2 = i - (j >> 1);
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zVR == 1) || (zVR == 3) || (zVR == 5) || (zVR == 7) || (zVR == 9) || (zVR == 11) || (zVR == 13)) {
				iXnN1 = i - (j / 2) - 2;
				iX = i - (j / 2) - 1;
				iXn = i - (j / 2);
				iXnP2 = i - (j / 2);
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (zVR == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = j - 2 * i - 1;
				iX = j - 2 * i - 2;
				iXn = j - 2 * i - 3;
				iXnP2 = j - 2 * i - 3;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}
		if (uiDirMode == 6) {
			int zHD = 2 * j - i;
			if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6) || (zHD == 8) || (zHD == 10) || (zHD == 12) || (zHD == 14)) {
				iXnN1 = j - (i >> 1) - 1;
				iX = j - (i >> 1);
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zHD == 1) || (zHD == 3) || (zHD == 5) || (zHD == 7) || (zHD == 9) || (zHD == 11) || (zHD == 13)) {
				iXnN1 = j - (i >> 1) - 2;
				iX = j - (i >> 1) - 1;
				iXn = j - (i >> 1);
				iXnP2 = j - (i >> 1);
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (zHD == -1) {
				iXnN1 = 0;
				iX = -1;
				iXn = -2;
				iXnP2 = -2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else {
				iXnN1 = i - 2 * j - 1;
				iX = i - 2 * j - 2;
				iXn = i - 2 * j - 3;
				iXnP2 = i - 2 * j - 3;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 7) {
			if ((j == 0) || (j == 2) || (j == 4) || (j == 6)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 1;
				iXnP2 = i + (j >> 1) + 1;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((j == 1) || (j == 3) || (j == 5) || (j == 7)) {
				iXnN1 = i + (j >> 1);
				iX = i + (j >> 1) + 1;
				iXn = i + (j >> 1) + 2;
				iXnP2 = i + (j >> 1) + 2;
				int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
		}

		if (uiDirMode == 8) {
			int zHU = 2 * j + i;
			if ((zHU == 0) || (zHU == 2) || (zHU == 4) || (zHU == 6) || (zHU == 8) || (zHU == 10) || (zHU == 12)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 1;
				iXnP2 = j + (i >> 1) + 1;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + lineRefer16[iX] + 1) >> 1;
			}
			else if ((zHU == 1) || (zHU == 3) || (zHU == 5) || (zHU == 7) || (zHU == 9) || (zHU == 11)) {
				iXnN1 = j + (i >> 1);
				iX = j + (i >> 1) + 1;
				iXn = j + (i >> 1) + 2;
				iXnP2 = j + (i >> 1) + 2;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 2 * lineRefer16[iX] + lineRefer16[iXn] + 2) >> 2;
			}
			else if (zHU == 13) {
				iXnN1 = 6;
				iX = 7;
				iXn = 7;
				iXnP2 = 7;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = (lineRefer16[iXnN1] + 3 * lineRefer16[iX] + 2) >> 2;
			}
			else {
				iXnN1 = 7;
				iX = 7;
				iXn = 7;
				iXnP2 = 7;
				int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
				h264_dst[j][i] = lineRefer16[iXnN1];
			}
			saveMaxMinIndex(iXnN1, iX, iXn, iXnP2);
		}
	}
	if (iHeight == 16 && iWidth == 16) {
		if (uiDirMode == 0) {
			iXnN1 = i;
			iX = i;
			iXn = i;
			iXnP2 = i;
			int linerefIndexMain = convertAboveSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}
		if (uiDirMode == 1) {
			iXnN1 = j;
			iX = j;
			iXn = j;
			iXnP2 = j;
			int linerefIndexMain = convertLeftSrcIndex2RefIndex(&iXnN1, &iX, &iXn, &iXnP2);
			h264_dst[j][i] = lineRefer16[iX];
		}
	}
}

int H264PredicterHW::convertAboveSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2) {
	int secondBouds = second_bouds[0];
	*iYnN1 = -*iYnN1 - 2 + CENTRE_OFFSET_H264 - secondBouds;
	*iY = -*iY - 2 + CENTRE_OFFSET_H264 - secondBouds;
	*iYn = -*iYn - 2 + CENTRE_OFFSET_H264 - secondBouds;
	*iYnP2 = -*iYnP2 - 2 + CENTRE_OFFSET_H264 - secondBouds;
	return 0;
}


int H264PredicterHW::convertLeftSrcIndex2RefIndex(int* iYnN1, int* iY, int* iYn, int* iYnP2) {
	int secondBouds = second_bouds[0];
	*iYnN1 = *iYnN1 + CENTRE_OFFSET_H264 - secondBouds;
	*iY = *iY + CENTRE_OFFSET_H264 - secondBouds;
	*iYn = *iYn + CENTRE_OFFSET_H264 - secondBouds;
	*iYnP2 = *iYnP2 + CENTRE_OFFSET_H264 - secondBouds;
	return 0;
}

int H264PredicterHW::convertAboveSrcIndex2RefIndex(int index) {
	
	int indexOut = -index -  2 + CENTRE_OFFSET_H264;
	return indexOut;
}
int H264PredicterHW::convertLeftSrcIndex2RefIndex(int index) {
	int indexOut = index + CENTRE_OFFSET_H264;
	return indexOut;
}
void H264PredicterHW::generateRefer() {
	int  Above[2 * MAX_CU_SIZE_H264 + 1];
	int  Left[MAX_CU_SIZE_H264 + 1];
	int *refLeft = Left + 1;
	int *refAbove = Above + 1;
	convertSrc(Above, Left);
	for (int i = 0; i < 2 * MAX_CU_SIZE_H264 ; i++) {
		int lineRefIndex = convertAboveSrcIndex2RefIndex(i);
		lineRefer256[lineRefIndex] = refAbove[i];
	}
	for (int i = 0; i <  MAX_CU_SIZE_H264; i++) {
		int lineRefIndex = convertLeftSrcIndex2RefIndex(i);
		lineRefer256[lineRefIndex] = refLeft[i];
	}
	lineRefer256[CENTRE_OFFSET_H264 - 1] = Above[0];// lineRefer[128] = refLeft[-1] = refAbove[-1]
}

void H264PredicterHW::get4X4MaxMinIndex(int i, int j, int x_offset, int y_offset) {

	int x = i * x_offset;
	int x4 = x_offset * (i + 1) - 1;
	int y = j * y_offset;
	int y4 = y_offset * (j + 1) - 1;
	save_index = 0;
	predPixelIndex(y, x);
	save_index++;
	predPixelIndex(y4, x);
	save_index++;
	predPixelIndex(y, x4);
	save_index++;
	predPixelIndex(y4, x4);
	int max_index_number = save_index + 1;
	max_index = calcMax(max_min_indexs, NUM_DISTANCE_SIZE_264, max_index_number);
	min_index = calcMin(max_min_indexs, NUM_DISTANCE_SIZE_264, max_index_number);
}


void H264PredicterHW::saveMaxMinIndex(int  iYnN1, int iY, int iYn, int iYnP2)
{
	max_min_indexs[save_index][0] = iYnN1 + CENTRE_OFFSET_H264;
	max_min_indexs[save_index][1] = iY + CENTRE_OFFSET_H264;
	max_min_indexs[save_index][2] = iYn + CENTRE_OFFSET_H264;
	max_min_indexs[save_index][3] = iYnP2 + CENTRE_OFFSET_H264;
}

void H264PredicterHW::convertSrc(int* above, int *left) {
	if ((src_data && src_data->h264_src)) {
		int src_stride = MAX_CU_SIZE_H264 * NUM_EXTEND_SIZE_H264 + 1;
		int max_width = MAX_CU_SIZE_H264 * NUM_EXTEND_SIZE_H264 + 1;
		for (int i = 0; i < max_width; i++) {
			above[i] = src_data->h264_src[i];
		}

		for (int i = 0; i <= MAX_CU_SIZE_H264; i++) {
			left[i] = src_data->h264_src[i*src_stride];
		}
	}
}


void H264PredicterHW::initDstData() {
	h264_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		h264_dst[i] = new int[tu_width]();
	}
}

void H264PredicterHW::deinitDstData() {
	if (h264_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] h264_dst[i];
		}
		delete[] h264_dst;
		h264_dst = NULL;
	}
}