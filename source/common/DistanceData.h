#ifndef __DISTANCEDATA__
#define __DISTANCEDATA__
#include <stdio.h>

class DistanceData{
 public:
     DistanceData(int width, int height, int size);
     ~DistanceData();
     void initDistanceMatri();
 public:
     int tu_height;
     int tu_width;
     int distance_size; 
     int  ***distance_matri;
};
#endif