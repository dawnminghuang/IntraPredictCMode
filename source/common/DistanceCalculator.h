#ifndef  __DISTANCECALCULATOR__
#define  __DISTANCECALCULATOR__
#include <math.h>
#include "DistanceData.h"
#include "OutputWriter.h"
#define  CALCURATIO 4

enum CalcuMode
{
	CALCU_MODE_ROW = 0,
	CALCU_MODE_COL = 1,
	CALCU_MODE_MATRI = 2,
	CALCU_MODE_MATRI_4X2 = 3,
	CALCU_MODE_MATRI_2X4 = 4,
	CALCU_MODE_ALL = 5,
};

class DistanceCalculator {
public:
	DistanceCalculator();
	~DistanceCalculator();
public:
	void initDistanceCalculator(int modeNumber, int maxCUsize, int calcMode);
	void calcuDistance(DistanceData* distanceMatri);
	void setPredictMode(int mode);
	void setCalcuMode(int mode);
	void setOutPutWriter(OutputWriter* outPutWriter);
private:
	void initCalcuParas();
	void initCalcuMatri(int width, int height);
	void deinitCalcuMatri(int width, int height);
	void calcuDistanceCol(DistanceData* distanceMatri);
	void calcuDistanceRow(DistanceData* distanceMatri);
	void calcuDistanceMatri(DistanceData* distanceMatri);
	void calcuDistanceMatri4X2(DistanceData* distanceMatri);
	void calcuDistanceMatri2X4(DistanceData* distanceMatri);
	void calcuMinMaxPosition();
	int calcMin(int** data, int width, int height);
	int calcMax(int** data, int width, int height);
	void isMinMaxDiag(int position, int calcRatio, bool *isDiag);
	void writeDiagPostionToFile();
public:
	int calcu_mode;
	int predict_mode;
	int max_point_number;
	int mode_number;
	int *col_max_distance;
	int *row_max_distance;
	int *matri_max_distance;
	int *matri4X2_max_distance;
	int *cu_distance;
	int *pos_x;
	int *pos_y;
	int **calc_matri;
	int *calc_matri_dig;
	int **min_max_posiont;
	int matri_max;
	int matri_min;
	int distance_size;
	int tu_width;
	int tu_height;
	int calc_pixel_number;
	bool is_min_diag;
	bool is_max_diag;
	bool is_diag;
	OutputWriter*   output_writer;
};

#endif


