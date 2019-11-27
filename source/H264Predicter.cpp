#include "H264Predicter.h"


H264Predicter::H264Predicter() {
	max_dst_number = (MAX_CU_SIZE_H264 *NUM_EXTEND_SIZE_H264 + 1)* (MAX_CU_SIZE_H264);
	h264_dst = NULL;
	tu_width = MAX_CU_SIZE_H264;
	tu_height =MAX_CU_SIZE_H264;

}

H264Predicter::~H264Predicter() {

}

void H264Predicter::predict() {
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
			for (int k = 0; k < NUM_COLOR_SPACE_SIZE; k++) {
				if (k == COLOR_SPACE_LUMA) {
					int iWidth = g_cu_size_264[j][0];
					int iHeight = g_cu_size_264[j][1];
					tu_width = iWidth;
					tu_height = iHeight;
					initDstData();
					DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_264);
					predIntraLumaAdi(distanMatri, uiDirMode);
					outPutWriter->writeModeInfoToFile(distanMatri);
					distanceCalculator->calcuDistance(distanMatri);
					outPutWriter->writeDstDataToFile(h264_dst, tu_width, tu_height);
					deinitDstData();
					delete distanMatri;
				}
				else {
					int iWidth = g_cu_size_264[j][0]/2;
					int iHeight = g_cu_size_264[j][1]/2;
					tu_width = iWidth;
					tu_height = iHeight;
					initDstData();
					DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_264);
					predIntraChromaAdi(distanMatri, uiDirMode);
					outPutWriter->writeDstDataToFile(h264_dst, tu_width, tu_height);
					deinitDstData();
					delete distanMatri;

				}
			}
		}
	}
	writeMaxDistanceToFile(calc_mode);
}
void H264Predicter::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	predIntraAngAdi(distanMatri, uiDirMode);
}
void H264Predicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_H264 + 1];
	int  Left[MAX_CU_SIZE_H264 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above + 1;
	int *refLeft = Left + 1;
	int iXnN1, iX, iXn, iXnP2;
	if ((iWidth == 4) && (iHeight == 4)) {
		if (uiDirMode == 0) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refAbove[i];
				}
			}
		}
		if (uiDirMode == 1) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refLeft[i];
				}
			}
		}
		if (uiDirMode == 2) {
			int  sumPixel = 0;			
				for (int i = 0; i < iWidth; i++) {
					sumPixel += refAbove[i];
				}
				for (int j = 0; j < iHeight; j++) {
					sumPixel += refLeft[j];
			    }
				for (int j = 0; j < iHeight; j++) {
					for (int i = 0; i < iWidth; i++) {
						h264_dst[j][i] = (sumPixel + 4) >> 3;
					}
				}

		}
		if (uiDirMode == 3) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if ((i == 3) && (j == 3)) {
						iXnN1 = 6;
						iX = 6;
						iXn = 7;
						iXnP2 = 7;
						h264_dst[j][i] = (refAbove[iX] + 3* refAbove[iXn] + 2) >> 2;
					}
					else {
						iXnN1 = i + j;
						iX = i + j + 1;
						iXn = i + j + 2;
						iXnP2 = i + j + 2;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
					}
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 4) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if (i > j) {
						iXnN1 = i - j - 2;
						iX = i - j - 1;
						iXn = i - j;
						iXnP2 = i - j;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if (i < j) {
						iXnN1 = j - i - 2;
						iX = j - i - 1;
						iXn = j - i;
						iXnP2 = j - i;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					else {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 5) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int zVR = 2 * i - j;
					if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6)) {
						iXnN1 = i - (j >> 1) - 1;
						iX = i - (j >> 1);
						iXn = i - (j >> 1);
						iXnP2 = i - (j >> 1);
						h264_dst[j][i] = (refAbove[iXnN1] + refAbove[iX] + 1) >> 1;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if ((zVR == 1) || (zVR == 3) || (zVR == 5)) {
						iXnN1 = i - (j >> 1) - 2;
						iX = i - (j >> 1) - 1;
						iXn = i - (j >> 1);
						iXnP2 = i - (j >> 1);
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if (zVR == -1) {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					else {
						iXnN1 = j - 1;
						iX = j - 2;
						iXn = j - 3;
						iXnP2 = j - 3;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 6) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int zHD = 2 * j - i;
					if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6)) {
						iXnN1 = j - (i >> 1) - 1;
						iX = j - (i >> 1);
						iXn = j - (i >> 1);
						iXnP2 = j - (i >> 1);
						h264_dst[j][i] = (refLeft[iXnN1] + refLeft[iX] + 1) >> 1;
					}
					else if ((zHD == 1) || (zHD == 3) || (zHD == 5)) {
						iXnN1 = j - (i >> 1) - 2;
						iX = j - (i >> 1) - 1;
						iXn = j - (i >> 1);
						iXnP2 = j - (i >> 1);
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;

					}
					else if (zHD == -1) {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					else {
						iXnN1 = i - 1;
						iX = i - 2;
						iXn = i - 3;
						iXnP2 = i - 3;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 7) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if ((j == 0) || (j == 2)) {
						iXnN1 = i + (j >> 1);
						iX = i + (j >> 1) + 1;
						iXn = i + (j >> 1) + 1;
						iXnP2 = i + (j >> 1) + 1;
						h264_dst[j][i] = (refAbove[iXnN1] + refAbove[iX] + 1) >> 1;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if ((j == 1) || (j == 3)) {
						iXnN1 = i + (j >> 1);
						iX = i + (j >> 1) + 1;
						iXn = i + (j >> 1) + 2;
						iXnP2 = i + (j >> 1) + 2;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 8) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int  zHU = 2 * j + i;
					if ((zHU == 0) || (zHU == 2) || (zHU == 4)) {
						iXnN1 = j + (i >> 1);
						iX = j + (i >> 1) + 1;
						iXn = j + (i >> 1) + 1;
						iXnP2 = j + (i >> 1) + 1;
						h264_dst[j][i] = (refLeft[iXnN1] + refLeft[iX] + 1) >> 1;
					}
					else if ((zHU == 1) || (zHU == 3)) {
						iXnN1 = j + (i >> 1);
						iX = j + (i >> 1) + 1;
						iXn = j + (i >> 1) + 2;
						iXnP2 = j + (i >> 1) + 2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					else if (zHU == 5) {
						iXnN1 = 2;
						iX = 3;
						iXn = 3;
						iXnP2 = 3;
						h264_dst[j][i] = (refLeft[iXnN1] + 3 * refLeft[iX] + 2) >> 2;
					}
					else {
						iXnN1 = 3;
						iX = 3;
						iXn = 3;
						iXnP2 = 3;
						h264_dst[j][i] = refLeft[iXnN1];

					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}
	}
	if (iHeight == 8 && iWidth == 8) {
		if (uiDirMode == 0) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refAbove[i];
				}
			}
		}
		if (uiDirMode == 1) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refLeft[i];
				}
			}
		}
		if (uiDirMode == 2) {
			int  sumPixel = 0;
			for (int i = 0; i < iWidth; i++) {
				sumPixel += refAbove[i];
			}
			for (int j = 0; j < iHeight; j++) {
				sumPixel += refLeft[j];
			}
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = (sumPixel + 8) >> 4;
				}
			}

		}
		if (uiDirMode == 3) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if ((i == 7) && (j == 7)) {
						iXnN1 = 14;
						iX = 14;
						iXn = 15;
						iXnP2 = 15;
						h264_dst[j][i] = (refAbove[iX] + 3*refAbove[iXn] + 2) >> 2;
					}
					else {
						iXnN1 = i + j;
						iX = i + j + 1;
						iXn = i + j + 2;
						iXnP2 = i + j + 2;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
					}
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 4) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if (i > j) {
						iXnN1 = i - j - 2;
						iX = i - j - 1;
						iXn = i - j;
						iXnP2 = i - j;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if (i < j) {
						iXnN1 = j - i - 2;
						iX = j - i - 1;
						iXn = j - i;
						iXnP2 = j - i;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					else {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}
		if (uiDirMode == 5) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int zVR = 2 * i - j;
					if ((zVR == 0) || (zVR == 2) || (zVR == 4) || (zVR == 6) || (zVR == 8) || (zVR == 10) || (zVR == 12) || (zVR == 14)) {
						iXnN1 = i - (j >> 1) - 1;
						iX = i - (j >> 1);
						iXn = i - (j >> 1);
						iXnP2 = i - (j >> 1);
						h264_dst[j][i] = (refAbove[iXnN1] + refAbove[iX] + 1) >> 1;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if ((zVR == 1) || (zVR == 3) || (zVR == 5) || (zVR == 7) || (zVR == 9) || (zVR == 11) || (zVR == 13)) {
						iXnN1 = i - (j / 2) - 2;
						iX = i - (j / 2) - 1;
						iXn = i - (j / 2);
						iXnP2 = i - (j / 2);
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					else if (zVR == -1) {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					else {
						iXnN1 = j - 2 * i - 1;
						iX = j - 2 * i - 2;
						iXn = j - 2 * i - 3;
						iXnP2 = j - 2 * i - 3;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}
		if (uiDirMode == 6) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int zHD = 2 * j - i;
					if ((zHD == 0) || (zHD == 2) || (zHD == 4) || (zHD == 6) || (zHD == 8) || (zHD == 10) || (zHD == 12) || (zHD == 14)) {
						iXnN1 = j - (i >> 1) - 1;
						iX = j - (i >> 1);
						iXn = j - (i >> 1);
						iXnP2 = j - (i >> 1);
						h264_dst[j][i] = (refLeft[iXnN1] + refLeft[iX] + 1) >> 1;
					}
					else if ((zHD == 1) || (zHD == 3) || (zHD == 5) || (zHD == 7) || (zHD == 9) || (zHD == 11) || (zHD == 13)) {
						iXnN1 = j - (i >> 1) - 2;
						iX = j - (i >> 1) - 1;
						iXn = j - (i >> 1);
						iXnP2 = j - (i >> 1);
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					else if (zHD == -1) {
						iXnN1 = 0;
						iX = -1;
						iXn = -2;
						iXnP2 = -2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refAbove[iXnN1] + 2) >> 2;
					}
					else {
						iXnN1 = i - 2 * j - 1;
						iX = i - 2 * j - 2;
						iXn = i - 2 * j - 3;
						iXnP2 = i - 2 * j - 3;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
						convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 7) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if ((j == 0) || (j == 2) || (j == 4) || (j == 6)) {
						iXnN1 = i + (j >> 1);
						iX = i + (j >> 1) + 1;
						iXn = i + (j >> 1) + 1;
						iXnP2 = i + (j >> 1) + 1;
						h264_dst[j][i] = (refAbove[iXnN1] + refAbove[iX] + 1) >> 1;
					}
					else if ((j == 1) || (j == 3) || (j == 5) || (j == 7)) {
						iXnN1 = i + (j >> 1);
						iX = i + (j >> 1) + 1;
						iXn = i + (j >> 1) + 2;
						iXnP2 = i + (j >> 1) + 2;
						h264_dst[j][i] = (refAbove[iXnN1] + 2 * refAbove[iX] + refAbove[iXn] + 2) >> 2;
					}
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}

		if (uiDirMode == 8) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					int zHU = 2 * j + i;
					if ((zHU == 0) || (zHU == 2) || (zHU == 4) || (zHU == 6) || (zHU == 8) || (zHU == 10) || (zHU == 12)) {
						iXnN1 = j + (i >> 1);
						iX = j + (i >> 1) + 1;
						iXn = j + (i >> 1) + 1;
						iXnP2 = j + (i >> 1) + 1;
						h264_dst[j][i] = (refLeft[iXnN1] + refLeft[iX] + 1) >> 1;
					}
					else if ((zHU == 1) || (zHU == 3) || (zHU == 5) || (zHU == 7) || (zHU == 9) || (zHU == 11)) {
						iXnN1 = j + (i >> 1);
						iX = j + (i >> 1) + 1;
						iXn = j + (i >> 1) + 2;
						iXnP2 = j + (i >> 1) + 2;
						h264_dst[j][i] = (refLeft[iXnN1] + 2 * refLeft[iX] + refLeft[iXn] + 2) >> 2;
					}
					else if (zHU == 13) {
						iXnN1 = 6;
						iX = 7;
						iXn = 7;
						iXnP2 = 7;
						h264_dst[j][i] = (refLeft[iXnN1] + 3 * refLeft[iX] + 2) >> 2;
					}
					else {
						iXnN1 = 7;
						iX = 7;
						iXn = 7;
						iXnP2 = 7;
						h264_dst[j][i] = refLeft[iXnN1];
					}
					saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
				}
			}
		}
	}
	if (iHeight == 16 && iWidth == 16) {
		if (uiDirMode == 0) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refAbove[i];
				}
			}
		}
		if (uiDirMode == 1) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refLeft[i];
				}
			}
		}
		if (uiDirMode == 2) {
			int  sumPixel = 0;
			for (int i = 0; i < iWidth; i++) {
				sumPixel += refAbove[i];
			}
			for (int j = 0; j < iHeight; j++) {
				sumPixel += refLeft[j];
			}
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = (sumPixel + 16) >> 5;
				}
			}

		}
		if (uiDirMode == 3) {
			int  H = 0;
			int  V = 0;
			int a, b, c;
			for (int i = 0; i < iWidth/2; i++) {
				H +=(i+1)*(refAbove[8+i] - refAbove[6-i]);
			}
			for (int j = 0; j < iHeight/2; j++) {
				V += (j+1)*refLeft[8+j]-refLeft[6-j];
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
}

void H264Predicter::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode) {
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
			int  sumPixel = 0;
			for (int i = 0; i < iWidth; i++) {
				sumPixel += refAbove[i];
			}
			for (int j = 0; j < iHeight; j++) {
				sumPixel += refLeft[j];
			}
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					if ((iWidth == 8) && (iHeight == 8)) {
						h264_dst[j][i] = (sumPixel + 8) >> 4;
					}else{
						h264_dst[j][i] = (sumPixel + 4) >> 3;
					}
				}
			}

		}
		if (uiDirMode == 1) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refAbove[i];
				}
			}
		}
		if (uiDirMode == 3) {
			for (int j = 0; j < iHeight; j++) {
				for (int i = 0; i < iWidth; i++) {
					h264_dst[j][i] = refLeft[i];
				}
			}
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
void H264Predicter::convertSrc(int* above, int *left) {
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

void H264Predicter::convertAbovePoints(int* iYnN1, int* iY, int* iYn, int* iYnP2)
{
	if(*iYnN1 !=-1 ){
		*iYnN1 = -*iYnN1 - 2;
	}
	if(*iY != -1){
		*iY = -*iY - 2;
	}
	if(*iYn != -1){
	   *iYn = -*iYn - 2;
	}
	if(*iYnP2 != -1){
	    *iYnP2 = -*iYnP2 - 2;
	}

}

void H264Predicter::initDstData() {
	h264_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		h264_dst[i] = new int[tu_width]();
	}
}

void H264Predicter::deinitDstData() {
	if (h264_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete [] h264_dst[i];
		}
		delete[] h264_dst;
		h264_dst = NULL;
	}
}


