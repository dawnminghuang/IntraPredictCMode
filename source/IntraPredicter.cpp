#include "IntraPredicter.h"

IntraPredicter::IntraPredicter() {
	distanceCalculator = new DistanceCalculator();
	outPutWriter = new OutputWriter();
	calc_mode = CALCU_MODE_MATRI;
	memset(outPath, 0, MAX_PATH_LENGHT);
	src_data = NULL;
}

void IntraPredicter::predict(){
}

void IntraPredicter::setPredictSrcData(SrcData *srcData){
	src_data = srcData;
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
		outPutWriter->writeDistanceToFile(distanceCalculator->matri4X2_max_distance, distanceCalculator->mode_number);
	}
	else if (calcMode == CALCU_MODE_ALL) {
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeRow");
		outPutWriter->writeDistanceToFile(distanceCalculator->row_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeCol");
		outPutWriter->writeDistanceToFile(distanceCalculator->col_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri_max_distance, distanceCalculator->mode_number);
		outPutWriter->initDistanceInfoFp(protocolOutPath, "ModeMatri4X2");
		outPutWriter->writeDistanceToFile(distanceCalculator->matri4X2_max_distance, distanceCalculator->mode_number);
	}
}

void IntraPredicter::writePostionToFile(DistanceData* distanMatri)
{
	if (distanMatri) {
		outPutWriter->writeModeInfoToFile(distanMatri);
	}
}
IntraPredicter::~IntraPredicter() {
	if (distanceCalculator)
		delete distanceCalculator;
	if (outPutWriter)
		delete outPutWriter;
}



