#include "OutputWriter.h"
//#define PRINT_ALL
OutputWriter::OutputWriter() {
	predictMode = 0;
	modeInfoFp = NULL;
	distanceInfoFp = NULL;
	outAllFp = NULL;
	dstDataFp = NULL;
	digPostionInfoFp = NULL;
}

OutputWriter::~OutputWriter() {

	if (modeInfoFp) {
		fclose(modeInfoFp);
		modeInfoFp = NULL;
	}

	if (distanceInfoFp) {
		fclose(distanceInfoFp);
		distanceInfoFp = NULL;
	}
	if (digPostionInfoFp) {
		fclose(digPostionInfoFp);
		digPostionInfoFp = NULL;
	}
}

int  OutputWriter::initOutputAllWriter(char * path) {
	if (_access(path, 0) < 0) {
		if (_mkdir(path) < 0) {
			return FAILURE;
		}
	}
	if (outAllFp) {
		fclose(outAllFp);
		outAllFp = NULL;
	}
	outAllFp = fopen(path, "w+");
	return SUCCESS;
}

int  OutputWriter::initModeInfoFp(char * path, int predictMode) {
	if (_access(path, 0) < 0) {
		if (_mkdir(path) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
			return FAILURE;
		}
	}
	memset(outPutPath, 0, MAX_PATH_LENGHT);
	char mode_path[MAX_MODE_PATH];
	sprintf(mode_path, "mode_%d.txt", predictMode);
	strcpy(outPutPath, path);
	strcat(outPutPath, mode_path);
	if (modeInfoFp) {
		fclose(modeInfoFp);
		modeInfoFp = NULL;
	}
	modeInfoFp = fopen(outPutPath, "w+");
	return SUCCESS;
}

int  OutputWriter::initDigPostionInfoFp(char * path, int predictMod) {
	if (_access(path, 0) < 0) {
		if (_mkdir(path) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
			return FAILURE;
		}
	}
	memset(outPutPath, 0, MAX_PATH_LENGHT);
	char mode_path[MAX_MODE_PATH];
	sprintf(mode_path, "%d.txt", predictMod);
	strcpy(outPutPath, path);
	strcat(outPutPath, mode_path);
	if (digPostionInfoFp) {
		fclose(digPostionInfoFp);
		digPostionInfoFp = NULL;
	}
	digPostionInfoFp = fopen(outPutPath, "w+");
	return SUCCESS;
}

int  OutputWriter::initDistanceInfoFp(char * path, char* calc_mode) {
	if (_access(path, 0) < 0) {
		if (_mkdir(path) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
			return FAILURE;
		}
	}
	memset(outPutPath, 0, MAX_PATH_LENGHT);
	char mode_path[MAX_MODE_PATH];
	sprintf(mode_path, "%s.txt", calc_mode);
	strcpy(outPutPath, path);
	strcat(outPutPath, mode_path);
	if (distanceInfoFp) {
		fclose(distanceInfoFp);
		distanceInfoFp = NULL;
	}
	distanceInfoFp = fopen(outPutPath, "w+");
	return SUCCESS;
}



int  OutputWriter::initDstDataFp(char * path, int predictMode) {
	if (_access(path, 0) < 0) {
		if (_mkdir(path) < 0) {
			printf("mk fail errno = %d reason = %s \n", errno, strerror(errno));
			return FAILURE;
		}
	}
	memset(outPutPath, 0, MAX_PATH_LENGHT);
	char mode_path[MAX_MODE_PATH];
	sprintf(mode_path, "mode_%d.txt", predictMode);
	strcpy(outPutPath, path);
	strcat(outPutPath, mode_path);
	if (dstDataFp) {
		fclose(dstDataFp);
		dstDataFp = NULL;
	}
	dstDataFp = fopen(outPutPath, "w+");
	return SUCCESS;
}

void  OutputWriter::writeModeInfoToFile(char *data) {
	if (modeInfoFp) {
		fprintf(modeInfoFp, "%s", data);
	}

}

void  OutputWriter::writeModeInfoToFile(DistanceData *distanceData) {
	if (distanceData) {
		for (int j = 0; j < distanceData->tu_height; j++) {
			for (int i = 0; i < distanceData->tu_width; i++) {
				if (modeInfoFp) {
#ifdef PRINT_ALL
					fprintf(modeInfoFp, "[");
					fflush(modeInfoFp);
					for (int k = 0; k < distanceData->distance_size; k++) {
						fprintf(modeInfoFp, "%2d ", distanceData->distance_matri[j][i][k]);
						fflush(modeInfoFp);
					}
					fprintf(modeInfoFp, "]");
					fflush(modeInfoFp);
#else
					fprintf(modeInfoFp, "%4d", distanceData->distance_matri[j][i][2]);
					fflush(modeInfoFp);
#endif
					if (i == (distanceData->tu_width - 1)) {
						fprintf(modeInfoFp, "\n");
						fflush(modeInfoFp);
					}
				}

			}
		}
		fprintf(modeInfoFp, "\n");
		fflush(modeInfoFp);
	}
}

void  OutputWriter::writeModeInfoToFile(int **calc_matri, int * calc_matri_dig, int calc_pixel_number, int distance_size, int y_ratio) {
	if (calc_matri && digPostionInfoFp) {
		for (int j = 0; j < calc_pixel_number; j++) {
			fprintf(digPostionInfoFp, "[");
			fflush(digPostionInfoFp);
			if(calc_matri_dig[j]){
				for (int i = 0; i < distance_size; i++) {
					fprintf(digPostionInfoFp, "%2d ", calc_matri[j][i]);
					fflush(digPostionInfoFp);
				}
			}else {
				fprintf(digPostionInfoFp, "            ");
				fflush(digPostionInfoFp);
			}
			fprintf(digPostionInfoFp, "]");
			fflush(digPostionInfoFp);
			if ((j%y_ratio) == (y_ratio - 1)) {
				fprintf(digPostionInfoFp, "\n");
				fflush(digPostionInfoFp);
			}
		}
		fprintf(digPostionInfoFp, "\n");
		fflush(digPostionInfoFp);
	}
}
void  OutputWriter::writeModeInfoToFile(int **calc_matri, int calc_pixel_number, int distance_size, int y_ratio) {
	if (calc_matri && digPostionInfoFp) {
		for (int j = 0; j < calc_pixel_number; j++) {
			fprintf(digPostionInfoFp, "[");
			fflush(digPostionInfoFp);
			for (int i = 0; i < distance_size; i++) {
			    fprintf(digPostionInfoFp, "%2d ", calc_matri[j][i]);
				fflush(digPostionInfoFp);
			}
			fprintf(digPostionInfoFp, "]");
			fflush(digPostionInfoFp);
			if ((j%y_ratio) == (y_ratio - 1)) {
				fprintf(digPostionInfoFp, "\n");
				fflush(digPostionInfoFp);
			}
		}
		fprintf(digPostionInfoFp, "\n");
		fflush(digPostionInfoFp);
	}
}
void  OutputWriter::writeDistanceToFile(int *distanceData, int modeNumber) {
	if (distanceData) {
		for (int j = 0; j < modeNumber; j++) {
			if (distanceInfoFp) {
				fprintf(distanceInfoFp, "%4d: ", j);
				fprintf(distanceInfoFp, "%4d ", distanceData[j]);
				fprintf(distanceInfoFp, "\n");
			}
		}
	}
}

void OutputWriter::writeDstDataToFile(int **dstData, int tu_width, int tu_height) {
	if (dstData) {
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				if (dstDataFp) {
					fprintf(dstDataFp, "%4d", dstData[j][i]);
					if (i == (tu_width - 1)) {
						fprintf(dstDataFp, "\n");
					}
				}

			}
		}
		if (dstDataFp){
		    fprintf(dstDataFp, "\n");
		}
	}
}

void OutputWriter::writeDstDataToFile(int *dstData, int tu_width, int tu_height,int dstStride) {
	if (dstData) {
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				if (dstDataFp) {
					fprintf(dstDataFp, "%4d", dstData[i]);
					if (i == (tu_width - 1)) {
						fprintf(dstDataFp, "\n");
					}
				}
			}
			dstData += dstStride;
		}
	}
}
void OutputWriter::writeDstDataToFile(uint8_t *dstData, int tu_width, int tu_height, int dstStride) {
	if (dstData) {
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				if (dstDataFp) {
					fprintf(dstDataFp, "%4d", dstData[i]);
					if (i == (tu_width - 1)) {
						fprintf(dstDataFp, "\n");
					}
				}
			}
			dstData += dstStride;
		}
	}
}




