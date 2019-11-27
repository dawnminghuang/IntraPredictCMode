#ifndef  __INTRAPREDICERTCOMPARE__
#define  __INTRAPREDICERTCOMPARE__
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "common/SrcData.h"
#include "common/Log.h"
#include "Avs2Predicter.h"
#include "Avs2PredicterCMode.h"
#include "Avs2PredicterHW.h"
#include "HevcPredicter.h"
#include "HevcPredicterCMode.h"
#include "HevcPredicterHW.h"
#include "H264Predicter.h"
#include "H264PredicterCMode.h"
#include "H264PredicterHW.h"
#include "Vp9Predicter.h"
#include "Vp9PredicterCMode.h"
#include "Vp9PredicterHW.h"
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

enum CompareType {
	COMPARE_TYPE_HW = 0,
	COMPARE_TYPE_SF = 1
};

enum ComputeType {
	COMPUTE_TYPE_ADI = 0,
	COMPUTE_TYPE_LUMA = 1,
	COMPUTE_TYPE_CHROME = 2
};
class IntraPredicterCompare {
public:
	IntraPredicterCompare(int CompareType, int ComputeType);
	~IntraPredicterCompare();
public:
	void pixelComPare(char *protocol);
	void avs2PixelComPare();
	void hevcPixelComPare();
	void h264PixelComPare();
	void vp9PixelComPare();
	void avs2InitDst(AVS2Predicter *dst, AVS2PredicterHW  *HwDst, AVS2PredicterCMode  *CModeDst, int tu_width, int tu_height);
	void avs2DstPixelComPare(AVS2Predicter *dst, AVS2PredicterCMode  *CModeDst);
	void avs2DstPixelComPare(AVS2PredicterHW *dst, AVS2PredicterCMode  *CModeDst);
	void hevcInitDst(HevcPredicter *hevcPredicter, HevcPredicterHW  *hevcPredicterHw, HevcPredicterCMode  *hevcPredicterCMode, int tu_width, int tu_height);
	void hevcDstPixelComPare(HevcPredicter *dst, HevcPredicterCMode  *CModeDst);
	void hevcDstPixelComPare(HevcPredicterHW *dst, HevcPredicterCMode  *CModeDst);
	void h264InitDst(H264Predicter *h264Predicter, H264PredicterHW  *h264PredicterHw, H264PredicterCMode  *h264PredicterCMode, int tu_width, int tu_height);
	void h264DstPixelComPare(H264Predicter *dst, H264PredicterCMode  *CModeDst);
	void h264DstPixelComPare(H264PredicterHW *dst, H264PredicterCMode  *CModeDst);
	void vp9InitDst(Vp9Predicter *vp9Predicter, Vp9PredicterHW  *vp9PredicterHw, Vp9PredicterCMode  *vp9PredicterCMode, int tu_width, int tu_height);
	void vp9DstPixelComPare(Vp9Predicter *dst, Vp9PredicterCMode  *CModeDst);
	void vp9DstPixelComPare(Vp9PredicterHW *dst, Vp9PredicterCMode  *CModeDst);
public:
	SrcData *srcData;
	char *protocol;
	int compareType;
	int computeType;
};
#endif

