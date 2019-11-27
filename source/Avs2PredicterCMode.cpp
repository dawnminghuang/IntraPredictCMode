#include "AVS2PredicterCMode.h"


AVS2PredicterCMode::AVS2PredicterCMode() {
	avs_dst = NULL;
	tu_width = 0;
	tu_height = 0;
	pSrc = NULL;
	sample_bit_depth = 8;
	bLeftAvail = 1;
	bAboveAvail = 1;
	initLog2size();
	cu_size_log = CU_SIZ_LOG_AVS2;
}

AVS2PredicterCMode::~AVS2PredicterCMode() {

}

void AVS2PredicterCMode::predict() {
	int mode_number = NUM_INTRA_PMODE_AVS;
	int max_cu_size = 64;
	int bAbove = 1;
	int bLeft = 1;


	generateOutPath(AVS2_PATH, calc_mode);
	for (int i = 0; i < NUM_MODE_INTRA_AVS; i++) {
		int uiDirMode = g_prdict_mode_avs[i];
		outPutWriter->initDstDataFp(AVS2_DATA_PATH_CMODE, uiDirMode);
		for (int j = 0; j < NUM_CU_PMODE_AVS; j++) {

			for (int k = 0; k < NUM_COLOR_SPACE_SIZE; k++) {
				if (k == COLOR_SPACE_LUMA) {
					tu_width = g_cu_size_avs[j][0];
					tu_height = g_cu_size_avs[j][1];
					initDstData();
					predIntraLumaAdi(NULL, uiDirMode);
					outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
					deinitDstData();
				}else {
					tu_width = g_cu_size_avs[j][0]/2;
					tu_height = g_cu_size_avs[j][1]/2;
					initDstData();
					predIntraChromaAdi(NULL, uiDirMode);
					outPutWriter->writeDstDataToFile(avs_dst, tu_width, tu_height);
					deinitDstData();
				}

			}
		}
	}
}


// This Function is used for outside compare process used.
void AVS2PredicterCMode::predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode) {
	int bAbove = 1;
	int bLeft = 1;
	switch (uiDirMode) {
	case VERT_PRED_AVS2:   // Vertical
		xPredIntraVertAdi(pSrc, avs_dst, tu_width, tu_height);
		break;
	case HOR_PRED_AVS2:    // Horizontal
		xPredIntraHorAdi(pSrc, avs_dst, tu_width, tu_height);
		break;
	case DC_PRED_AVS2:     // DC
		xPredIntraDCAdi(pSrc, avs_dst, tu_width, tu_height, bAbove, bLeft, sample_bit_depth);
		break;
	case PLANE_PRED_AVS2:  // Plane
		xPredIntraPlaneAdi(pSrc, avs_dst, tu_width, tu_height, sample_bit_depth);
		break;
	case BI_PRED_AVS2:     // bi
		xPredIntraBiAdi(pSrc, avs_dst, tu_width, tu_height, sample_bit_depth);
		break;
	default:
		xPredIntraAngAdi(pSrc, avs_dst, uiDirMode, tu_width, tu_height);
		break;
	}
}

void AVS2PredicterCMode::predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode)
{
	int bAbove = 1;
	int bLeft = 1;
	int chromeWidth = tu_width;
	int chromeHeight = tu_height;
	switch (uiDirMode) {
	case VERT_PRED_AVS2:   // Vertical
		xPredIntraVertAdi(pSrc, avs_dst, chromeWidth, chromeHeight);
		break;
	case HOR_PRED_AVS2:    // Horizontal
		xPredIntraHorAdi(pSrc, avs_dst, chromeWidth, chromeHeight);
		break;
	case DC_PRED_AVS2:     // DC
		xPredIntraDCAdi(pSrc, avs_dst, chromeWidth, chromeHeight, bAbove, bLeft, sample_bit_depth);
		break;
	case PLANE_PRED_AVS2:  // Plane
		xPredIntraPlaneAdi(pSrc, avs_dst, chromeWidth, chromeHeight, sample_bit_depth);
		break;
	case BI_PRED_AVS2:     // bi
		xPredIntraBiAdi(pSrc, avs_dst, chromeWidth, chromeHeight, sample_bit_depth);
		break;
	default:
		xPredIntraAngAdi(pSrc, avs_dst, uiDirMode, chromeWidth, chromeHeight);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
/// function definition
/////////////////////////////////////////////////////////////////////////////
void AVS2PredicterCMode::xPredIntraVertAdi(int *pSrc, int **pDst, int iWidth, int iHeight)
{
	int x, y;
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	int *rpSrc = pSrc + 1;

	for (y = 0; y < iHeight; y++) {
		for (x = 0; x < iWidth; x++) {
			pDst[y][x] = rpSrc[x];
		}
	}
}

void AVS2PredicterCMode::xPredIntraHorAdi(int *pSrc, int **pDst, int iWidth, int iHeight)
{
	int x, y;
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	int *rpSrc = pSrc - 1;

	for (y = 0; y < iHeight; y++) {
		for (x = 0; x < iWidth; x++) {
			pDst[y][x] = rpSrc[-y];
		}
	}
}
void AVS2PredicterCMode::xPredIntraDCAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail,
	int sample_bit_depth)
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
			pDst[y][x] = iDCValue;
		}
	}
}

void AVS2PredicterCMode::xPredIntraPlaneAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int sample_bit_depth)
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

void AVS2PredicterCMode::xPredIntraBiAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int sample_bit_depth)
{
	int x, y;
	int ishift_x = g_log2size[iWidth];
	int ishift_y = g_log2size[iHeight];
	int ishift = min(ishift_x, ishift_y);
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a, b, c, w, wxy, tmp;
	int predx;
	int pTop[MAX_CU_SIZE_AVS2], pLeft[MAX_CU_SIZE_AVS2], pT[MAX_CU_SIZE_AVS2], pL[MAX_CU_SIZE_AVS2], wy[MAX_CU_SIZE_AVS2];

	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	for (x = 0; x < iWidth; x++) {
		pTop[x] = pSrc[1 + x];
		//printf("pTop[x]:%d ", pTop[x]);
	}

	for (y = 0; y < iHeight; y++) {
		pLeft[y] = pSrc[-1 - y];
		//printf("pLeft[y]:%d ", pLeft[y]);
	}
	a = pTop[iWidth - 1];
	b = pLeft[iHeight - 1];

	c = (iWidth == iHeight) ? (a + b + 1) >> 1 :
		(((a << ishift_x) + (b << ishift_y)) * 13 + (1 << (ishift + 5))) >> (ishift + 6);
	w = (c << 1) - a - b;


	for (x = 0; x < iWidth; x++) {
		pT[x] = b - pTop[x];
		pTop[x] <<= ishift_y;
		//printf("pT[x]:%d, pTop[x]:%d \n ", pT[x], pTop[x]);
	}
	tmp = 0;
	for (y = 0; y < iHeight; y++) {
		pL[y] = a - pLeft[y];
		pLeft[y] <<= ishift_x;
		wy[y] = tmp;
		tmp += w;
		//printf("pL[y]:%d, pLeft[y]:%d \n ", pL[y], pLeft[y]);
	}


	for (y = 0; y < iHeight; y++) {
		predx = pLeft[y];
		wxy = 0;
		for (x = 0; x < iWidth; x++) {
			predx += pL[y];

			pTop[x] += pT[x];
	        //printf("predx:%d, pT[x]:%d, pTop[x]:%d, ishift_x:%d, wxy:%d, offset:%d, ishift_xy:%d \n", predx, pT[x], pTop[x], wxy, ishift_x, offset,ishift_xy);
			pDst[y][x] = Clip3(0, (1 << sample_bit_depth) - 1,
				(((predx << ishift_y) + (pTop[x] << ishift_x) + wxy + offset) >> ishift_xy));
			wxy += wy[y];
		}
	}

}

void AVS2PredicterCMode::xPredIntraAngAdi(int *pSrc, int **pDst, int uiDirMode, int iWidth, int iHeight)
{
	int  iDx, iDy, i, j, iTempDx, iTempDy, iXx, iXy, iYx, iYy;
	int  uixyflag = 0; // 0 for x axis 1 for y axis
	int offset, offsetx, offsety;
	int iX, iY, iXn, iYn, iXnN1, iYnN1, iXnP2, iYnP2;
	int iDxy;
	int iWidth2 = iWidth << 1;
	int iHeight2 = iHeight << 1;

	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
	}
	int  *rpSrc = pSrc;
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
				pDst[j][i] = (rpSrc[iXnN1] * (32 - offset) + rpSrc[iX] * (64 - offset) +
					rpSrc[iXn] * (32 + offset) + rpSrc[iXnP2] * offset + 64) >> 7;
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

				pDst[j][i] = (rpSrc[-iYnN1] * (32 - offset) + rpSrc[-iY] * (64 - offset) +
					rpSrc[-iYn] * (32 + offset) + rpSrc[-iYnP2] * offset + 64) >> 7;
			}
		}
	}
}

void AVS2PredicterCMode::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {

	int  *pSrc = NULL;
	if (src_data && src_data->avs2_src) {
		pSrc = src_data->avs2_src + ((1 << cu_size_log) * 2);
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

int AVS2PredicterCMode::getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset)
{
	int imult = g_aucDirDxDy[uiXYflag][uiDirMode][0];
	int ishift = g_aucDirDxDy[uiXYflag][uiDirMode][1];
	int iTempDn = iTempD * imult >> ishift;
	*offset = ((iTempD * imult * 32) >> ishift) - iTempDn * 32;
	return iTempDn;
}



void AVS2PredicterCMode::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX)
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

void AVS2PredicterCMode::initDstData() {
	avs_dst = new int *[tu_height];
	for (int i = 0; i < tu_height; i++) {
		avs_dst[i] = new int[tu_width]();
	}
}

void AVS2PredicterCMode::deinitDstData() {
	if (avs_dst) {
		for (int i = 0; i < tu_height; i++) {
			delete[] avs_dst[i];
		}
		delete[] avs_dst;
		avs_dst = NULL;
	}
}
