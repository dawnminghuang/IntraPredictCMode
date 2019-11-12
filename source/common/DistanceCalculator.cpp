#include "DistanceCalculator.h"
DistanceCalculator::DistanceCalculator() {
	calcu_mode = CALCU_MODE_MATRI;
	predict_mode = 0;
	mode_number = 0;
	max_point_number = CALCURATIO;
	col_max_distance = NULL;
	row_max_distance = NULL;
	matri_max_distance = NULL;
	matri4X2_max_distance = NULL;
	cu_distance = NULL;

}

DistanceCalculator::~DistanceCalculator() {
	calcu_mode = CALCU_MODE_ROW;
	predict_mode = 0;
	mode_number = 0;
	max_point_number = CALCURATIO;
	col_max_distance = NULL;
	row_max_distance = NULL;
	matri_max_distance = NULL;
	matri4X2_max_distance = NULL;
	cu_distance = NULL;
	calc_matri = NULL;

}

void DistanceCalculator::initDistanceCalculator(int modeNumber, int maxCUsize, int calcMode) {
	max_point_number = maxCUsize * maxCUsize / CALCURATIO;
	mode_number = modeNumber;
	col_max_distance = new int[modeNumber]();
	row_max_distance = new int[modeNumber]();
	matri_max_distance = new int[modeNumber]();
	matri4X2_max_distance = new int[modeNumber]();
	cu_distance = new int[max_point_number]();
	pos_x = new int[max_point_number]();
	pos_y = new int[max_point_number]();
	calcu_mode = calcMode;
}
void DistanceCalculator::initCalcuParas() {
	for (int i = 0; i < max_point_number; i++) {
		pos_x[i] = 0;
		pos_y[i] = 0;
	}

	for (int i = 0; i < max_point_number; i++) {
		cu_distance[i] = 0;
	}
}

void DistanceCalculator::initCalcuMatri(int width, int height) {
	calc_matri = new int *[height];
	for (int i = 0; i < height; i++) {
		calc_matri[i] = new int[width]();
	}
}

void DistanceCalculator::deinitCalcuMatri(int **calc_matri, int width, int height) {
	if (calc_matri) {
		for (int i = 0; i < height; i++) {
			delete[] calc_matri[i];
		}
		delete[] calc_matri;
	}
}
void DistanceCalculator::setPredictMode(int mode) {
	predict_mode = mode;
}

void DistanceCalculator::setCalcuMode(int mode) {
	calcu_mode = mode;
}

void DistanceCalculator::calcuDistance(DistanceData* distanceMatri) {
	if (distanceMatri) {
		if (distanceMatri->distance_matri) {
			if (calcu_mode == CALCU_MODE_ROW) {
				calcuDistanceCol(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_COL) {
				calcuDistanceRow(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_MATRI) {
				calcuDistanceMatri(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_MATRI_4X2) {
				calcuDistanceMatri4X2(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_ALL) {
				calcuDistanceCol(distanceMatri);
				calcuDistanceRow(distanceMatri);
				calcuDistanceMatri(distanceMatri);
				calcuDistanceMatri4X2(distanceMatri);
			}

		}
	}
}

void DistanceCalculator::calcuDistanceCol(DistanceData* distanceMatri) {
	initCalcuParas();
	int width_number = distanceMatri->tu_width;
	int heigh_number = distanceMatri->tu_height / CALCURATIO;
	int distance_size = distanceMatri->distance_size;
	int point_unmber = CALCURATIO;
	initCalcuMatri(distance_size, point_unmber);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < width_number; j++) {
		for (int i = 0; i < heigh_number; i++) {
			for (int k = i * CALCURATIO; k < CALCURATIO*(i + 1); k++) {
				for (int m = 0; m < distance_size; m++) {
					calc_matri[cal_matri_index][m] = distanceMatri->distance_matri[k][j][m];
				}
				cal_matri_index++;
			}
			int distance_max = calcMax(calc_matri, distance_size, point_unmber);
			int distance_min = calcMin(calc_matri, distance_size, point_unmber);
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = j;
			pos_y[distance_index] = i * CALCURATIO;
			if (distance > col_max_distance[predict_mode])
				col_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(calc_matri, distance_size, point_unmber);
}


void DistanceCalculator::calcuDistanceRow(DistanceData* distanceMatri) {
	initCalcuParas();
	int width_number = distanceMatri->tu_width / CALCURATIO;
	int heigh_number = distanceMatri->tu_height;
	int distance_size = distanceMatri->distance_size;
	int point_unmber = CALCURATIO;
	initCalcuMatri(distance_size, point_unmber);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int k = i * CALCURATIO; k < CALCURATIO*(i + 1); k++) {
				for (int m = 0; m < distance_size; m++) {
					calc_matri[cal_matri_index][m] = distanceMatri->distance_matri[j][k][m];
				}
				cal_matri_index++;
			}
			int distance_max = calcMax(calc_matri, distance_size, point_unmber);
			int distance_min = calcMin(calc_matri, distance_size, point_unmber);
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * CALCURATIO;
			pos_y[distance_index] = j;
			if (distance > row_max_distance[predict_mode])
				row_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(calc_matri, distance_size, point_unmber);
}


void DistanceCalculator::calcuDistanceMatri(DistanceData* distanceMatri) {
	initCalcuParas();
	int width_number = distanceMatri->tu_width / CALCURATIO;
	int heigh_number = distanceMatri->tu_height / CALCURATIO;
	int distance_size = distanceMatri->distance_size;
	int point_unmber = CALCURATIO * CALCURATIO;
	initCalcuMatri(distance_size, point_unmber);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int m = j * CALCURATIO; m < CALCURATIO*(j + 1); m++) {
				for (int n = i * CALCURATIO; n < CALCURATIO*(i + 1); n++) {
					for (int k = 0; k < distance_size; k++) {
						calc_matri[cal_matri_index][k] = distanceMatri->distance_matri[m][n][k];
					}
					cal_matri_index++;
				}
			}
			int distance_max = calcMax(calc_matri, distance_size, point_unmber);
			int distance_min = calcMin(calc_matri, distance_size, point_unmber);
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * CALCURATIO;
			pos_y[distance_index] = j * CALCURATIO;
			if (distance > matri_max_distance[predict_mode])
				matri_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(calc_matri, distance_size, point_unmber);
}


void DistanceCalculator::calcuDistanceMatri4X2(DistanceData* distanceMatri) {
	initCalcuParas();
	int width_number = distanceMatri->tu_width / CALCURATIO;
	int heigh_number = distanceMatri->tu_height / (CALCURATIO / 2);
	int distance_size = distanceMatri->distance_size;
	int point_unmber = CALCURATIO * (CALCURATIO / 2);
	initCalcuMatri(distance_size, point_unmber);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int m = j * CALCURATIO; m < (CALCURATIO / 2)*(j + 1); m++) {
				for (int n = i * CALCURATIO; n < CALCURATIO*(i + 1); n++) {
					for (int k = 0; k < distance_size; k++) {
						calc_matri[cal_matri_index][k] = distanceMatri->distance_matri[m][n][k];
					}
					cal_matri_index++;
				}
			}
			int distance_max = calcMax(calc_matri, distance_size, point_unmber);
			int distance_min = calcMin(calc_matri, distance_size, point_unmber);
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * CALCURATIO;
			pos_y[distance_index] = j * (CALCURATIO / 2);
			if (distance > matri_max_distance[predict_mode])
				matri_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(calc_matri, distance_size, point_unmber);
}

int DistanceCalculator::calcMin(int** data, int width, int height)
{
	int min = data[0][0];
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (data[j][i] < min) {
				min = data[j][i];
			}
		}
	}
	return min;
}

int DistanceCalculator::calcMax(int** data, int width, int height)
{
	int max = data[0][0];
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (data[j][i] > max) {
				max = data[j][i];
			}
		}

	}
	return max;
}


