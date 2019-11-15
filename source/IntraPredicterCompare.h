#ifndef  __INTRAPREDICERTCOMPARE__
#define  __INTRAPREDICERTCOMPARE__
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "common/SrcData.h"
#include "common/Log.h"
#include "Avs2Predicter.h"
#include "Avs2PredicterCMode.h"
#include "HevcPredicter.h"
#include "HevcPredicterCMode.h"
#include "H264Predicter.h"
#include "H264PredicterCMode.h"
#include "Vp9Predicter.h"
#include "Vp9PredicterCMode.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
class IntraPredicterCompare {
public:
	IntraPredicterCompare();
	~IntraPredicterCompare();
public:
	void pixelComPare(char *protocol);
	void avs2PixelComPare();
	void hevcPixelComPare();
	void h264PixelComPare();
	void vp9PixelComPare();
	void avs2DstPixelComPare(AVS2Predicter *avs2Dst, AVS2PredicterCMode  *avs2CModeDst);
	void hevcDstPixelComPare(HevcPredicter *dst, HevcPredicterCMode  *CModeDst);
	void h264DstPixelComPare(H264Predicter *dst, H264PredicterCMode  *CModeDst);
	void vp9DstPixelComPare(Vp9Predicter *dst, Vp9PredicterCMode  *CModeDst);
public:
	SrcData *srcData;
	char *protocol;
};
#endif

