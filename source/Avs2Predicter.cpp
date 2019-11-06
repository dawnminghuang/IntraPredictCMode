#include "Avs2Predicter.h"


AVS2Predicter::AVS2Predicter() {
}

AVS2Predicter::~AVS2Predicter() {

}

void AVS2Predicter::predict() {
	int mode_number = NUM_INTRA_PMODE_AVS;
	int max_cu_size = 64;
	generateOutPath(AVS2_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_MODE_INTRA_AVS; i++) {
		int uiDirMode = g_prdict_mode_avs[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		for (int j = 0; j < NUM_CU_PMODE_AVS; j++) {
			int iWidth = g_cu_size_avs[j][0];
			int iHeight = g_cu_size_avs[j][1];
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_AVS);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}

void AVS2Predicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int  iDx, iDy, i, j, iTempDx, iTempDy, iXx, iXy, iYx, iYy;
	int  uixyflag = 0; // 0 for x axis 1 for y axis
	int offset, offsetx, offsety;
	int iX, iY, iXn, iYn, iXnN1, iYnN1, iXnP2, iYnP2;
	int iDxy;
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int iWidth2 = iWidth << 1;
	int iHeight2 = iHeight << 1;
	iDx = g_aucDirDx[uiDirMode];
	iDy = g_aucDirDy[uiDirMode];
	uixyflag = g_aucXYflg[uiDirMode];
	iDxy = g_aucSign[uiDirMode];

	for (j = 0; j < iHeight; j++) {
		for (i = 0; i < iWidth; i++) {
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
				saveDistanceMatri(distanMatri, i, j, iXnN1, iX, iXn, iXnP2, 0);

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
				saveDistanceMatri(distanMatri, i, j, iYnN1, iY, iYn, iYnP2, -1);
			}
		}
	}


}

int AVS2Predicter::getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset)
{
	int imult = g_aucDirDxDy[uiXYflag][uiDirMode][0];
	int ishift = g_aucDirDxDy[uiXYflag][uiDirMode][1];
	int iTempDn = iTempD * imult >> ishift;
	*offset = ((iTempD * imult * 32) >> ishift) - iTempDn * 32;
	return iTempDn;
}



void AVS2Predicter::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX)
{
	if (iX == -1) {
		iYnN1 = -iYnN1;
		iY = -iY;
		iYn = -iYn;
		iYnP2 = -iYnP2;

	}
	distanMatri->distance_matri[j][i][0] = iYnN1;
	distanMatri->distance_matri[j][i][1] = iY;
	distanMatri->distance_matri[j][i][2] = iYn;
	distanMatri->distance_matri[j][i][3] = iYnP2;
}


