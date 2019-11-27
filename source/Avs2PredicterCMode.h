#ifndef __AVS2PREDICTERCMODE__
#define __AVS2PREDICTERCMODE__
#include "common/Avs2Common.h"
#include "IntraPredicter.h"

class AVS2PredicterCMode :public IntraPredicter
{

public:
	AVS2PredicterCMode();
	~AVS2PredicterCMode();


	void predict();
	void initDstData();
	void deinitDstData();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraLumaAdi(DistanceData* distanMatri, int uiDirMode);
	void predIntraChromaAdi(DistanceData* distanMatri, int uiDirMode);
	int  getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset);
	void saveDistanceMatri(DistanceData* distanMatri, int i, int j, int  iYnN1, int iY, int iYn, int iYnP2, int iX);
	void xPredIntraAngAdi(int *pSrc, int **pDst, int uiDirMode, int iWidth, int iHeight);
	void xPredIntraVertAdi(int *pSrc, int **pDst, int iWidth, int iHeight);
	void xPredIntraHorAdi(int *pSrc, int **pDst, int iWidth, int iHeight);
	void xPredIntraDCAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail,int sample_bit_depth);
	void xPredIntraPlaneAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int sample_bit_depth);
	void xPredIntraBiAdi(int *pSrc, int **pDst, int iWidth, int iHeight, int sample_bit_depth);
public:
	int **avs_dst;
	int *pSrc;
	int cu_size_log;
	int sample_bit_depth;
	int bLeftAvail;
	int bAboveAvail;
};

#endif

