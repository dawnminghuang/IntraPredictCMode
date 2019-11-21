#include "DistanceCalculator.h"
DistanceCalculator::DistanceCalculator() {
	calcu_mode = CALCU_MODE_MATRI;
	predict_mode = 0;
	mode_number = 0;
	max_point_number = CALCURATIO;
	calc_pixel_number = 0;
	col_max_distance = NULL;
	row_max_distance = NULL;
	matri_max_distance = NULL;
	matri4X2_max_distance = NULL;
	cu_distance = NULL;
	calc_matri = NULL;
	calc_matri_dig = NULL;
	output_writer = NULL;
	matri_max = 0;
	matri_min = 0;
	mini = 0;
	maxi = 0;
	is_max_diag = false;
	is_min_diag = false;
	is_diag = false;
	is_pair = false;
}

DistanceCalculator::~DistanceCalculator() {
	deinitCalcuMatri(distance_size, calc_pixel_number);
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
	for (int i = 0; i < CALCURATIO; i++) {
		max_index[i][0] = -1;
		max_index[i][1] = -1;
		min_index[i][0] = -1;
		min_index[i][1] = -1;
	}
}

void DistanceCalculator::initMinMaxPair() {
	for (int i = 0; i < CALCURATIO; i++) {
		max_index[i][0] = -1;
		max_index[i][1] = -1;
		min_index[i][0] = -1;
		min_index[i][1] = -1;
	}
}
void DistanceCalculator::initCalcuMatri(int width, int height) {
	calc_matri = new int *[height];
	for (int i = 0; i < height; i++) {
		calc_matri[i] = new int[width]();
	}
	calc_matri_dig = new int[height]();
}

void DistanceCalculator::deinitCalcuMatri(int width, int height) {
	if (calc_matri) {
		for (int i = 0; i < height; i++) {
			delete[] calc_matri[i];
		}
		delete[] calc_matri;
		calc_matri = NULL;
	}
	if (calc_matri_dig) {
		delete[] calc_matri_dig;
		calc_matri_dig = NULL;
	}
}
void DistanceCalculator::setPredictMode(int mode) {
	predict_mode = mode;
}

void DistanceCalculator::setCalcuMode(int mode) {
	calcu_mode = mode;
}

void DistanceCalculator::setOutPutWriter(OutputWriter* outPutWriter) {
	output_writer = outPutWriter;
}
void DistanceCalculator::calcuDistance(DistanceData* distanceMatri) {
	if (distanceMatri) {
		if (distanceMatri->distance_matri) {
			if (calcu_mode == CALCU_MODE_ROW) {
				calcuDistanceRow(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_COL) {
				calcuDistanceCol(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_MATRI) {
				calcuDistanceMatri(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_MATRI_4X2) {
				calcuDistanceMatri4X2(distanceMatri);
			}
			else if (calcu_mode == CALCU_MODE_MATRI_2X4) {
				calcuDistanceMatri2X4(distanceMatri);
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
	int x_offset = 1;
	int y_offset = CALCURATIO;
	int width_number = distanceMatri->tu_width/ x_offset;
	int heigh_number = distanceMatri->tu_height/ y_offset;
	distance_size = distanceMatri->distance_size;
	calc_pixel_number = y_offset* x_offset;
	initCalcuMatri(distance_size, calc_pixel_number);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int i = 0; i < width_number; i++) {
		for (int j = 0; j < heigh_number; j++) {
			for (int k = j * CALCURATIO; k < y_offset*(j + 1); k++) {
				for (int m = 0; m < distance_size; m++) {
					calc_matri[cal_matri_index][m] = distanceMatri->distance_matri[k][i][m];
				}
				cal_matri_index++;
			}
			int distance_max = calcMax(calc_matri, distance_size, calc_pixel_number);
			matri_max = distance_max;
			int distance_min = calcMin(calc_matri, distance_size, calc_pixel_number);
			matri_min = distance_min;
			calcuMinMaxPosition();
			writeDiagPostionToFile();
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i;
			pos_y[distance_index] = j * y_offset;
			if (distance > col_max_distance[predict_mode])
				col_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(distance_size, calc_pixel_number);
}


void DistanceCalculator::calcuDistanceRow(DistanceData* distanceMatri) {
	initCalcuParas();
	int x_offset = CALCURATIO;
	int y_offset = 1;
	int width_number = distanceMatri->tu_width / x_offset;
	int heigh_number = distanceMatri->tu_height/ y_offset;
	distance_size = distanceMatri->distance_size;
	calc_pixel_number = x_offset * y_offset;
	initCalcuMatri(distance_size, calc_pixel_number);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int k = i * CALCURATIO; k < x_offset*(i + 1); k++) {
				for (int m = 0; m < distance_size; m++) {
					calc_matri[cal_matri_index][m] = distanceMatri->distance_matri[j][k][m];
				}
				cal_matri_index++;
			}
			int distance_max = calcMax(calc_matri, distance_size, calc_pixel_number);
			matri_max = distance_max;
			int distance_min = calcMin(calc_matri, distance_size, calc_pixel_number);
			matri_min = distance_min;
			calcuMinMaxPosition();
			writeDiagPostionToFile();
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * x_offset;
			pos_y[distance_index] = j;
			if (distance > row_max_distance[predict_mode])
				row_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(distance_size, calc_pixel_number);
}


void DistanceCalculator::calcuDistanceMatri(DistanceData* distanceMatri) {
	initCalcuParas();
	int x_offset = CALCURATIO;
	int y_offset = CALCURATIO;
	int width_number = distanceMatri->tu_width / x_offset;
	int heigh_number = distanceMatri->tu_height / y_offset;
	distance_size = distanceMatri->distance_size;
	calc_pixel_number = x_offset * y_offset;
	initCalcuMatri(distance_size, calc_pixel_number);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					for (int k = 0; k < distance_size; k++) {
						calc_matri[cal_matri_index][k] = distanceMatri->distance_matri[m][n][k];
					}
					cal_matri_index++;
				}
			}
			int distance_max = calcMax(calc_matri, distance_size, calc_pixel_number);
			matri_max = distance_max;
			int distance_min = calcMin(calc_matri, distance_size, calc_pixel_number);
			matri_min = distance_min;
			calcuMinMaxPosition();
			writeDiagPostionToFile();
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * x_offset;
			pos_y[distance_index] = j * y_offset;
			if (distance > matri_max_distance[predict_mode])
				matri_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(distance_size, calc_pixel_number);
}


void DistanceCalculator::calcuDistanceMatri2X4(DistanceData* distanceMatri) {
	initCalcuParas();
	int x_offset = CALCURATIO;
	int y_offset = (CALCURATIO / 2);
	int width_number = distanceMatri->tu_width / x_offset;
	int heigh_number = distanceMatri->tu_height / y_offset;
    distance_size = distanceMatri->distance_size;
	calc_pixel_number = x_offset * y_offset;
	initCalcuMatri(distance_size, calc_pixel_number);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					for (int k = 0; k < distance_size; k++) {
						calc_matri[cal_matri_index][k] = distanceMatri->distance_matri[m][n][k];
					}
					cal_matri_index++;
				}
			}
			int distance_max = calcMax(calc_matri, distance_size, calc_pixel_number);
			matri_max = distance_max;
			int distance_min = calcMin(calc_matri, distance_size, calc_pixel_number);
			matri_min = distance_min;
			calcuMinMaxPosition();
			writeDiagPostionToFile();
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * x_offset;
			pos_y[distance_index] = j * y_offset;
			if (distance > matri_max_distance[predict_mode])
				matri_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(distance_size, calc_pixel_number);
}


void DistanceCalculator::calcuDistanceMatri4X2(DistanceData* distanceMatri) {
	initCalcuParas();
	int x_offset = (CALCURATIO / 2);
	int y_offset = CALCURATIO;
	int width_number = distanceMatri->tu_width/ x_offset;
	int heigh_number = distanceMatri->tu_height/ y_offset;

	distance_size = distanceMatri->distance_size;
	calc_pixel_number = x_offset * y_offset;
	initCalcuMatri(distance_size, calc_pixel_number);
	int cal_matri_index = 0;
	int distance_index = 0;
	for (int j = 0; j < heigh_number; j++) {
		for (int i = 0; i < width_number; i++) {
			for (int m = j * y_offset; m < y_offset*(j + 1); m++) {
				for (int n = i * x_offset; n < x_offset*(i + 1); n++) {
					for (int k = 0; k < distance_size; k++) {
						calc_matri[cal_matri_index][k] = distanceMatri->distance_matri[m][n][k];
					}
					cal_matri_index++;
				}
			}
			int distance_max = calcMax(calc_matri, distance_size, calc_pixel_number);
			matri_max = distance_max;
			int distance_min = calcMin(calc_matri, distance_size, calc_pixel_number);
			matri_min = distance_min;
			calcuMinMaxPosition();
			writeDiagPostionToFile();
			int distance = abs(distance_max - distance_min) + 1;
			cu_distance[distance_index] = distance;
			pos_x[distance_index] = i * x_offset;
			pos_y[distance_index] = j * y_offset;
			if (distance > matri_max_distance[predict_mode])
				matri_max_distance[predict_mode] = distance;
			cal_matri_index = 0;
			distance_index++;
		}
	}
	deinitCalcuMatri(distance_size, calc_pixel_number);
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


void DistanceCalculator::calcuMinMaxPosition()
{
	is_max_diag = false;
	is_min_diag = false;
	is_diag = false;
	is_pair = false;
	mini = 0;
	maxi = 0;
	bool ismax = false;
	initMinMaxPair();
	for (int j = 0; j < calc_pixel_number; j++) {
		calc_matri_dig[j] = 0;
		for (int i = 0; i < distance_size; i++) {
			if (calc_matri[j][i] == matri_max) {
				ismax = true;
				isMinMaxDiag(j, CALCURATIO, &is_max_diag, ismax);
				calc_matri_dig[j] = 1;
			}
			if (calc_matri[j][i] == matri_min) {
				ismax = false;
				isMinMaxDiag(j, CALCURATIO,&is_min_diag, ismax);
				calc_matri_dig[j] = 1;
			}
		}
	}
	if(calcu_mode == CALCU_MODE_MATRI){
		isMaxMinPair(&is_pair);
	}
	is_diag = (is_max_diag && is_min_diag);
}

void DistanceCalculator::isMaxMinPair(bool *isPair)
{
	for (int i = 0; i < CALCURATIO; i++) {
		int maxx = max_index[i][0];
		int maxy = max_index[i][1];
		for (int j = 0; j < CALCURATIO; j++) {
			int minx = min_index[j][0];
			int miny = min_index[j][1];
			if (((abs(maxx - minx) + abs(maxy - miny)) == 6) || ((abs(maxx - minx) + abs(maxy - miny)) == 0)) {
				*isPair = true;
			}
		}
	}
}

bool DistanceCalculator::isMaxMinPairAlreadyIn(int x, int y, bool max)
{
	int xx = -1;
	int yy = -1;
	for (int i = 0; i < CALCURATIO; i++) {
		if(max){
			xx = max_index[i][0];
			yy = max_index[i][1];
		}else {
			xx = min_index[i][0];
			yy = min_index[i][1];
		}
		if ((xx == x) && (yy == y)) {
			return true;
		}
	}
	return false;
}
void DistanceCalculator::isMinMaxDiag(int position, int calcRatio, bool *isDiag, bool max)
{
	int y = position / calcRatio;
	int x = position % calcRatio;
	if(calcu_mode == CALCU_MODE_MATRI){
		if (((x == 0) && (y == 0)) || ((x == 3) && (y == 0)) || ((x == 0) && (y == 3)) || ((x == 3) && (y == 3))) {
			*isDiag = true;
			if (max) {
				if(!isMaxMinPairAlreadyIn(x,y,max)){
					max_index[maxi][0] = x;
					max_index[maxi][1] = y;
					maxi++ ;
				}
			}else {
				if(!isMaxMinPairAlreadyIn(x, y, max)){
					min_index[mini][0] = x;
					min_index[mini][1] = y;
					mini++;
				}
			}
		}
	}else if (calcu_mode == CALCU_MODE_MATRI_4X2) {
		y = position/(calcRatio / 2);
		x = position% (calcRatio / 2);
		if (((x == 0) && (y == 0)) || ((x == 1) && (y == 0)) || ((x == 0) && (y == 3)) || ((x == 1) && (y == 3))) {
			*isDiag = true;
		}
	}else if (calcu_mode == CALCU_MODE_MATRI_2X4) {
		y = position / (calcRatio);
		x = position % (calcRatio);
		if (((x == 0) && (y == 0)) || ((x == 3) && (y == 0)) || ((x == 0) && (y == 1)) || ((x == 3) && (y == 1))) {
			*isDiag = true;
		}
	}else if (calcu_mode == CALCU_MODE_ROW) {
		y = position / (calcRatio);
		x = position % (calcRatio);
		if (((x == 0) && (y == 0)) || ((x == 3) && (y == 0))) {
			*isDiag = true;
		}
	}else if (calcu_mode == CALCU_MODE_COL) {
		y = position / (calcRatio);
		x = position % (calcRatio);
		if (((x == 0) && (y == 0)) || ((x == 3) && (y == 0))) {
			*isDiag = true;
		}
	}
}


void DistanceCalculator::writeDiagPostionToFile(){
	if (calc_matri && output_writer) {
		if(!is_diag){
			int y_ratio = CALCURATIO;
            if (calcu_mode == CALCU_MODE_MATRI_4X2) {
				y_ratio = CALCURATIO/2;
				//output_writer->writeModeInfoToFile(calc_matri, calc_matri_dig, calc_pixel_number, distance_size, y_ratio);
				output_writer->writeModeInfoToFile(calc_matri, calc_pixel_number, distance_size, y_ratio);
			}
			else if (calcu_mode == CALCU_MODE_MATRI_2X4) {
				y_ratio = CALCURATIO;
				//output_writer->writeModeInfoToFile(calc_matri, calc_matri_dig, calc_pixel_number, distance_size, y_ratio);
				output_writer->writeModeInfoToFile(calc_matri, calc_pixel_number, distance_size, y_ratio);
			}
		}
		if (!is_pair || !is_diag) {
			int y_ratio = CALCURATIO;
			if (calcu_mode == CALCU_MODE_MATRI) {
				y_ratio = CALCURATIO;
				printf("CALCU_MODE_MATRI no pair ...............\n");
				//output_writer->writeModeInfoToFile(calc_matri, calc_matri_dig, calc_pixel_number, distance_size, y_ratio);
				output_writer->writeModeInfoToFile(calc_matri, calc_pixel_number, distance_size, y_ratio);
			}
		}
	}
}