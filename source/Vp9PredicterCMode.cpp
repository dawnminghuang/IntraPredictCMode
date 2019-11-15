#include "Vp9PredicterCMode.h"


Vp9PredicterCMode::Vp9PredicterCMode() {
	point_number = 0;
	bs = 0;
	stride = 0;
}

Vp9PredicterCMode::~Vp9PredicterCMode() {

}

void Vp9PredicterCMode::predict() {
	int mode_number = NUM_INTRA_PMODE_VP9 + START_INDEX_VP9;
	int max_cu_size = 64;
	generateOutPath(VP9_PATH, calc_mode);
	distanceCalculator->initDistanceCalculator(mode_number, max_cu_size, calc_mode);
	for (int i = 0; i < NUM_INTRA_PMODE_VP9; i++) {
		int uiDirMode = g_prdict_mode_vp9[i];
		outPutWriter->initDstDataFp(VP9_DATA_PATH_CMODE, uiDirMode);
		distanceCalculator->setPredictMode(uiDirMode);
		for (int j = 0; j < NUM_CU_SIZE_VP9; j++) {
			int iWidth = g_cu_size_vp9[j][0];
			int iHeight = g_cu_size_vp9[j][1];
			tu_width = iWidth;
			tu_height = iHeight;
			stride = tu_width;
			bs = tu_width;
			initDstData();
			DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_VP9);
			predIntraAngAdi(distanMatri, uiDirMode);
			distanceCalculator->calcuDistance(distanMatri);
			outPutWriter->writeDstDataToFile(vp9_dst, iWidth, iHeight, stride);
			deinitDstData();
			delete distanMatri;
		}
	}
}

void Vp9PredicterCMode::d207_predictor(uint8_t *above, uint8_t *left) {
	int r, c;
	(void)above;
	// first column
	uint8_t *dst = vp9_dst;
	for (r = 0; r < bs - 1; ++r) dst[r * stride] = AVG2(left[r], left[r + 1]);
	dst[(bs - 1) * stride] = left[bs - 1];
	dst++;

	// second column
	for (r = 0; r < bs - 2; ++r)
		dst[r * stride] = AVG3(left[r], left[r + 1], left[r + 2]);
	dst[(bs - 2) * stride] = AVG3(left[bs - 2], left[bs - 1], left[bs - 1]);
	dst[(bs - 1) * stride] = left[bs - 1];
	dst++;

	// rest of last row
	for (c = 0; c < bs - 2; ++c) dst[(bs - 1) * stride + c] = left[bs - 1];

	for (r = bs - 2; r >= 0; --r)
		for (c = 0; c < bs - 2; ++c)
			dst[r * stride + c] = dst[(r + 1) * stride + c - 2];
}

void Vp9PredicterCMode::d63_predictor(uint8_t *above, uint8_t *left) {
	int r, c;
	int size;
	(void)left;
	uint8_t *dst = vp9_dst;
	for (c = 0; c < bs; ++c) {
		dst[c] = AVG2(above[c], above[c + 1]);
		dst[stride + c] = AVG3(above[c], above[c + 1], above[c + 2]);
	}
	for (r = 2, size = bs - 2; r < bs; r += 2, --size) {
		memcpy(dst + (r + 0) * stride, dst + (r >> 1), size);
		memset(dst + (r + 0) * stride + size, above[bs - 1], (bs - size));
		memcpy(dst + (r + 1) * stride, dst + stride + (r >> 1), size);
		memset(dst + (r + 1) * stride + size, above[bs - 1], (bs - size));
	}
}

void Vp9PredicterCMode::d45_predictor(uint8_t *above, uint8_t *left) {
	uint8_t *dst = vp9_dst;
	const uint8_t above_right = above[bs - 1];
	const uint8_t *const dst_row0 = dst;
	int x, size;
	(void)left;

	for (x = 0; x < bs - 1; ++x) {
		dst[x] = AVG3(above[x], above[x + 1], above[x + 2]);
	}
	dst[bs - 1] = above_right;
	dst += stride;
	for (x = 1, size = bs - 2; x < bs; ++x, --size) {
		memcpy(dst, dst_row0 + x, size);
		memset(dst + size, above_right, (x + 1));
		dst += stride;
	}
}

void Vp9PredicterCMode::d117_predictor(uint8_t *above, uint8_t *left) {
	int r, c;
	uint8_t *dst = vp9_dst;
	// first row
	for (c = 0; c < bs; c++) dst[c] = AVG2(above[c - 1], above[c]);
	dst += stride;

	// second row
	dst[0] = AVG3(left[0], above[-1], above[0]);
	for (c = 1; c < bs; c++) dst[c] = AVG3(above[c - 2], above[c - 1], above[c]);
	dst += stride;

	// the rest of first col
	dst[0] = AVG3(above[-1], left[0], left[1]);
	for (r = 3; r < bs; ++r)
		dst[(r - 2) * stride] = AVG3(left[r - 3], left[r - 2], left[r - 1]);

	// the rest of the block
	for (r = 2; r < bs; ++r) {
		for (c = 1; c < bs; c++) dst[c] = dst[-2 * stride + c - 1];
		dst += stride;
	}
}

void Vp9PredicterCMode::d135_predictor(uint8_t *above, uint8_t *left) {
	int i;
	uint8_t *dst = vp9_dst;
	uint8_t border[32 + 32 - 1];  // outer border from bottom-left to top-right

  // dst(bs, bs - 2)[0], i.e., border starting at bottom-left
	for (i = 0; i < bs - 2; ++i) {
		border[i] = AVG3(left[bs - 3 - i], left[bs - 2 - i], left[bs - 1 - i]);
	}
	border[bs - 2] = AVG3(above[-1], left[0], left[1]);
	border[bs - 1] = AVG3(left[0], above[-1], above[0]);
	border[bs - 0] = AVG3(above[-1], above[0], above[1]);
	// dst[0][2, size), i.e., remaining top border ascending
	for (i = 0; i < bs - 2; ++i) {
		border[bs + 1 + i] = AVG3(above[i], above[i + 1], above[i + 2]);
	}

	for (i = 0; i < bs; ++i) {
		memcpy(dst + i * stride, border + bs - 1 - i, bs);
	}
}

void Vp9PredicterCMode::d153_predictor(uint8_t *above, uint8_t *left) {
	int r, c;
	uint8_t *dst = vp9_dst;
	dst[0] = AVG2(above[-1], left[0]);
	for (r = 1; r < bs; r++) dst[r * stride] = AVG2(left[r - 1], left[r]);
	dst++;

	dst[0] = AVG3(left[0], above[-1], above[0]);
	dst[stride] = AVG3(above[-1], left[0], left[1]);
	for (r = 2; r < bs; r++)
		dst[r * stride] = AVG3(left[r - 2], left[r - 1], left[r]);
	dst++;

	for (c = 0; c < bs - 2; c++)
		dst[c] = AVG3(above[c - 1], above[c], above[c + 1]);
	dst += stride;

	for (r = 1; r < bs; ++r) {
		for (c = 0; c < bs - 2; c++) dst[c] = dst[-stride + c - 2];
		dst += stride;
	}
}
void Vp9PredicterCMode::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode) {
	int iWidth = distanMatri->tu_width;
	int iHeight = distanMatri->tu_height;
	uint8_t  Above[2 * MAX_CU_SIZE_VP9 + 1];
	uint8_t  Left[MAX_CU_SIZE_VP9 + 1];
	convertSrc(Above, Left);
	uint8_t *refAbove = Above + 1;
	uint8_t *refLeft = Left + 1;
	int delta_pos = 0;
	int iXnN1 = 0;
	int iX = 0;
	int iXn = 0;
	int iXnP2 = 0;
	if (uiDirMode == 1) {
		d207_predictor(refAbove, refLeft);
	}
	if (uiDirMode == 2) {
		d63_predictor(refAbove, refLeft);
	}
	if (uiDirMode == 3) {
		d45_predictor(refAbove, refLeft);
	}
	if (uiDirMode == 4) {
		d117_predictor(refAbove, refLeft);
	}
	if (uiDirMode == 5) {
		d135_predictor(refAbove, refLeft);
	}
	if (uiDirMode == 6) {
		d153_predictor(refAbove, refLeft);
	}
}

void Vp9PredicterCMode::convertSrc(uint8_t* above, uint8_t *left) {
	if ((src_data && src_data->vp9_src)) {
		int src_stride = MAX_CU_SIZE_VP9 * NUM_EXTEND_SIZE_VP9 + 1;
		int max_width = MAX_CU_SIZE_VP9 * NUM_EXTEND_SIZE_VP9 + 1;
		for (int i = 0; i < bs; i++) {
			above[i] = src_data->vp9_src[i];
		}
		for (int i = bs; i < max_width; i++) {
			above[i] = src_data->vp9_src[bs];
		}
		for (int i = 0; i <= MAX_CU_SIZE_VP9; i++) {
			left[i] = src_data->vp9_src[i*src_stride];
		}
	}
}

void Vp9PredicterCMode::setStride(int dstStride) {
	stride = dstStride;
	bs = dstStride;
}

void Vp9PredicterCMode::initDstData() {
	int dstOutNumber = tu_height * tu_height;
	vp9_dst = new uint8_t[dstOutNumber]();
}

void Vp9PredicterCMode::deinitDstData() {
	if (vp9_dst) {
		delete[] vp9_dst;
		vp9_dst = NULL;
	}
}



