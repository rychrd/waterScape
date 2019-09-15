#include "ofApp.h"

// Gets information from a water captured by camera input and tries to turn it into a viable semantic map for a machine learing model "SPADE"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofHideCursor();
    
    bAuto = false;
    debug = true;
    pumpSpeed = 0;
    mapmake.setup();
    ofBackground(0);
    t = ofGetElapsedTimeMillis();
    
    // Set up the NNG messaging callback for the Pump controller Pi
    // * nng * setup responder style socket with a callback function - responds to the pump controller wih a value for the pump.
    ofx::nng::Rep::Settings reps;
    rep_.setup(reps, std::function<bool(const ofBuffer&, ofBuffer&)>
               ([this] (const ofBuffer &inBuffer, ofBuffer &outBuffer)
                {
                    outBuffer = ofToString(pumpSpeed);
                    cout << "sent pump level " << pumpSpeed << endl;  // callback function based
                    return true;
                }));
    rep_.listen("tcp://192.168.0.30:54321");   // start listening for speed requests from the pump
    
    //---------- Setup the Runway HTTP client
    ready = true;
    toSend = false;
    bFetchImage = false;
    ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
    
    rwOutUrl =   "http://192.168.0.10:8000/data";   // can be queried for an image
    runway.setup("http://192.168.0.10:8000");       // server address to send images to
    runway.start(); // starts the thread
    
    
    //---------- Setup the Camera input
    camWidth=   720;
    camHeight=  576;
    
    vector<ofVideoDevice> cams = camIn.listDevices();
    vector<ofVideoDevice> availableCams;
    for (auto &device : cams)
    {
        if(device.bAvailable)
        {
            availableCams.push_back(device);
            ofLogNotice() << device.id << "-" << device.deviceName << " is available";
        } else {
            ofLogNotice() << device.id << "-" << device.deviceName << "has gone away";
        }
    }
    if(availableCams.size()>0){
        
        size_t devID = availableCams.size()-1;
        camIn.setDeviceID(0); //  don't use a builtin camera if there
        camWidth = camIn.getWidth();
        camIn.initGrabber(camWidth, camHeight);
        
        newFrame.allocate(camWidth, camHeight, camIn.getPixelFormat());// allocate some memory for camera images
        prevFrameGray.allocate(camWidth, camHeight, 1);         // for the result of processing
        prevFrameGray = camIn.getPixels();
        
        outPix.allocate(newFrame);
        grayPixOut.allocate(camWidth, camHeight, 1);
        segMapTex.allocate(640,360,3);
    }
    
    bFetchImage = true;
    pumpSpeed == 2200;
}

//-------------------------------------------------------------
void ofApp::update(){
   
    if(bAuto){
    if(!((ofGetFrameNum()/60) % 300))
    {
    
        float pumpRoll = ofRandom(1, 100);
    
    if(pumpRoll > 10){
        pumpSpeed = ofRandom(2195, 2260);
        } else {
        if(pumpRoll < 10)
        {
            pumpSpeed=0;
        }
    }
    }
    }
    pumpSpeed = pumpSpeed % 4096;           // keep the speed below 4096 (5V from the DAC)
    
    camIn.update();
    if(camIn.isFrameNew()){
        newFrame  = camIn.getPixels();
        
        ofPixels grayImage = pxl.grayScale(newFrame);
        pxl.frameDiff(grayImage, prevFrameGray);
        prevFrameGray = grayImage;

    
        ofPixels xGradients; //
        xGradients.setNumChannels(1);
        xGradients = pxl.gradientX(grayImage);
        
        ofPixels yGradients; //
        yGradients.setNumChannels(1);
        yGradients = pxl.gradientY(grayImage);
 
        ofPixels xyGradients = pxl.gradientXY(xGradients, yGradients);
        
        grayPixOut.allocate(pxl.grayd);
        xGrads.allocate(pxl.gradX);
        yGrads.allocate(pxl.gradY);
        xyGrads.allocate(pxl.gradXY);
        diffs.allocate(pxl.diff);
        
        segMap = mapmake.imageToMap(yGradients);
        segMapTex.allocate(segMap);
        
    }
//    pass the grayscale camera input to be coloured
   
//  *** try to get an image from the server every 2 seconds
  
   
    
    if((!(ofGetFrameNum()%300)) && bFetchImage)
    {
        toSend = true;
        auto rwOutput = (ofHttpResponse(ofLoadURL(rwOutUrl)));  // try to get an iamge fromh server
        if(rwOutput.data.size()>0)
        {
        readOutput(rwOutput);
        }
    
    }
          // uplaod a new Semantic Map based on the camrea input
    
    if(toSend) {
        uploadImage(segMap);
    }
    
    if((!(ofGetFrameNum()/60 % 900))){
        ofSaveScreen(ofGetTimestampString() + ".png");
       // ofImage diskImg;
        diskImg.setFromPixels(segMap);
        diskImg.save(ofGetTimestampString() + "segMap.png");
        ofLogNotice() << "saved an image";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    if (rwDecoded.isAllocated())
    {
        rwDecoded.draw(glm::vec2(0,0), ofGetWidth(),ofGetHeight());
        ofDrawBitmapString(pumpSpeed, 0, 10);
       if(debug) grayPixOut.draw(glm::vec2(0,240), 360, 240);
       
//        diffs.draw(glm::vec2(0,0), 360, 240);
//        rwDecoded.draw(glm::vec2(0,0), 360, 240);
//        xGrads.draw(glm::vec2(0,240), 360, 240);
//        yGrads.draw(glm::vec2(360,240), 360, 240);
//        xyGrads.draw(glm::vec2(360,0), 360, 240);
    }
   // segMapTex.draw(glm::vec2(360,0), 360, 240);
   // grayPixOut.draw(glm::vec2(0,240), 360, 240);
}

//--------------------------------------------------------------
std::string ofApp::getTime(){
    
        string _time = ofToString(ofGetFrameNum());
        return _time;
}
//--------------------------------------------------------------
void ofApp::readOutput(ofHttpResponse _rwOutput){
    
    data.clear();
    data = _rwOutput.data;
    
//  *** exit if there's no data
    if(data.size()==0)
    {
        ofLogNotice() << "got no data, exiting";
        return;
    }
//  read from the comma after the messag info string
    data = data.substr(data.find(",")+1);
//  data = data.substr(0, data.size()-2); // *** removes padding
    
    data = ofxIO::Base64Encoding::decode(data, true);  //decode from base64

    bufLen = data.size();
    char* imgBuf = new char[bufLen];            //  delete[] this later or it will leak
    strcpy(imgBuf, data.c_str());
    
    ofLogNotice()
    <<          "buffer size:"
    <<           bufLen;
    
    
    //  *** libspng PNG decompressor:  based on the example code at https://libspng.org
    spng_ctx *ctx = spng_ctx_new(0);           /* Create a context */
    spng_set_png_buffer(ctx, imgBuf, bufLen);  /* loads PNG into buffer */
    
    struct spng_ihdr ihdr;                  // parses the PNG header
    int r = spng_get_ihdr(ctx,  &ihdr);
    
    if(r)
        {
            printf("spng_get_ihdr() error: %s\n", spng_strerror(r));
        }
    
    string clr_type_str;    //char *clr_type_str; - changed from  char*
    if(ihdr.color_type == SPNG_COLOR_TYPE_GRAYSCALE)
            clr_type_str = "grayscale";
    else if(ihdr.color_type == SPNG_COLOR_TYPE_TRUECOLOR)
            clr_type_str = "truecolor";
    else if(ihdr.color_type == SPNG_COLOR_TYPE_INDEXED)
            clr_type_str = "indexed color";
    else if(ihdr.color_type == SPNG_COLOR_TYPE_GRAYSCALE_ALPHA)
            clr_type_str = "grayscale with alpha";
    else
            clr_type_str = "truecolor with alpha";
    
//      <intypes> macros:
    
////        printf("width: %" PRIu32 "\nheight: %" PRIu32 "\n"
//           "bit depth: %" PRIu8 "\ncolor type: %" PRIu8 " - %s\n",
//           ihdr.width, ihdr.height,
//           ihdr.bit_depth, ihdr.color_type, clr_type_str.c_str()); // needs c.str()
//        printf("compression method: %" PRIu8 "\nfilter method: %" PRIu8 "\n"
//           "interlace method: %" PRIu8 "\n",
//           ihdr.compression_method, ihdr.filter_method,
//           ihdr.interlace_method);
    
//    decode as 8bit RGB
        size_t out_size;
        r = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &out_size);
    
        if(r){
//          check for mad values in the buffer, probably wasn't image data
            printf("image size error: %s\n", spng_strerror(r));
            spng_ctx_free(ctx);
            delete[] imgBuf;
            return;
        }
    
        unsigned char* out =  new unsigned char[out_size];      // array to hold the RGBA image values, needs releasing later
    
        r = spng_decode_image(ctx, out, out_size, SPNG_FMT_RGBA8, 0);
    
        if(r){ cout << "decode err" << endl;}
    
    //  Allocate the array, now RGB values, to an ofImage
        decodedImage.setFromPixels(out, 640, 360, OF_IMAGE_COLOR_ALPHA);
        rwDecoded.allocate(decodedImage.getPixels());           // load up to the ofImage texture
    
    
        spng_ctx_free(ctx);
        delete[] out;
        delete[] imgBuf;
    
}
//--------------------------------------------------------------
void ofApp::uploadImage(ofPixels& imageToSend){
    
// this uses part of an ofxRunway example to send a image into the model
    
    if (toSend) {
        ofxRunwayBundle bundle;
        bundle.address = "query";
        bundle.images["semantic_map"] = imageToSend;
        runway.send(bundle);
        toSend = false;
        ready = false;
    }
    
    ofxRunwayBundle bundleToReceive;
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key){
	
    if(key == 's'){
        ofSaveScreen(ofGetTimestampString() + ".png");
    }
    if(key == 'r'){
        
        toSend = true;
       // sendImage();
    }
    if(key == 'g') bFetchImage =! bFetchImage;
    
    if(key== '=') pumpSpeed += 5;
    
    if(key== '-') pumpSpeed -= 5;
    
    if(key== 'o') pumpSpeed = 0;
    
    if(key== 'l') pumpSpeed = 2000;
    
    if(key== 'e'){
        pumpSpeed = 0;
        ofExit();
    }
    if(key=='d') debug =! debug;
    
    if(key=='a') bAuto = ! bAuto;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
