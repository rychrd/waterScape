//
//  imgProc.cpp
//  nng_test
//
//  Created by Richard on 27/08/2019.
//  Some pixel processing functions
#include <stdio.h>
#include "imgProc.h"
// RGB 0.2126 // 0.7152 // 0.0722
//-----------------------------------------------
Pxl::Pxl(){
    
    w=320;h=240;chans=1;
    gradX.allocate(w, h, chans);
    gradY.allocate(w, h, chans);
    gradXY.allocate(w, h, chans);
    angs.allocate(w, h, chans);
    mags.allocate(w, h, chans);
    grayd.allocate(w, h, 1);
    diff.allocate(w, h, 1);
    
}
//-----------------------------------------------
Pxl::Pxl(size_t w, size_t h, size_t chans){
    
}
//-----------------------------------------------
void Pxl::pxlSetup(size_t w, size_t h, size_t chans){
    
    gradX.allocate(w, h, chans);
    gradY.allocate(w, h, chans);
    gradXY.allocate(w, h, chans);
    angs.allocate(w, h, chans);
    mags.allocate(w, h, chans);
   
}
//-----------------------------------------------
Pxl::~Pxl(){

}
//-----------------------------------------------
ofPixels& Pxl::gradientX(ofPixels& px){
    
    size_t numChans = px.getNumChannels();
    
    if (numChans>1){}
    
    size_t w = px.getWidth();
    size_t h = px.getHeight();
    size_t col;
    
    for (size_t i=1; i<h-1; i++){
        col = i * w * numChans;   // span one row of pixels in the array
        for (size_t j=1; j<w-1; j+=numChans){
            gradX[col+j] = (((px[(col+(j-1))]) *-1) + px[col+(j+1)]) * 0.5; // gradient in x using a [-1, 0, 1] kernel.
        }
    }
    return gradX;
}
//-----------------------------------------------
ofPixels& Pxl::gradientY(ofPixels& px){
    
    size_t w = px.getWidth();
    size_t h = px.getHeight();
    size_t col, colA, colB;
    
    for (size_t i=1; i<h-1; i++){
        colA = (i-1) * w;       // the pixel directly above in the image
        colB = (i+1) * w;       // the pixel below
        col = i * w;            // the centre pixel
    
        for (size_t j=1; j<w-1; j++){
            gradY[col+j] =  (((px[colA+j])*-1) + px[colB+j]) * 0.5;
        }
    }
    return gradY;    
}

//-----------------------------------------------
ofPixels& Pxl::gradientXY(ofPixels& xGrad, ofPixels& yGrad){

    size_t totalPix = xGrad.size();
    
    for (int i=0; i<totalPix;i++){
        gradXY[i] = xGrad[i] + yGrad[i];
    }
    return gradXY;
}
//-----------------------------------------------

ofPixels& Pxl::grayScale(ofPixels& px_){
    
    size_t numChans = px_.getNumChannels();
    
    for (int i=0; i<grayd.size(); i++){
        
        grayd[i] = ((px_[i*numChans] * 0.2126) + (px_[i*numChans+1] * 0.7152) + (px_[i*numChans+2] * 0.0722));
    }
    
    return grayd;
}
//-----------------------------------------------
ofPixels& Pxl::frameDiff(ofPixels& currentF, ofPixels& previousF){
    
    size_t totalPix = currentF.size();
    for (int i = 0; i<totalPix; i++){
        diff[i] = abs(currentF[i] - previousF[i]);
    }
    return diff;
}

