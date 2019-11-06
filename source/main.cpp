#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "IntraPredicter.h"
#include "Avs2Predicter.h"
#include "HevcPredicter.h"
#include "H264Predicter.h"
#include "Vp9Predicter.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

IntraPredicter* GetIntraPredicter(char *protocol);
int main(int argc, char* argv[]) {
	int returnCode = EXIT_SUCCESS;
	IntraPredicter* intraPredicter = GetIntraPredicter("vp9");
	intraPredicter->predict();


}
IntraPredicter* GetIntraPredicter(char *value) {
	if (strcmp(value, "avs2") == 0) {
		return new AVS2Predicter();
	}
	else if (strcmp(value, "hevc") == 0) {
		return new  HevcPredicter();
	}
	else if (strcmp(value, "h264") == 0) {
		return new  H264Predicter();
	}
	else if (strcmp(value, "vp9") == 0) {
		return new Vp9Predicter();
	}
	else {
		return new AVS2Predicter();
	}
}