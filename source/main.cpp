#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common/SrcData.h"
#include "common/Log.h"
#include "IntraPredicter.h"
#include "Avs2Predicter.h"
#include "Avs2PredicterCMode.h"
#include "HevcPredicter.h"
#include "HevcPredicterCMode.h"
#include "H264Predicter.h"
#include "H264PredicterCMode.h"
#include "Vp9Predicter.h"
#include "Vp9PredicterCMode.h"
#include "IntraPredicterCompare.h"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define TEST_PROTOCOL "avs2"
#define TEST_CALC_METHOD 1
#define SRC_GENERATE_MODE   0  
#define PROTOCOL_NUMBER 4
#define TEST_TIMES 100
#define CALC_NUMBER 5
#define PROCESS_ALL 
char protocolName[PROTOCOL_NUMBER][MAX_PATH_LENGHT] = { "avs2", "hevc","h264" ,"vp9"};
//char protocolName[PROTOCOL_NUMBER][MAX_PATH_LENGHT] = { "h264" ,"vp9" };
int calcMethod[CALC_NUMBER] = { CALCU_MODE_ROW ,CALCU_MODE_COL , CALCU_MODE_MATRI,CALCU_MODE_MATRI_4X2 ,CALCU_MODE_MATRI_2X4 };
IntraPredicter* GetIntraPredicter(char *protocol);
IntraPredicter* GetIntraPredicterCMode(char *protocol);
void singlePredictProcedure(char *protocol, int calcMethod);

int main(int argc, char* argv[]) {
	int returnCode = EXIT_SUCCESS;
	for (int k = 0; k < TEST_TIMES; k++) {
#ifdef PROCESS_ALL
		for (int i = 0; i < PROTOCOL_NUMBER; i++) {
#ifdef CALC_DISTANCE
			for (int j = 0; j < CALC_NUMBER; j++) {
				printf("%s  protocol:%s, method:%d, processing......\n", logTime(), protocolName[i], calcMethod[j]);
				singlePredictProcedure(protocolName[i], calcMethod[j]);
			}
#else
			IntraPredicterCompare* intraPredicterCompare = new IntraPredicterCompare();
			intraPredicterCompare->pixelComPare(protocolName[i]);
#endif 
		}
#else
		singlePredictProcedure(TEST_PROTOCOL, TEST_CALC_METHOD);
#endif
	}

}


void singlePredictProcedure(char *protocol, int calcMethod) {
	SrcData *srcData = new SrcData(SRC_GENERATE_MODE);
	srcData->initSrcData(protocol);
	IntraPredicter* intraPredicter = GetIntraPredicter(protocol);
	intraPredicter->setPredictMode(calcMethod);
	intraPredicter->setPredictSrcData(srcData);
	intraPredicter->predict();
	IntraPredicter* intraPredicterCMode = GetIntraPredicterCMode(protocol);
	intraPredicterCMode->setPredictSrcData(srcData);
	intraPredicterCMode->predict();
	delete intraPredicter;
	delete intraPredicterCMode;
	delete srcData;
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