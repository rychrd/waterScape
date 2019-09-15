//
//  mapMake.hpp
//  waterScape_v0_02
//
//  Created by Richard on 01/09/2019.

// Model semantic colour references
//                                  "clouds":   [170,170,170],
//                                   "grass":    [29,195,49],
//                                   "river":    [0,57,150],
//                                     "sea":      [54,62,167],
//                                     "sky":      [95,219,255],
//                                    "tree":     [140,104,47]},

#ifndef mapMake_hpp
#define mapMake_hpp

#include <stdio.h>
#include "ofMain.h"

#endif /* mapMake_hpp */
using namespace glm;
class MapMaker  {
    
public:
    MapMaker();
    ~MapMaker();
    
    void setup();
    void update();
    
    ofPixels imageToMap(ofPixels _imageIn);

private:
    map<string,ofColor> segMapCols;
    map<string,ofColor> segGrays;
    
    const ofColor clouds=  ofColor(170,170,170);
    const ofColor grass=   ofColor(29,195,49);
    const ofColor river=   ofColor(0,57,150);
    const ofColor sea=     ofColor(54,62,167);
    const ofColor sky=     ofColor(95,219,255);
    const ofColor tree=    ofColor(140,104,47);
    
    const array<string, 6> keys{"clouds", "grass", "river",
                                "sea", "sky", "tree"};
    
    
    size_t   gClouds,gGrass, gRiver, gSea, gSky, gTree;
    ofPixels pix2Map;
    
    vector<vec3> segColours;
    vector<size_t> greyLevels;
    
    
    
};
