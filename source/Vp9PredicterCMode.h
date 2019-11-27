#ifndef __VP9PREDICTERCMODE__
#define __VP9PREDICTERCMODE__
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/Vp9Common.h"


class Vp9PredicterCMode :public IntraPredicter
{

public:
	Vp9PredicterCMode();
	~Vp9PredicterCMode();
	void predict();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
	void d207_predictor(uint8_t *above, uint8_t *left);
	void d63_predictor(uint8_t *above, uint8_t *left);
	void d45_predictor(uint8_t *above, uint8_t *left);
	void d117_predictor(uint8_t *above, uint8_t *left);
	void d153_predictor(uint8_t *above, uint8_t *left);
	void d135_predictor(uint8_t *above, uint8_t *left);
	void v_predictor(uint8_t *above, uint8_t *left);
	void h_predictor(uint8_t *above, uint8_t *left);
	void tm_predictor(uint8_t *above, uint8_t *left);
	void convertSrc(uint8_t* above, uint8_t *left);
	void setStride(int dstStride);
	void initDstData();
	void deinitDstData();
public:
	int bs;
	uint8_t *vp9_dst;
	int point_number;
	int max_dst_number;
	int stride;
};

#endif


