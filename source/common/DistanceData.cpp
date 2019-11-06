#include"DistanceData.h"

DistanceData::DistanceData(int width, int height, int size){
    tu_width = width;
    tu_height  = height;
    distance_size = size;
    distance_matri = new int**[tu_height];
    for (int j = 0; j < tu_height; j++) {
        distance_matri[j] = new int*[tu_width];
        for (int i = 0; i < tu_width;i++) {
            distance_matri[j][i] = new int[distance_size]();
        }
    }

}

void DistanceData::initDistanceMatri(){
    distance_matri = new int**[tu_height];
    for (int j = 0; j < tu_height; j++) {
        distance_matri[j] = new int*[tu_width];
        for (int i = 0; i < tu_width;i++) {
            distance_matri[j][i] = new int[distance_size]();
        }
    }

}

DistanceData::~DistanceData(){
	for (int j = 0; j < tu_height; ++j) {
		for (int i = 0; i < tu_width; ++i) {
			delete [] distance_matri[j][i];
		}
		delete [] distance_matri[j];
	}
	delete [] distance_matri;
}



