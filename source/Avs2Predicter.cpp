#include "Avs2Predicter.h"


AVS2Predicter::AVS2Predicter() {
	avs_dst = NULL;
	tu_width = 0;
	tu_height = 0;
}

AVS2Predicter::~AVS2Predicter() {

}

void AVS2Predicter::predict() {
	int mode_number = NUM_INTRA_PMODE_AVS;
	int max_cu_size = 64;
	generateOutPath(AVS2_PATH, calc_mode);
	generateDigOutPath(AVS2_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_MODE_INTRA_AVS; i++) {
		int uiDirMode = g_prdict_mode_avs[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);// position info
		outPutWriter->initDstDataFp(AVS2_DATA_PATH, uiDirMode);// predict pixel
		outPutWriter->initDigPostionInfoFp(digOutPath, uiDirMode);//is in diag info
		distanceCalculator->setPredictMode(uiDirMode);
		for (int j = 0; j < NUM_CU_PMODE_AVS; j++) {
			tu_width = g_cu_size_avs[j][0];
			tu_height = g_cu_size_avs[j][1];
			initDstData();
			DistanceData* distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
			predIntraAngAdi(distanMatri, uiDirMode);
			outPutWriter->writeModeInfoToFile(distanMatri);
			outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
			distanceCalculator->calcuDistance(distanMatri);
			deinitDstData();
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}

void AVS2Predicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int  *pSrc = NULL;
	if(src_data && src_data->avs2_src){
		pSrc = src_data->avs2_src + ((1 << CU_SIZ_LOG_AVS2) * 2);
	}
	int  *rpSrc = pSrc;
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
				rpSrc = pSrc + 1;
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
				if (rpSrc) {
					avs_dst[j][i] = (rpSrc[iXnN1] * (32 - offset) + rpSrc[iX] * (64 - offset) +
						rpSrc[iXn] * (32 + offset) + rpSrc[iXnP2] * offset + 64) >> 7;
				}
				saveDistanceMatri(distanMatri, i, j, iXnN1, iX, iXn, iXnP2, 0);

			}
			else if (iX == -1) {
				rpSrc = pSrc - 1;
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
				if (rpSrc) {
					avs_dst[j][i] = (rpSrc[-iYnN1] * (32 - offset) + rpSrc[-iY] * (64 - offset) +
						rpSrc[-iYn] * (32 + offset) + rpSrc[-iYnP2] * offset + 64) >> 7;
				}

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
		iYnN1 = -iYnN1-1;
		iY = -iY-1;
		iYn = -iYn-1;
		iYnP2 = -iYnP2-1;
     }else {
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

void AVS2Predicter::initDstData() {
	avs_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		avs_dst[i] = new int[tu_width]();
	}
}

void AVS2Predicter::deinitDstData() {
	if (avs_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] avs_dst[i];
		}
		delete[] avs_dst;
		avs_dst = NULL;
	}
}
