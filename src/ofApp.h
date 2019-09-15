#pragma once

#include "ofMain.h"

#include "ofxNNGRep.h"
#include "ofxNNGReq.h"
#include "ofxXmlPoco.h"
#include "ofxIO.h"

#include "imgProc.h"
#include "mapMake.hpp"
#include "ofxRunway.h"
#include "spng.h"
#include <inttypes.h>


class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    void uploadImage(ofPixels& imageToSend_);
    void readOutput(ofHttpResponse _rwOut);
    std::string getTime();
    
    string  data, b64_decoded, rwOutUrl;
    int camWidth, camHeight, pumpSpeed;
    long bufLen, t;
    
    ofVideoGrabber camIn;
    ofPixels newFrame, prevFrameGray, pixFromPNG, segMap;
    ofImage decodedImage, noHead, testMap, diskImg;
    ofTexture outPix, grayPixOut, xGrads, yGrads, xyGrads, diffs, rwDecoded, segMapTex;
    bool toSend, bFetchImage, debug;
    bool ready, bAuto;
    
    
    Pxl pxl;
    MapMaker mapmake;
    ofJson jMapToSend;
    ofxIO::Base64Encoding b64Encoder;
    ofxRunway runway;
    
    
    
    
private:
	ofx::nng::Req rw_req_;
    ofx::nng::Rep rw_rep_;
	ofx::nng::Rep rep_;

};
