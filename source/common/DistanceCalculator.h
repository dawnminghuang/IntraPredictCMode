#ifndef  __DISTANCECALCULATOR__
#define  __DISTANCECALCULATOR__
#include <math.h>
#include "DistanceData.h"

#define  CALCURATIO 4

enum CalcuMode
{
  CALCU_MODE_ROW       = 0,
  CALCU_MODE_COL      = 1,
  CALCU_MODE_MATRI      = 2,
  CALCU_MODE_MATRI_4X2 = 3,
  CALCU_MODE_ALL = 4,
};

class DistanceCalculator{
public:
    DistanceCalculator();
    ~DistanceCalculator();
 public:
    void initDistanceCalculator(int modeNumber, int maxCUsize, int calcMode);
    void calcuDistance(DistanceData* distanceMatri);
    void setPredictMode(int mode);
    void setCalcuMode(int mode);
private:
    void initCalcuParas();
    void initCalcuMatri(int width, int height);
	void deinitCalcuMatri(int **calc_matri, int width, int height);
    void calcuDistanceCol(DistanceData* distanceMatri);
    void calcuDistanceRow(DistanceData* distanceMatri);
    void calcuDistanceMatri(DistanceData* distanceMatri);
    void calcuDistanceMatri4X2(DistanceData* distanceMatri);
    int calcMin(int** data, int width, int height);
    int calcMax(int** data, int width, int height);
    
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
};

#endif


