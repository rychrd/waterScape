//
//  imgProc.h
//
//  Created by Richard on 27/08/2019.
//  Want to capture some features of water and use then to try to determine where the drops
//  and rivulets are - using brightness, gradients etc

#ifndef imgProc_h
#define imgProc_h
#endif /* imgProc_h */
#include "ofMain.h"

class Pxl {
public:
    
    Pxl();
    Pxl(size_t w, size_t h, size_t chans);
    ~Pxl();
    
    size_t w, h, chans;
    
    void       pxlSetup(size_t _w, size_t _h, size_t _chans);
    ofPixels&  gradientX(ofPixels& _px);
    ofPixels&  gradientY(ofPixels& _px);
    ofPixels&  gradientXY(ofPixels& _xGrad, ofPixels& _yGrad);
    ofPixels   angles(ofPixels& _px);
    ofPixels   magnitudes(ofPixels& _px);
    ofPixels&  grayScale(ofPixels& _px);
    ofPixels&  frameDiff(ofPixels& _cF, ofPixels& _lF);
    
    ofPixels gradX, gradY, gradXY, angs, mags, grayd, diff;
    
};
