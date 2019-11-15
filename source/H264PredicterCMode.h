#ifndef __H264PREDICTERCMODE__
#define __H264PREDICTERCMODE__
 
#include "IntraPredicter.h"
#include "common/DistanceCalculator.h"
#include "common/H264Common.h"

// 4x4 intra prediction modes 
typedef enum {
	VERT_PRED = 0,
	HOR_PRED = 1,
	DC_PRED = 2,
	DIAG_DOWN_LEFT_PRED = 3,
	DIAG_DOWN_RIGHT_PRED = 4,
	VERT_RIGHT_PRED = 5,
	HOR_DOWN_PRED = 6,
	VERT_LEFT_PRED = 7,
	HOR_UP_PRED = 8
} I4x4PredModes;

typedef int imgpel;

// Notation for comments regarding prediction and predictors.
// The pels of the 4x4 block are labelled a..p. The predictor pels above
// are labelled A..H, from the left I..L, and from above left X, as follows:
//
//  X A B C D E F G H
//  I a b c d
//  J e f g h
//  K i j k l
//  L m n o p
//

// Predictor array index definitions
#define P_X (PredPel[0])
#define P_A (PredPel[1])
#define P_B (PredPel[2])
#define P_C (PredPel[3])
#define P_D (PredPel[4])
#define P_E (PredPel[5])
#define P_F (PredPel[6])
#define P_G (PredPel[7])
#define P_H (PredPel[8])
#define P_I (PredPel[9])
#define P_J (PredPel[10])
#define P_K (PredPel[11])
#define P_L (PredPel[12])


// Notation for comments regarding prediction and predictors.
// The pels of the 8x8 block are labeled a..p. The predictor pels above
// are labeled A..H, from the left I..P, and from above left X, as follows:
//
//  Z  A  B  C  D  E  F  G  H  I  J  K  L  M   N  O  P
//  Q  a1 b1 c1 d1 e1 f1 g1 h1
//  R  a2 b2 c2 d2 e2 f2 g2 h2
//  S  a3 b3 c3 d3 e3 f3 g3 h3
//  T  a4 b4 c4 d4 e4 f4 g4 h4
//  U  a5 b5 c5 d5 e5 f5 g5 h5
//  V  a6 b6 c6 d6 e6 f6 g6 h6
//  W  a7 b7 c7 d7 e7 f7 g7 h7
//  X  a8 b8 c8 d8 e8 f8 g8 h8


// Predictor array index definitions
#define PP_Z (PredPel[0])
#define PP_A (PredPel[1])
#define PP_B (PredPel[2])
#define PP_C (PredPel[3])
#define PP_D (PredPel[4])
#define PP_E (PredPel[5])
#define PP_F (PredPel[6])
#define PP_G (PredPel[7])
#define PP_H (PredPel[8])
#define PP_I (PredPel[9])
#define PP_J (PredPel[10])
#define PP_K (PredPel[11])
#define PP_L (PredPel[12])
#define PP_M (PredPel[13])
#define PP_N (PredPel[14])
#define PP_O (PredPel[15])
#define PP_P (PredPel[16])
#define PP_Q (PredPel[17])
#define PP_R (PredPel[18])
#define PP_S (PredPel[19])
#define PP_T (PredPel[20])
#define PP_U (PredPel[21])
#define PP_V (PredPel[22])
#define PP_W (PredPel[23])
#define PP_X (PredPel[24])

class H264PredicterCMode :public IntraPredicter
{

public:
	H264PredicterCMode();
	~H264PredicterCMode();
	void predict();
	void predIntraAngAdi(DistanceData* distanMatri, int uiDirMode);
	void setBlockSize(int size);
	void convertSrc(int* above, int *left);
	void initDstData();
	void deinitDstData();
	void intra4x4_diag_down_left_pred(int *refAbove, int *refLeft);
	void intra4x4_diag_down_right_pred(int *refAbove, int *refLeft);
	void intra4x4_vert_right_pred(int *refAbove, int *refLeft);
	void intra4x4_vert_left_pred(int *refAbove, int *refLeft);
	void intra4x4_hor_up_pred(int *refAbove, int *refLeft);
	void intra4x4_hor_down_pred(int *refAbove, int *refLeft);
	void intra8x8_diag_down_right_pred(int *refAbove, int *refLeft);
	void intra8x8_diag_down_left_pred(int *refAbove, int *refLeft);
	void intra8x8_vert_right_pred(int *refAbove, int *refLeft);
	void intra8x8_vert_left_pred(int *refAbove, int *refLeft);
	void intra8x8_hor_up_pred(int *refAbove, int *refLeft);
	void intra8x8_hor_down_pred(int *refAbove, int *refLeft);
public:
    int max_dst_number;
	int block_size;
	int **h264_dst;

};

#endif

