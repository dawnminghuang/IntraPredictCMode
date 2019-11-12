#include "HevcPredicterCMode.h"


HevcPredicterCMode::HevcPredicterCMode() {
	max_dst_number = MAX_CU_SIZE_HEVC * MAX_CU_SIZE_HEVC;
	hevc_dst = NULL;
	dst_stride_True = MAX_CU_SIZE_HEVC;
}

HevcPredicterCMode::~HevcPredicterCMode() {

}

void HevcPredicterCMode::predict() {
	int mode_number = NUM_INTRA_PMODE_HEVC + START_INDEX_HEVC;
	int max_cu_size = 64;
	generateOutPath(HEVC_PATH, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_HEVC; i++) {
		int uiDirMode = g_prdict_mode_hevc[i];
		outPutWriter->initDstDataFp(HEVC_DATA_PATH_CMODE, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		computeIntraPredAngle(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			int iWidth = g_cu_size_hevc[j][0];
			int iHeight = g_cu_size_hevc[j][1];
			initDstData();
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
			predIntraAngAdi(distanMatri, uiDirMode);
			outPutWriter->writeDstDataToFile((int *)hevc_dst, iWidth, iHeight, dst_stride_True);
			deinitDstData();
			delete distanMatri;
		}
	}
}

void HevcPredicterCMode::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int bitDepth = 8;
	int srcStride = 2 * iWidth + 1;

	int *pSrc = NULL;
	if (src_data && src_data->hevc_src) {
		pSrc = src_data->hevc_src + srcStride + 1;
	}
	Int width = Int(iWidth);
	Int height = Int(iHeight);


	const Bool       bIsModeVer = (uiDirMode >= 18);
	const Int        intraPredAngleMode = (bIsModeVer) ? (Int)uiDirMode - VER_IDX : -((Int)uiDirMode - HOR_IDX);
	const Int        absAngMode = abs(intraPredAngleMode);
	const Int        signAng = intraPredAngleMode < 0 ? -1 : 1;

	// Set bitshifts and scale the angle parameter to block size
	static const Int angTable[9] = { 0,    2,    5,   9,  13,  17,  21,  26,  32 };
	static const Int invAngTable[9] = { 0, 4096, 1638, 910, 630, 482, 390, 315, 256 }; // (256 * 32) / Angle
	Int invAngle = invAngTable[absAngMode];
	Int absAng = angTable[absAngMode];
	Int intraPredAngle = signAng * absAng;

	Pel* refMain;
	Pel* refSide;

	Pel  refAbove[2 * MAX_CU_SIZE_HEVC + 1];
	Pel  refLeft[2 * MAX_CU_SIZE_HEVC + 1];

	// Initialize the Main and Left reference array.
	if (intraPredAngle < 0)
	{
		const Int refMainOffsetPreScale = (bIsModeVer ? height : width) - 1;
		const Int refMainOffset = height - 1;
		for (Int x = 0; x < width + 1; x++)
		{
			refAbove[x + refMainOffset] = pSrc[x - srcStride - 1];
		}
		for (Int y = 0; y < height + 1; y++)
		{
			refLeft[y + refMainOffset] = pSrc[(y - 1)*srcStride - 1];
		}
		refMain = (bIsModeVer ? refAbove : refLeft) + refMainOffset;
		refSide = (bIsModeVer ? refLeft : refAbove) + refMainOffset;

		// Extend the Main reference to the left.
		Int invAngleSum = 128;       // rounding for (shift by 8)
		for (Int k = -1; k > (refMainOffsetPreScale + 1)*intraPredAngle >> 5; k--)
		{
			invAngleSum += invAngle;
			refMain[k] = refSide[invAngleSum >> 8];
		}
	}
	else
	{
		for (Int x = 0; x < 2 * width + 1; x++)
		{
			refAbove[x] = pSrc[x - srcStride - 1];
		}
		for (Int y = 0; y < 2 * height + 1; y++)
		{
			refLeft[y] = pSrc[(y - 1)*srcStride - 1];
		}
		refMain = bIsModeVer ? refAbove : refLeft;
		refSide = bIsModeVer ? refLeft : refAbove;
	}

	// swap width/height if we are doing a horizontal mode:
	Pel tempArray[MAX_CU_SIZE_HEVC*MAX_CU_SIZE_HEVC];
	const Int dstStride = bIsModeVer ? dst_stride_True : MAX_CU_SIZE_HEVC;
	Pel *pDst = bIsModeVer ? hevc_dst : tempArray;

	if (!bIsModeVer)
	{
		std::swap(width, height);
	}

	if (intraPredAngle == 0)  // pure vertical or pure horizontal
	{
		for (Int y = 0; y < height; y++)
		{
			for (Int x = 0; x < width; x++)
			{
				pDst[y*dstStride + x] = refMain[x + 1];
			}
		}

	}
	else
	{
		Pel *pDsty = pDst;

		for (Int y = 0, deltaPos = intraPredAngle; y < height; y++, deltaPos += intraPredAngle)
		{
			const Int deltaInt = deltaPos >> 5;
			const Int deltaFract = deltaPos & (32 - 1);

			if (deltaFract)
			{
				// Do linear filtering
				const Pel *pRM = refMain + deltaInt + 1;
				Int lastRefMainPel = *pRM++;
				for (Int x = 0; x < width; pRM++, x++)
				{
					Int thisRefMainPel = *pRM;
					pDsty[x + y* dstStride] = (Pel)(((32 - deltaFract)*lastRefMainPel + deltaFract * thisRefMainPel + 16) >> 5);
					lastRefMainPel = thisRefMainPel;
				}
			}
			else
			{
				// Just copy the integer samples
				for (Int x = 0; x < width; x++)
				{
					pDsty[x + y * dstStride] = refMain[x + deltaInt + 1];
				}
			}
		}
	}

	// Flip the block if this is the horizontal mode
	int printDst[1024] = { 0 };
	int printDstTrue[1024] = { 0 };
	int printNum = 0;
	if (!bIsModeVer)
	{
		for (Int y = 0; y < height; y++)
		{
			for (Int x = 0; x < width; x++)
			{
				hevc_dst[x*dst_stride_True + y] = pDst[x];
			}
			pDst += dstStride;
		}

	}

}



int HevcPredicterCMode::computeIntraPredAngle(int uiDirMode) {
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
	printf("CMode uiDirMode:%d, is_mod_ver:%d \n", uiDirMode, is_mod_ver);
	abs_ang = g_ang_table[abs_ang_mode];
	inv_angle = g_inv_ang_table[abs_ang_mode];
	intra_pred_angle = sign_ang * abs_ang;
	return intra_pred_angle;
}


void HevcPredicterCMode::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index)
{
	distanMatri->distance_matri[j][i][0] = ref_main_index;
	distanMatri->distance_matri[j][i][1] = ref_side_index;

}


void HevcPredicterCMode::initDstData() {
	hevc_dst = new Pel[max_dst_number]();
}

void HevcPredicterCMode::deinitDstData() {
	if (hevc_dst) {
		delete[] hevc_dst;
	}
}