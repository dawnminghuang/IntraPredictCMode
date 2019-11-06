#include "HevcPredicter.h"


HevcPredicter::HevcPredicter() {
}

HevcPredicter::~HevcPredicter() {

}

void HevcPredicter::predict() {
	int mode_number = NUM_INTRA_PMODE_HEVC + START_INDEX_HEVC;
	int max_cu_size = 64;
	generateOutPath(HEVC_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_HEVC; i++) {
		int uiDirMode = g_prdict_mode_hevc[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		computeIntraPredAngle(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			int iWidth = g_cu_size_hevc[j][0];
			int iHeight = g_cu_size_hevc[j][1];
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			writePostionToFile(distanMatri);
			delete distanMatri;
		}
	}
	writeMaxDistanceToFile(calc_mode);
}

void HevcPredicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	int delta_pos = 0;
	int delta_int = 0;
	if (is_mod_ver) {
		for (int j = 0; j < iHeight; j++) {
			delta_pos = delta_pos + intra_pred_angle;
			delta_int = delta_pos >> 5;
			for (int i = 0; i < iWidth; i++) {
				int ref_main_index = i + delta_int;
				int ref_side_index = ref_main_index + 1;
				saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
			}
		}
	}
	else {
		for (int i = 0; i < iWidth; i++) {
			delta_pos = delta_pos + intra_pred_angle;
			delta_int = delta_pos >> 5;
			for (int j = 0; j < iHeight; j++) {
				int ref_main_index = delta_int - j - 1;
				int ref_side_index = ref_main_index + 1;
				saveDistanceMatri(distanMatri, i, j, ref_main_index, ref_side_index);
			}
		}
	}
}


int HevcPredicter::computeIntraPredAngle(int uiDirMode) {
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
	printf("uiDirMode:%d, is_mod_ver:%d \n", uiDirMode, is_mod_ver);
	abs_ang = g_ang_table[abs_ang_mode];
	intra_pred_angle = sign_ang * abs_ang;
	return intra_pred_angle;
}


void HevcPredicter::saveDistanceMatri(DistanceData* distanMatri, int i, int j, int ref_main_index, int ref_side_index)
{
	distanMatri->distance_matri[j][i][0] = ref_main_index;
	distanMatri->distance_matri[j][i][1] = ref_side_index;

}


