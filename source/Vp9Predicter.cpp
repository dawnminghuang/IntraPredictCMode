#include "Vp9Predicter.h"


Vp9Predicter::Vp9Predicter() {
	point_number = 0;
	bs = 0;
}

Vp9Predicter::~Vp9Predicter() {

}
void Vp9Predicter::initVp9Matri(int width, int height, int distanceSize) {
	bs = width;
	point_number = width * height;
	vp9_Matri = new int *[point_number];
	for (int i = 0; i < point_number; i++) {
		vp9_Matri[i] = new int[distanceSize + 1]();
	}

}

void Vp9Predicter::deinitVp9Matri() {
	for (int i = 0; i < point_number; i++) {
		delete[] vp9_Matri[i];
	}
	delete[] vp9_Matri;
}

void Vp9Predicter::predict() {
	int mode_number = NUM_INTRA_PMODE_VP9 + START_INDEX_VP9;
	int max_cu_size = 64;
	generateOutPath(VP9_PATH, calc_mode);
	outPutWriter->initDistanceInfoFp(outPath, "Matri");
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_VP9; i++) {
		int uiDirMode = g_prdict_mode_vp9[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		outPutWriter->initDstDataFp(VP9_DATA_PATH, uiDirMode);
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
			outPutWriter->writeDstDataToFile(vp9_dst, iWidth, iHeight);
			deinitDstData();
			deinitVp9Matri();
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}

void Vp9Predicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int  Above[2 * MAX_CU_SIZE_VP9 + 1];
	int  Left[MAX_CU_SIZE_VP9 + 1];
	convertSrc(Above, Left);
	int *refAbove = Above + 1;
	int *refLeft = Left + 1;
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
					iXn = j + 1;
					iXnP2 = j + 1;
					vp9_dst[j][i] = AVG2(refLeft[iXnN1], refLeft[iXn]);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((i == 1) && (j != iHeight - 1) && (j != iHeight - 2)) {
					iXnN1 = j;
					iX = j + 1;
					iXn = j + 2;
					iXnP2 = j + 2;
					vp9_dst[j][i] = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((i == 1) && (j == iHeight - 2)) {
					iXnN1 = bs - 1;
					iX = bs - 2;
					iXn = bs - 2;
					iXnP2 = bs - 2;
					vp9_dst[j][i] = (refLeft[iX] + refLeft[iXnN1] * 3 + 2) >> 2;
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if (j == iHeight - 1) {
					iXnN1 = bs - 1;
					iX = bs - 1;
					iXn = bs - 1;
					iXnP2 = bs - 1;
					vp9_dst[j][i] = refLeft[iX];
					saveVp9Matri(vp9_Matri, (bs - 1)*bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
				vp9_dst[j][i] = predictPixel;
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
				}
				else {
					iXnN1 = (j / 2) + i;
					iX = (j / 2) + i + 1;
					iXn = (j / 2) + i + 1;
					iXnP2 = (j / 2) + i + 1;
					vp9_dst[j][i] = AVG2(refAbove[iXnN1], refAbove[iX]);
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
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
				}
				else {
					iXnN1 = bs * 2 - 1;
					iX = bs * 2 - 1;
					iXn = bs * 2 - 1;
					iXnP2 = bs * 2 - 1;
					vp9_dst[j][i] = refAbove[iX];
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
					iXn = i;
					iXnP2 = i;
					vp9_dst[j][i] = AVG2(refAbove[iXnN1], refAbove[iX]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 1) && (i != 0)) {
					iXnN1 = i - 2;
					iX = i - 1;
					iXn = i;
					iXnP2 = i;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 1) && (i == 0)) {
					iXnN1 = -2;
					iX = -1;
					iXn = 0;
					iXnP2 = 0;
					vp9_dst[j][i] = AVG3(refLeft[iXn], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 2) && (i == 0)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, 2 * bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if (i == 0) {
					iXnN1 = j - 3;
					iX = j - 2;
					iXn = j - 1;
					iXnP2 = j - 1;
					vp9_dst[j][i] = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
				vp9_dst[j][i] = predictPixel;
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
					vp9_dst[j][i] = AVG3(refLeft[iXnN1 + 2], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, 0, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 0) && (i != 0)) {
					iXnN1 = i - 2;
					iX = i - 1;
					iXn = i;
					iXnP2 = i;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 1) && (i == 0)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j >= 2) && (i == 0)) {
					iXnN1 = j - 2;
					iX = j - 1;
					iXn = j;
					iXnP2 = j;
					vp9_dst[j][i] = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
				vp9_dst[j][i] = predictPixel;
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
					iXn = -2;
					iXnP2 = -2;
					vp9_dst[j][i] = AVG2(refAbove[iXnN1], refLeft[iX + 2]);
					saveVp9Matri(vp9_Matri, 0, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j != 0) && (i == 0)) {
					iXnN1 = j - 1;
					iX = j;
					iXn = j;
					iXnP2 = j;
					vp9_dst[j][i] = AVG2(refLeft[iXnN1], refLeft[iX]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 0) && (i == 1)) {
					iXnN1 = -2;
					iX = -1;
					iXn = 0;
					iXnP2 = 0;
					vp9_dst[j][i] = AVG3(refLeft[iXn], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j == 1) && (i == 1)) {
					iXnN1 = -1;
					iX = -2;
					iXn = -3;
					iXnP2 = -3;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refLeft[iX + 2], refLeft[iXnN1 + 2]);
					saveVp9Matri(vp9_Matri, bs + 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((j >= 2) && (i == 1)) {
					iXnN1 = j - 2;
					iX = j - 1;
					iXn = j;
					iXnP2 = j;
					vp9_dst[j][i] = AVG3(refLeft[iXnN1], refLeft[iX], refLeft[iXn]);
					convertXPoints(&iXnN1, &iX, &iXn, &iXnP2);
					saveVp9Matri(vp9_Matri, j*bs + 1, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				}
				else if ((i >= 2) && (j == 0)) {
					iXnN1 = i - 3;
					iX = i - 2;
					iXn = i - 1;
					iXnP2 = i - 1;
					vp9_dst[j][i] = AVG3(refAbove[iXnN1], refAbove[iX], refAbove[iXn]);
					saveVp9Matri(vp9_Matri, i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
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
				vp9_dst[j][i] = predictPixel;
				saveVp9Matri(vp9_Matri, j*bs + i, iXnN1, iX, iXn, iXnP2, vp9_dst[j][i]);
				saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
			}
		}
	}
}


void Vp9Predicter::saveVp9Matri(int ** vp9Matri, int index, int iYnN1, int iY, int iYn, int iYnP2, int predictData)
{
	vp9Matri[index][0] = iYnN1;
	vp9Matri[index][1] = iY;
	vp9Matri[index][2] = iYn;
	vp9Matri[index][3] = iYnP2;
	vp9Matri[index][4] = predictData;
}

void Vp9Predicter::convertSrc(int* above, int *left) {
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


void Vp9Predicter::initDstData() {
	vp9_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		vp9_dst[i] = new int[tu_width]();
	}
}

void Vp9Predicter::deinitDstData() {
	if (vp9_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] vp9_dst[i];
		}
		delete[] vp9_dst;
	}
}



