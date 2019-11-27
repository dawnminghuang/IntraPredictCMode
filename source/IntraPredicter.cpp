#include "IntraPredicter.h"

IntraPredicter::IntraPredicter() {
	distanceCalculator = new DistanceCalculator();
	outPutWriter = new OutputWriter();
	if (distanceCalculator) {
		if (outPutWriter) {
			distanceCalculator->setOutPutWriter(outPutWriter);
		}
	}
	calc_mode = CALCU_MODE_MATRI;
	memset(outPath, 0, MAX_PATH_LENGHT);
	src_data = NULL;
	extraType = EXTRACT_BOUND_4;
	groupType = GROUD_TYPE_MIN;
	bi_left_min = 0;
	bi_above_min = 0;
	bi_above_max = 0;
}

void IntraPredicter::predict(){
}

void IntraPredicter::setPredictSrcData(SrcData *srcData){
	src_data = srcData;
}

void IntraPredicter::setPredictMode(int mode){
	calc_mode  = mode;
}

void IntraPredicter::setTuSize(int width, int height) {
	tu_width = width;
	tu_height = height;
}
void IntraPredicter::generateOutPath(char * protocolPath, int calcMode) {
	if (_access(protocolPath, 0) < 0) {
		if (_mkdir(protocolPath) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
		}
	}
	strcpy(outPath, protocolPath);
	strcpy(protocolOutPath, protocolPath);
	if (calcMode == CALCU_MODE_ROW) {
		strcat(outPath, "modeRow\\");
	}
	else if (calcMode == CALCU_MODE_COL) {
		strcat(outPath, "modeCol\\");
	}
	else if (calcMode == CALCU_MODE_MATRI) {
		strcat(outPath, "modeMatri\\");
	}
	else if (calcMode == CALCU_MODE_MATRI_4X2) {
		strcat(outPath, "modeMatri4X2\\");
	}
	else if (calcMode == CALCU_MODE_MATRI_2X4) {
		strcat(outPath, "modeMatri2X4\\");
	}

}

void IntraPredicter::generateDigOutPath(char * protocolPath, int calcMode) {
	if (_access(protocolPath, 0) < 0) {
		if (_mkdir(protocolPath) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
		}
	}
	strcpy(digOutPath, protocolPath);
	if (calcMode == CALCU_MODE_ROW) {
		strcat(digOutPath, "modeRowDig\\");
	}
	else if (calcMode == CALCU_MODE_COL) {
		strcat(digOutPath, "modeColDig\\");
	}
	else if (calcMode == CALCU_MODE_MATRI) {
		strcat(digOutPath, "modeMatriDigPair\\");
	}
	else if (calcMode == CALCU_MODE_MATRI_4X2) {
		strcat(digOutPath, "modeMatri4X2Dig\\");
	}
	else if (calcMode == CALCU_MODE_MATRI_2X4) {
		strcat(digOutPath, "modeMatri2X4Dig\\");
	}

}

void IntraPredicter::saveDistanceMatri(DistanceData* distanMatri, int uriMode, int width, int height, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2)
{
	distanMatri->distance_matri[j][i][0] = iYnN1;
	distanMatri->distance_matri[j][i][1] = iY;
	distanMatri->distance_matri[j][i][2] = iYn;
	distanMatri->distance_matri[j][i][3] = iYnP2;
}

void IntraPredicter::convertXPoints(int* iYnN1, int* iY, int* iYn, int* iYnP2)
{
	*iYnN1 = -*iYnN1 - 2;
	*iY = -*iY - 2;
	*iYn = -*iYn - 2;
	*iYnP2 = -*iYnP2 - 2;

}

void IntraPredicter::writeMaxDistanceToFile(int calcMode)
{
	if (calcMode == CALCU_MODE_ROW) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeRow");
		outPutWriter->writeDistanceToFile(distanceCalculator->row_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_COL) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeCol");
		outPutWriter->writeDistanceToFile(distanceCalculator->col_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_MATRI) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_MATRI_4X2) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri4X2");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_MATRI_2X4) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri2X4");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_ALL) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeRow");
		outPutWriter->writeDistanceToFile(distanceCalculator->row_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeCol");
		outPutWriter->writeDistanceToFile(distanceCalculator->col_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri4X2");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri2X4");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
	}
}


void IntraPredicter::writeDiagPostionToFile()
{
	int** calcMatri = distanceCalculator->calc_matri;
	bool isDiag = distanceCalculator->is_diag;
	if (calcMatri && !isDiag) {
		int pixel_number = distanceCalculator->calc_pixel_number;
		int distance_size = distanceCalculator->distance_size;
		int y_ratio = CALCURATIO;
		outPutWriter->writeModeInfoToFile(calcMatri, pixel_number, distance_size, y_ratio);
	}
}


void IntraPredicter::writePostionToFile(DistanceData* distanMatri)
{
	if (distanMatri) {
		outPutWriter->writeModeInfoToFile(distanMatri);
	}
}

int IntraPredicter::calcMin(int** data, int width, int height)
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

int IntraPredicter::calcMax(int** data, int width, int height)
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

void IntraPredicter::initIndexMatri(int maxIndexNumber, int distanceSize) {
	max_min_indexs = new int *[maxIndexNumber];
	for (int i = 0; i < maxIndexNumber; i++) {
		max_min_indexs[i] = new int[distanceSize]();
	}
}

void IntraPredicter::group256(int index) {
	int left128 = 0;
	int right128 = 0;
	int boud128 = MAX_INDEX / 2;
	if (index <= boud128) {
		left128 = 0;
		right128 = boud128;
	}
	else {
		left128 = boud128 + 1;
		right128 = MAX_INDEX;
	}
	if (extraType == EXTRACT_BOUND_128) {
		extractBouds(left128, right128);
	}
	else {
		group128(left128, right128, index);
	}
}

void IntraPredicter::group128(int left, int right, int index) {
	int left64 = 0;
	int right64 = 0;
	int boud64 = left + (right - left) / 2;
	if (index <= boud64) {
		left64 = left;
		right64 = boud64;
	}
	else {
		left64 = boud64 + 1;
		right64 = right;
	}
	if (extraType == EXTRACT_BOUND_64) {
		extractBouds(left64, right64);
	}
	else {
		group64(left64, right64, index);
	}
}

void IntraPredicter::group64(int left, int right, int index) {
	int left32 = 0;
	int right32 = 0;
	int boud32 = left + (right - left) / 2;
	if (index <= boud32) {
		left32 = left;
		right32 = boud32;
	}
	else {
		left32 = boud32 + 1;
		right32 = right;
	}
	if (extraType == EXTRACT_BOUND_32) {
		extractBouds(left32, right32);
	}
	else {
		group32(left32, right32, index);
	}
}

void IntraPredicter::group32(int left, int right, int index) {
	int left16 = 0;
	int right16 = 0;
	int boud16 = left + (right - left) / 2;
	if (index <= boud16) {
		left16 = left;
		right16 = boud16;
	}
	else {
		left16 = boud16 + 1;
		right16 = right;
	}
	if (extraType == EXTRACT_BOUND_16) {
		extractBouds(left16, right16);
	}else {
		group16(left16, right16, index);
	}

}

void IntraPredicter::group16(int left, int right, int index) {
	int left8 = 0;
	int right8 = 0;
	int boud8 = left + (right - left) / 2;
	if (index <= boud8) {
		left8 = left;
		right8 = boud8;
	}else {
		left8 = boud8 + 1;
		right8 = right;
	}

	if (extraType == EXTRACT_BOUND_8) {
		extractBouds(left8, right8);
	}else {
		group8(left8, right8, index);
	}
}

void IntraPredicter::group8(int left, int right, int index) {
	int left4 = 0;
	int right4 = 0;
	int bounds[2] = { 0 };
	int boud4 = left + (right - left) / 2;
	if (index <= boud4) {
		left4 = left;
		right4 = boud4;
	}
	else {
		left4 = boud4 + 1;
		right4 = right;
	}
	if(extraType == EXTRACT_BOUND_4){
	    extractBouds(left4, right4);
	}

}
void IntraPredicter::extractBouds(int left, int right) {
	if (groupType == GROUD_TYPE_MIN) {
		minBounds[0] = left;
		minBounds[1] = right;
	}
	else {
		maxBounds[0] = left;
		maxBounds[1] = right;
	}
}

void IntraPredicter::initLog2size() {
	memset(g_log2size, -1, MAX_CU_SIZE_AVS2 + 1);
	int c = 2;
	for (int k = 4; k <= MAX_CU_SIZE_AVS2; k *= 2) {
		g_log2size[k] = c;
		c++;
	}
}
void IntraPredicter::deinitIndexMatri(int maxIndexNumber) {
	if (max_min_indexs) {
		for (int i = 0; i < maxIndexNumber; i++) {
			delete[] max_min_indexs[i];
		}
		delete[] max_min_indexs;
		max_min_indexs = NULL;
	}
}

IntraPredicter::~IntraPredicter() {
	if (distanceCalculator)
		delete distanceCalculator;
	if (outPutWriter)
		delete outPutWriter;
}



