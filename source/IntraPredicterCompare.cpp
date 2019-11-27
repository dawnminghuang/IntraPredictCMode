#include "IntraPredicterCompare.h"

IntraPredicterCompare::IntraPredicterCompare(int CompareType, int ComputeType) {
	LOG("Compare starting CompareType:%d ............. \n", CompareType);
	srcData = new SrcData(0);
	protocol = NULL;
	compareType = COMPARE_TYPE_HW;
	computeType = COMPUTE_TYPE_LUMA;
}

void IntraPredicterCompare::pixelComPare(char *value) {
	srcData->initSrcData(value);
	protocol = value;
	if (strcmp(value, "avs2") == 0) {
		avs2PixelComPare();
	}
	else if (strcmp(value, "hevc") == 0) {
		hevcPixelComPare();
	}
	else if (strcmp(value, "h264") == 0) {
		h264PixelComPare();
	}
	else if (strcmp(value, "vp9") == 0) {
		vp9PixelComPare();
	}
	delete srcData;
}

void IntraPredicterCompare::avs2PixelComPare(){
	AVS2Predicter *avsPredicter = new AVS2Predicter();
	avsPredicter->setPredictSrcData(srcData);
	AVS2PredicterCMode *avsPredicterCMode = new AVS2PredicterCMode();
	avsPredicterCMode->setPredictSrcData(srcData);
	AVS2PredicterHW* avsPredicterHw = new AVS2PredicterHW();
	avsPredicterHw->setPredictSrcData(srcData);
	int mode_number = NUM_INTRA_PMODE_AVS;
	int max_cu_size = 64;
	for (int i = 0; i < NUM_MODE_INTRA_AVS; i++) {
		int uiDirMode = g_prdict_mode_avs[i];
		for (int j = 0; j < NUM_CU_PMODE_AVS; j++) {
			int tu_width = g_cu_size_avs[j][0];
			int tu_height = g_cu_size_avs[j][1];
			LOG("compare starting protocol:%s, mode:%d, [w:%d h:%d]\n", protocol, uiDirMode, tu_width, tu_height);
			DistanceData* distanMatri = NULL;
			if (computeType == COMPUTE_TYPE_ADI) {
				distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
				avs2InitDst(avsPredicter, avsPredicterHw, avsPredicterCMode, tu_width, tu_height);
				avsPredicter->predIntraAngAdi(distanMatri, uiDirMode);
				avsPredicterCMode->xPredIntraAngAdi(avsPredicterCMode->pSrc, avsPredicterCMode->avs_dst, uiDirMode, tu_width, tu_height);
				avsPredicterHw->PredIntraAngAdi(distanMatri, uiDirMode);
			}
			else if (computeType == COMPUTE_TYPE_LUMA) {
				distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
				avs2InitDst(avsPredicter, avsPredicterHw, avsPredicterCMode, tu_width, tu_height);
				avsPredicter->predIntraLumaAdi(distanMatri, uiDirMode);
				avsPredicterCMode->predIntraLumaAdi(distanMatri, uiDirMode);
				avsPredicterHw->predIntraLumaAdi(distanMatri, uiDirMode);

			}
			else if (computeType == COMPUTE_TYPE_CHROME) {
				tu_width = tu_width/2;
				tu_height = tu_height / 2;
				distanMatri = new DistanceData(tu_width, tu_height, NUM_DISTANCE_SIZE_AVS);
				avs2InitDst(avsPredicter, avsPredicterHw, avsPredicterCMode, tu_width, tu_height);
				avsPredicter->predIntraChromaAdi(distanMatri, uiDirMode);
				avsPredicterCMode->predIntraChromaAdi(distanMatri, uiDirMode);
				avsPredicterHw->predIntraChromaAdi(distanMatri, uiDirMode);
			}

			if (compareType == COMPARE_TYPE_HW) {
				avs2DstPixelComPare(avsPredicterHw,avsPredicterCMode);
			}else {
				avs2DstPixelComPare(avsPredicter, avsPredicterCMode);
			}
			avsPredicter->deinitDstData();
			avsPredicterCMode->deinitDstData();
			avsPredicterHw->deinitDstData();
			if(distanMatri)
			    delete distanMatri;
		}
	}
	delete avsPredicter;
	delete avsPredicterHw;
	delete avsPredicterCMode;
	LOG("compare ending protocol:%s \n", protocol);
}
void IntraPredicterCompare::avs2InitDst(AVS2Predicter *avsPredicter, AVS2PredicterHW  *avsPredicterHw, AVS2PredicterCMode  *avsPredicterCMode, int tu_width, int tu_height) {
	avsPredicter->setTuSize(tu_width, tu_height);
	avsPredicterCMode->setTuSize(tu_width, tu_height);
	avsPredicterHw->setTuSize(tu_width, tu_height);
	avsPredicter->initDstData();
	avsPredicterCMode->initDstData();
	avsPredicterHw->initDstData();

}
void IntraPredicterCompare::avs2DstPixelComPare(AVS2Predicter *dst, AVS2PredicterCMode  *CModeDst) {
	int tu_width = dst->tu_width;
	int tu_height = dst->tu_height;
	for (int j = 0; j < tu_height; j++) {
		for (int i = 0; i < tu_width; i++) {
			int avs2Data = dst->avs_dst[j][i];
			int avs2CModeData = CModeDst->avs_dst[j][i];
			if (avs2Data != avs2CModeData) {
				LOG("ERROR!!!!!!!!!!!!! [i:%d,j%d]:%d, %d \n", i, j, avs2Data, avs2CModeData);
			}

		}
	}
}

void IntraPredicterCompare::avs2DstPixelComPare(AVS2PredicterHW *dst, AVS2PredicterCMode  *CModeDst) {
	int tu_width = dst->tu_width;
	int tu_height = dst->tu_height;
	for (int j = 0; j < tu_height; j++) {
		for (int i = 0; i < tu_width; i++) {
			int avs2Data = dst->avs_dst[j][i];
			int avs2CModeData = CModeDst->avs_dst[j][i];
			if (avs2Data != avs2CModeData) {
				LOG("ERROR!!!!!!!!!!!!! [j:%d,i:%d]:%d, %d \n", j, i, avs2Data, avs2CModeData);
			}

		}
	}
}

void IntraPredicterCompare::hevcPixelComPare(){
	HevcPredicter *hevcPredicter = new HevcPredicter();
	hevcPredicter->setPredictSrcData(srcData);
	HevcPredicterCMode *hevcPredicterCMode = new HevcPredicterCMode();
	hevcPredicterCMode->setPredictSrcData(srcData);
	HevcPredicterHW *hevcPredicterHw = new HevcPredicterHW();
	hevcPredicterHw->setPredictSrcData(srcData);
	int mode_number = NUM_INTRA_PMODE_HEVC;
	int max_cu_size = 64;
	for (int i = 0; i < NUM_INTRA_PMODE_HEVC; i++) {
		int uiDirMode = g_prdict_mode_hevc[i];
		hevcPredicter->computeIntraPredAngle(uiDirMode);
		hevcPredicterCMode->computeIntraPredAngle(uiDirMode);
		hevcPredicterHw->computeIntraPredAngle(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_HEVC; j++) {
			int tu_width;
			int tu_height;
			int iWidth = g_cu_size_hevc[j][0];
			int iHeight = g_cu_size_hevc[j][1];
			LOG("compare starting protocol:%s, mode:%d, [w:%d h:%d]\n", protocol, uiDirMode, iWidth, iHeight);
			DistanceData* distanMatri = NULL;
			if (computeType == COMPUTE_TYPE_ADI) {
				iWidth = g_cu_size_hevc[j][0];
			    iHeight = g_cu_size_hevc[j][1];
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
				hevcInitDst(hevcPredicter, hevcPredicterHw, hevcPredicterCMode,tu_width, tu_height);
				hevcPredicter->predIntraAngAdi(distanMatri, uiDirMode);
				hevcPredicterCMode->predIntraAngAdi(distanMatri, uiDirMode);
				hevcPredicterHw->predIntraAngAdi(distanMatri, uiDirMode);
			}
			if (computeType == COMPUTE_TYPE_LUMA) {
				iWidth = g_cu_size_hevc[j][0];
				iHeight = g_cu_size_hevc[j][1];
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
				hevcInitDst(hevcPredicter, hevcPredicterHw, hevcPredicterCMode, tu_width, tu_height);
				hevcPredicter->predIntraLumaAdi(distanMatri, uiDirMode);
				hevcPredicterCMode->predIntraLumaAdi(distanMatri, uiDirMode);
				hevcPredicterHw->predIntraLumaAdi(distanMatri, uiDirMode);
			} if (computeType == COMPUTE_TYPE_CHROME) {
				iWidth = g_cu_size_hevc[j][0]/2;
				iHeight = g_cu_size_hevc[j][1]/2;
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_HEVC);
				hevcInitDst(hevcPredicter, hevcPredicterHw, hevcPredicterCMode, tu_width, tu_height);
				hevcPredicter->predIntraChromaAdi(distanMatri, uiDirMode);
				hevcPredicterCMode->predIntraChromaAdi(distanMatri, uiDirMode);
				hevcPredicterHw->predIntraChromaAdi(distanMatri, uiDirMode);
			}
			if (compareType == COMPARE_TYPE_HW) {
				hevcDstPixelComPare(hevcPredicterHw, hevcPredicterCMode);;
			}else {
				hevcDstPixelComPare(hevcPredicter, hevcPredicterCMode);
			}
			hevcPredicter->deinitDstData();
			hevcPredicterCMode->deinitDstData();
			hevcPredicterHw->deinitDstData();

			if(distanMatri)
			    delete distanMatri;
		}
	}
	delete hevcPredicter;
	delete hevcPredicterHw;
	delete hevcPredicterCMode;
	LOG("compare ending protocol:%s \n", protocol);
}

void IntraPredicterCompare::hevcInitDst(HevcPredicter *hevcPredicter, HevcPredicterHW  *hevcPredicterHw, HevcPredicterCMode  *hevcPredicterCMode, int tu_width, int tu_height) {
	hevcPredicter->setTuSize(tu_width, tu_height);
	hevcPredicterCMode->setTuSize(tu_width, tu_height);
	hevcPredicterHw->setTuSize(tu_width, tu_height);
	hevcPredicter->initDstData();
	hevcPredicterCMode->initDstData();
	hevcPredicterHw->initDstData();

}
void IntraPredicterCompare::hevcDstPixelComPare(HevcPredicter *dst, HevcPredicterCMode  *CModeDst) {
	int dstStride = MAX_CU_SIZE_HEVC;

	if (dst && CModeDst) {
		int *dstData = dst->hevc_dst;
		int *dstCModeData = CModeDst->hevc_dst;
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int data = dstData[i];
				int CModeData = dstCModeData[i];
				if (data != CModeData) {
					LOG("ERROR!!!!!!!!!!!!![j:%d,i:%d]:%d, %d \n", j, i, data, CModeData);
				}
			}
			dstData += dstStride;
			dstCModeData += dstStride;
		}
	}
}

void IntraPredicterCompare::hevcDstPixelComPare(HevcPredicterHW *dst, HevcPredicterCMode  *CModeDst) {
	int dstStride = MAX_CU_SIZE_HEVC;

	if (dst && CModeDst) {
		int *dstData = dst->hevc_dst;
		int *dstCModeData = CModeDst->hevc_dst;
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int data = dstData[i];
				int CModeData = dstCModeData[i];
				if (data != CModeData) {
					LOG("ERROR!!!!!!!!!!!!![j:%d,i:%d]:%d, %d \n", j, i, data, CModeData);
				}
			}
			dstData += dstStride;
			dstCModeData += dstStride;
		}
	}
}

void IntraPredicterCompare::h264PixelComPare(){
	H264Predicter *h264Predicter = new H264Predicter();
	h264Predicter->setPredictSrcData(srcData);
	H264PredicterCMode *h264PredicterCMode = new H264PredicterCMode();
	h264PredicterCMode->setPredictSrcData(srcData);
	H264PredicterHW *h264PredicterHw = new H264PredicterHW();
	h264PredicterHw->setPredictSrcData(srcData);
	int mode_max_index = NUM_INTRA_PMODE_264 + START_INDEX_264;
	int max_cu_size = 64;
	for (int i = 0; i < NUM_INTRA_PMODE_264; i++) {
		int uiDirMode = g_prdict_mode_264[i];
		for (int j = 0; j < NUM_CU_SIZE_264; j++) {
			int iWidth = g_cu_size_264[j][0];
			int iHeight = g_cu_size_264[j][1];
			int tu_width;
			int tu_height;
			LOG("compare starting protocol:%s, mode:%d, [w:%d h:%d]\n", protocol, uiDirMode, iWidth, iHeight);
			DistanceData* distanMatri = NULL;
			if (computeType == COMPUTE_TYPE_ADI) {
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
				h264InitDst(h264Predicter, h264PredicterHw, h264PredicterCMode, tu_width, tu_height);
				h264Predicter->predIntraAngAdi(distanMatri, uiDirMode);
				h264PredicterCMode->predIntraAngAdi(distanMatri, uiDirMode);
				h264PredicterHw->predIntraAngAdi(distanMatri, uiDirMode);
			}else if (computeType == COMPUTE_TYPE_LUMA) {
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
				h264InitDst(h264Predicter, h264PredicterHw, h264PredicterCMode, tu_width, tu_height);
				h264Predicter->predIntraLumaAdi(distanMatri, uiDirMode);
				h264PredicterCMode->predIntraLumaAdi(distanMatri, uiDirMode);
				h264PredicterHw->predIntraLumaAdi(distanMatri, uiDirMode);
			}
			else if (computeType == COMPUTE_TYPE_CHROME) {
				iWidth = g_cu_size_264[j][0] / 2;
				iHeight = g_cu_size_264[j][1] / 2;
				tu_width = iWidth;
				tu_height = iHeight;

				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
				h264InitDst(h264Predicter, h264PredicterHw, h264PredicterCMode, tu_width, tu_height);
				h264Predicter->predIntraChromaAdi(distanMatri, uiDirMode);
				h264PredicterCMode->predIntraChromaAdi(distanMatri, uiDirMode);
				h264PredicterHw->predIntraChromaAdi(distanMatri, uiDirMode);
			}
			if (compareType == COMPARE_TYPE_HW) {
				h264DstPixelComPare(h264PredicterHw, h264PredicterCMode);
			}else {
				h264DstPixelComPare(h264Predicter, h264PredicterCMode);
			}
			h264Predicter->deinitDstData();
			h264PredicterCMode->deinitDstData();
			h264PredicterHw->deinitDstData();
			if(distanMatri)
			    delete distanMatri;
		}
	}
	delete h264Predicter;
	delete h264PredicterHw;
	delete h264PredicterCMode;
	LOG("compare ending protocol:%s \n", protocol);
}

void IntraPredicterCompare::h264InitDst(H264Predicter *h264Predicter, H264PredicterHW  *h264PredicterHw, H264PredicterCMode  *h264PredicterCMode, int tu_width, int tu_height) {
	h264Predicter->setTuSize(tu_width, tu_height);
	h264PredicterCMode->setTuSize(tu_width, tu_height);
	h264PredicterCMode->setBlockSize(tu_width);
	h264PredicterHw->setTuSize(tu_width, tu_height);
	h264Predicter->initDstData();
	h264PredicterCMode->initDstData();
	h264PredicterHw->initDstData();

}
void IntraPredicterCompare::h264DstPixelComPare(H264Predicter *dst, H264PredicterCMode  *CModeDst) {
	if (dst && CModeDst) {
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int dstData = dst->h264_dst[j][i];
				int CModeDstData = CModeDst->h264_dst[j][i];
				if (dstData != CModeDstData) {
					LOG("ERROR!!!!!!!!!!!!![j:%d,i:%d]:%d, %d \n", j, i, dstData, CModeDstData);
				}
			}
		}
	}
}

void IntraPredicterCompare::h264DstPixelComPare(H264PredicterHW *dst, H264PredicterCMode  *CModeDst) {
	if (dst && CModeDst) {
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int dstData = dst->h264_dst[j][i];
				int CModeDstData = CModeDst->h264_dst[j][i];
				if (dstData != CModeDstData) {
					LOG("ERROR!!!!!!!!!!!!![i:%d,j%d]:%d, %d \n", i, j, dstData, CModeDstData);
				}
			}
		}
	}
}
void IntraPredicterCompare::vp9PixelComPare(){
	Vp9Predicter *vp9Predicter = new Vp9Predicter();
	vp9Predicter->setPredictSrcData(srcData);
	Vp9PredicterCMode *vp9PredicterCMode = new Vp9PredicterCMode();
	vp9PredicterCMode->setPredictSrcData(srcData);
	Vp9PredicterHW *vp9PredicterHw = new Vp9PredicterHW();
	vp9PredicterHw->setPredictSrcData(srcData);
	int mode_number = NUM_INTRA_PMODE_VP9 + START_INDEX_VP9;
	int max_cu_size = 64;
	computeType = COMPUTE_TYPE_CHROME;
	for (int i = 0; i < NUM_INTRA_PMODE_VP9; i++) {
		int uiDirMode = g_prdict_mode_vp9[i];
		for (int j = 0; j < NUM_CU_SIZE_VP9; j++) {
			int iWidth = g_cu_size_vp9[j][0];
			int iHeight = g_cu_size_vp9[j][1];
			int tu_width;
			int tu_height;
			LOG("compare starting protocol:%s, mode:%d, [w:%d h:%d]\n", protocol, uiDirMode, iWidth, iHeight);
			DistanceData* distanMatri = NULL;
			if (computeType == COMPUTE_TYPE_ADI) {
			    iWidth = g_cu_size_vp9[j][0];
				iHeight = g_cu_size_vp9[j][1];
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
				vp9InitDst(vp9Predicter, vp9PredicterHw, vp9PredicterCMode, tu_width, tu_height);
				vp9Predicter->predIntraAngAdi(distanMatri, uiDirMode);
				vp9PredicterCMode->predIntraAngAdi(distanMatri, uiDirMode);
				vp9PredicterHw->predIntraAngAdi(distanMatri, uiDirMode);
			}else if (computeType == COMPUTE_TYPE_LUMA) {
				iWidth = g_cu_size_vp9[j][0];
				iHeight = g_cu_size_vp9[j][1];
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
				vp9InitDst(vp9Predicter, vp9PredicterHw, vp9PredicterCMode, tu_width, tu_height);
				vp9Predicter->predIntraLumaAdi(distanMatri, uiDirMode);
				vp9PredicterCMode->predIntraLumaAdi(distanMatri, uiDirMode);
				vp9PredicterHw->predIntraLumaAdi(distanMatri, uiDirMode);
			
			}else if (computeType == COMPUTE_TYPE_CHROME) {
				iWidth = g_cu_size_vp9[j][0]/2;
				iHeight = g_cu_size_vp9[j][1]/2;
				tu_width = iWidth;
				tu_height = iHeight;
				distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
				vp9InitDst(vp9Predicter, vp9PredicterHw, vp9PredicterCMode, tu_width, tu_height);
				if (tu_width >= 4) {
					vp9Predicter->predIntraLumaAdi(distanMatri, uiDirMode);
					vp9PredicterCMode->predIntraLumaAdi(distanMatri, uiDirMode);
					vp9PredicterHw->predIntraLumaAdi(distanMatri, uiDirMode);
				}

			}
			if (compareType == COMPARE_TYPE_HW) {
				vp9DstPixelComPare(vp9PredicterHw, vp9PredicterCMode);
			}else {
				vp9DstPixelComPare(vp9Predicter, vp9PredicterCMode);
			}
			vp9PredicterCMode->deinitDstData();
			vp9Predicter->deinitDstData();
			vp9Predicter->deinitVp9Matri();
			vp9PredicterHw->deinitDstData();
			vp9PredicterHw->deinitVp9Matri();
			if(distanMatri)
			    delete distanMatri;
		}
	}
	delete vp9Predicter;
	delete vp9PredicterCMode;
	delete vp9PredicterHw;
	LOG("compare ending protocol:%s \n", protocol);
}

void IntraPredicterCompare::vp9InitDst(Vp9Predicter *vp9Predicter, Vp9PredicterHW  *vp9PredicterHw, Vp9PredicterCMode  *vp9PredicterCMode, int tu_width, int tu_height) {
	vp9Predicter->setTuSize(tu_width, tu_height);
	vp9Predicter->initVp9Matri(tu_width, tu_height, NUM_DISTANCE_SIZE_VP9);
	vp9PredicterCMode->setTuSize(tu_width, tu_height);
	vp9PredicterCMode->setStride(tu_height);
	vp9PredicterHw->setTuSize(tu_width, tu_height);
	vp9Predicter->initDstData();
	vp9PredicterCMode->initDstData();
	vp9PredicterHw->initDstData();
	vp9PredicterHw->initVp9Matri(tu_width, tu_height, NUM_DISTANCE_SIZE_VP9);

}
void IntraPredicterCompare::vp9DstPixelComPare(Vp9Predicter *dst, Vp9PredicterCMode  *CModeDst) {
	if (dst && CModeDst) {
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		int dstStride = tu_width;
		uint8_t *CModeData = CModeDst->vp9_dst;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int dstData = dst->vp9_dst[j][i];
				uint8_t CModeDstData = CModeData[i];
				if (dstData != CModeDstData) {
					LOG("ERROR!!!!!!!!!!!!! [j:%d,i:%d]:%d, %d \n", j, i, dstData, CModeDstData);
				}
			}
			CModeData += dstStride;
		}
	}
}

void IntraPredicterCompare::vp9DstPixelComPare(Vp9PredicterHW *dst, Vp9PredicterCMode  *CModeDst) {
	if (dst && CModeDst) {
		int tu_width = dst->tu_width;
		int tu_height = dst->tu_height;
		int dstStride = tu_width;
		uint8_t *CModeData = CModeDst->vp9_dst;
		for (int j = 0; j < tu_height; j++) {
			for (int i = 0; i < tu_width; i++) {
				int dstData = dst->vp9_dst[j][i];
				uint8_t CModeDstData = CModeData[i];
				if (dstData != CModeDstData) {
					LOG("ERROR!!!!!!!!!!!!! [i:%d,j%d]:%d, %d \n", i, j, dstData, CModeDstData);
				}
			}
			CModeData += dstStride;
		}
	}
}

IntraPredicterCompare::~IntraPredicterCompare() {

}



