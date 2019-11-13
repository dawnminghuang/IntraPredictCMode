#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common/SrcData.h"
#include "IntraPredicter.h"
#include "Avs2Predicter.h"
#include "Avs2PredicterCMode.h"
#include "HevcPredicter.h"
#include "HevcPredicterCMode.h"
#include "H264Predicter.h"
#include "H264PredicterCMode.h"
#include "Vp9Predicter.h"
#include "Vp9PredicterCMode.h"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define TEST_PROTOCOL "h264"
#define TEST_MODE   1  
IntraPredicter* GetIntraPredicter(char *protocol);
IntraPredicter* GetIntraPredicterCMode(char *protocol);

int main(int argc, char* argv[]) {
	int returnCode = EXIT_SUCCESS;
	SrcData *srcData = new SrcData(TEST_MODE);
	srcData->initSrcData(TEST_PROTOCOL);
	IntraPredicter* intraPredicter = GetIntraPredicter(TEST_PROTOCOL);
	intraPredicter->setPredictSrcData(srcData);
	intraPredicter->predict();
	IntraPredicter* intraPredicterCMode = GetIntraPredicterCMode(TEST_PROTOCOL);
	intraPredicterCMode->setPredictSrcData(srcData);
	intraPredicterCMode->predict();
	delete srcData;
	delete intraPredicter;
}
IntraPredicter* GetIntraPredicter(char *value) {
	if (strcmp(value, "avs2") == 0) {
		return new AVS2Predicter();
	}else if (strcmp(value, "hevc") == 0) {
		return new  HevcPredicter();
	}else if (strcmp(value, "h264") == 0) {
		return new  H264Predicter();
	}else if (strcmp(value, "vp9") == 0) {
		return new Vp9Predicter();
	}else {
		return new AVS2Predicter();
	}
}

IntraPredicter* GetIntraPredicterCMode(char *value) {
	if (strcmp(value, "avs2") == 0) {
		return new AVS2PredicterCMode();
	}else if (strcmp(value, "hevc") == 0) {
		return new HevcPredicterCMode();
	}else if (strcmp(value, "h264") == 0) {
		return new H264PredicterCMode();
	}else if (strcmp(value, "vp9") == 0) {
		return new Vp9PredicterCMode();
	}else {
		return new AVS2PredicterCMode();
	}

}