#include "H264Predicter.h"


H264Predicter::H264Predicter(){

}

H264Predicter::~H264Predicter(){

}

void H264Predicter::predict(){
    int mode_max_index  = NUM_INTRA_PMODE_264 + START_INDEX_264;
    int max_cu_size = 64;
	generateOutPath(H264_PATH, calc_mode);
    distanceCalculator->initDistanceCalculator(mode_max_index, max_cu_size,calc_mode);
    for(int i = 0; i < NUM_INTRA_PMODE_264; i++){
        int uiDirMode = g_prdict_mode_264[i];
		outPutWriter->initModeInfoFp(outPath, uiDirMode);
        distanceCalculator->setPredictMode(uiDirMode);
        for(int j =0; j < NUM_CU_SIZE_264; j++){
            int iWidth = g_cu_size_264[j][0];
            int iHeight = g_cu_size_264[j][1];
            DistanceData* distanMatri = new DistanceData(iWidth, iHeight, NUM_DISTANCE_SIZE_264);
            predIntraAngAdi(distanMatri, uiDirMode);
            distanceCalculator->calcuDistance(distanMatri);
            delete distanMatri;
        }
    }
	writeMaxDistanceToFile(calc_mode);
}

void H264Predicter::predIntraAngAdi(DistanceData* distanMatri, int uiDirMode){
        int iWidth = distanMatri->tu_width;
        int iHeight = distanMatri->tu_height;
        int iXnN1, iX, iXn, iXnP2;
        if((iWidth == 4) && (iHeight == 4)){
            if(uiDirMode == 3){
				for (int j = 0; j < iHeight; j++) {
					for (int i = 0; i < iWidth; i++) {
                      if((i==3)&&(j==3)){
						 iXnN1 = 6;
					     iX = 6;
                         iXn = 7;
                         iXnP2 = 7;
                      } else{
                         iXnN1 = i+j;
                         iX = i+j+1;
                         iXn = i+j+2;
                         iXnP2 =  i+j+2;
                      }
                      convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                      saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                     }
                }  
            }
            
            if(uiDirMode == 4){
				for (int j = 0; j < iHeight; j++) {
					for (int i = 0; i < iWidth; i++) {
                        if(i > j){
                            iXnN1 = i-j-2;
                            iX = i-j-1;
                            iXn = i-j;
                            iXnP2 = i-j;
                            convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                        }else if(i < j){
                            iXnN1 = j-i-2;
                            iX = j-i-1;
                            iXn = j-i;
                            iXnP2 = j-i;
                        }else{
                            iXnN1 = 0;
                            iX = -1;
                            iXn = -2;
                            iXnP2 = -2;
                        }
                        saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                    }
                }  
            }
            
            if(uiDirMode == 5){
				for (int j = 0; j < iHeight; j++) {
					for (int i = 0; i < iWidth; i++) {
                        int zVR = 2*i - j;
                        if((zVR == 0)||(zVR == 2)||(zVR == 4) ||(zVR == 6)){
                         iXnN1 = i-(j/2)-1;
                         iX = i-(j/2);
                         iXn = i-(j/2);
                         iXnP2 = i-(j/2);
                         convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                        }else if((zVR == 1)||(zVR == 3)||(zVR == 5)){
                         iXnN1 = i-(j/2)-2;
                         iX = i-(j/2)-1 ;
                         iXn = i-(j/2);
                         iXnP2 = i-(j/2);
                         convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                        }else if(zVR == -1){
                          iXnN1 = 0;
                          iX = -1;
                          iXn = -2;
                          iXnP2 = -2;
                        }else{
                          iXnN1 = j -1;
                          iX = j -2;
                          iXn = j -3;
                          iXnP2 = j -3;
                        }
                        saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                    }
                }  
            }

            if(uiDirMode == 6){
				for (int j = 0; j < iHeight; j++) {
					for (int i = 0; i < iWidth; i++) {
                        int zHD = 2*j - i;
                        if((zHD == 0)||(zHD == 2)||(zHD == 4) ||(zHD == 6)){
                            iXnN1 = j-(i/2)-1;
                            iX = j-(i/2);
                            iXn = j-(i/2);
                            iXnP2 = j-(i/2);
                        }else if((zHD == 1)||(zHD == 3)||(zHD == 5)){
                            iXnN1 = j-(i/2)-2;
                            iX = j-(i/2)-1 ;
                            iXn = j -(i/2);
                            iXnP2 = j-(i/2);
                        }else if(zHD == -1){
                            iXnN1 = 0;
                            iX = -1;
                            iXn = -2;
                            iXnP2 = -2;
                        }else{
                            iXnN1 = i -1;
                            iX = i -2;
                            iXn = i -3;
                            iXnP2 = i -3;
                            convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                        }
                        saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                    }
                }  
            }     
        
              if(uiDirMode == 7){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                          if((j == 0)||(j == 2)){
                             iXnN1 = i + (j/2);
                             iX = i + (j/2)+1;
                             iXn = i + (j/2)+1;
                             iXnP2 = i + (j/2)+1;
                             convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          }else if((j == 1)||(j == 3)){
                             iXnN1 = i + (j/2);
                             iX = i + (j/2)+1;
                             iXn = i + (j/2)+2;
                             iXnP2 = i + (j/2)+2;
                             convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          }
                          saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                       }
                    }  
               }  
        
              if(uiDirMode == 8){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                         int  zHU = 2*j + i;
                          if((zHU == 0)||(zHU == 2)||(zHU == 4)){
                             iXnN1 = j + (i/2);
                             iX = j+(i/2) +1;
                             iXn = j+(i/2) +1;
                             iXnP2 = j+(i/2) +1;
                          }else if((zHU == 1)||(zHU == 3)){
                             iXnN1 = j + (i/2);
                             iX = j + (i/2)+1 ;
                             iXn = j +(i/2)+2;
                             iXnP2 = j +(i/2)+2;
                          }else if(zHU == 5){
                              iXnN1 = 2;
                              iX = 3;
                              iXn = 3;
                              iXnP2 = 3;
                          }else{
                              iXnN1 = 3;
                              iX = 3;
                              iXn = 3;
                              iXnP2 = 3;
                          }
                          saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                        }
                    }  
                }  
          }
          if(iHeight == 8 && iWidth == 8){
              if(uiDirMode == 3){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                          if((i==7)&&(j==7)){
                             iXnN1 = 14;
                             iX = 14;
                             iXn = 15;
                             iXnP2 = 15;
                          }else{
                             iXnN1 = i+j;
                             iX = i+j+1;
                             iXn = i+j+2;
                             iXnP2 =  i+j+2;
                            }
                           convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                           saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                        }
                    }  
                }
        
              if(uiDirMode == 4){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                          if(i > j){
                             iXnN1 = i-j-2;
                             iX = i-j-1;
                             iXn = i-j;
                             iXnP2 = i-j;
                             convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          }else if(i < j){
                             iXnN1 = j-i-2;
                             iX = j-i-1 ;
                             iXn = j -i;
                             iXnP2 = j-i;
                          }else{
                              iXnN1 = 0;
                              iX = -1;
                              iXn = -2;
                              iXnP2 = -2;
                           }
                           saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                        }
                    }  
                }
               if(uiDirMode == 5){
				   for (int j = 0; j < iHeight; j++) {
					   for (int i = 0; i < iWidth; i++) {
                          int zVR = 2*i - j;
                          if((zVR == 0)||(zVR == 2)||(zVR == 4) ||(zVR == 6) || (zVR == 8)||(zVR == 10)||(zVR == 12) ||(zVR == 14)){
                             iXnN1 = i-(j/2)-1;
                             iX = i-(j/2);
                             iXn = i-(j/2);
                             iXnP2 = i-(j/2);
                             convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          }else if((zVR == 1)||(zVR == 3)||(zVR == 5) || (zVR == 7)||(zVR == 9)||(zVR == 11) ||(zVR == 13)){
                             iXnN1 = i-(j/2)-2;
                             iX = i-(j/2)-1 ;
                             iXn = i-(j/2);
                             iXnP2 = i-(j/2);
                             convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          }else if(zVR == -1){
                              iXnN1 = 0;
                              iX = -1;
                              iXn = -2;
                              iXnP2 = -2;
                          }else{
                              iXnN1 = j -2*i-1;
                              iX = j -2*i -2;
                              iXn = j -2*i -3;
                              iXnP2 = j -2*i -3;
                            }
                           saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                        }
                    }  
              }       
              if(uiDirMode == 6){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                          int zHD = 2*j - i;
                          if((zHD == 0)||(zHD == 2)||(zHD == 4) ||(zHD == 6) || (zHD == 8)||(zHD == 10)||(zHD == 12) ||(zHD == 14)){
                             iXnN1 = j-(i/2)-1;
                             iX = j-(i/2);
                             iXn = j-(i/2);
                             iXnP2 = j-(i/2);
                          }else if((zHD == 1)||(zHD == 3)||(zHD == 5) || (zHD == 7)||(zHD == 9)||(zHD == 11) ||(zHD == 13)){
                             iXnN1 = j-(i/2)-2;
                             iX = j-(i/2)-1 ;
                             iXn = j -(i/2);
                             iXnP2 = j-(i/2);
                          }else if(zHD == -1){
                              iXnN1 = 0;
                              iX = -1;
                              iXn = -2;
                              iXnP2 = -2;
                          }else{
                              iXnN1 = i - 2*j -1;
                              iX = i - 2*j -2;
                              iXn = i - 2*j -3;
                              iXnP2 = i - 2*j -3;
                              convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                           }
                           saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                        }
                    }  
                }     
        
              if(uiDirMode == 7){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {
                          if((j == 0)||(j == 2) || (j == 4)||(j == 6)){
                             iXnN1 = i + (j/2);
                             iX = i + (j/2)+1;
                             iXn = i + (j/2)+1;
                             iXnP2 = i + (j/2)+1;
                          }else if((j == 1)||(j == 3) || (j== 5)||(j == 7)){
                             iXnN1 = i + (j/2);
                             iX = i + (j/2)+1;
                             iXn = i + (j/2)+2;
                             iXnP2 = i + (j/2)+2;
                          }
                          convertXPoints(&iXnN1,&iX,&iXn, &iXnP2);
                          saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                      }
                  }  
              }  
        
              if(uiDirMode == 8){
				  for (int j = 0; j < iHeight; j++) {
					  for (int i = 0; i < iWidth; i++) {

                          int zHU = 2*j + i;
                          if((zHU == 0)||(zHU == 2)||(zHU == 4) || (zHU == 6)||(zHU == 8)||(zHU == 10) || (zHU == 12)){
                             iXnN1 = j + (i/2);
                             iX = j+(i/2) +1;
                             iXn = j+(i/2) +1;
                             iXnP2 = j+(i/2) +1;
                          }else if((zHU == 1)||(zHU == 3) || (zHU == 5)||(zHU == 7) || (zHU == 9)||(zHU == 11)){
                             iXnN1 = j + (i/2);
                             iX = j + (i/2)+1 ;
                             iXn = j +(i/2)+2;
                             iXnP2 = j +(i/2)+2;
                          }else if(zHU == 13){
                              iXnN1 = 6;
                              iX = 7;
                              iXn = 7;
                              iXnP2 = 7;
                          }else{
                              iXnN1 = 7;
                              iX = 7;
                              iXn = 7;
                              iXnP2 = 7;
                          }
                          saveDistanceMatri(distanMatri, uiDirMode, iWidth, iHeight, i, j, iXnN1, iX, iXn, iXnP2);
                      }
                  }  
              }  
          }
}





