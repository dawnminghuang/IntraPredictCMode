#include "OutputWriter.h"
OutputWriter::OutputWriter() {
	predictMode = 0;
	modeInfoFp = NULL;
	distanceInfoFp = NULL;
	outAllFp = NULL;
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
	sprintf(mode_path, "\\mode_%d", predictMode);
	strcpy(outPutPath, path);
	strcat(outPutPath, mode_path);
	if (modeInfoFp) {
		fclose(modeInfoFp);
		modeInfoFp = NULL;
	}
	modeInfoFp = fopen(outPutPath, "w+");

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
					fprintf(modeInfoFp, "%4d", distanceData->distance_matri[j][i][1]);
					if (i == (distanceData->tu_width - 1)) {
						fprintf(modeInfoFp, "\n");
					}
				}

			}
		}
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





