//
//  mapMake.cpp
//  waterScape_v0_02
//
//  Created by Richard on 01/09/2019.
//
// RGB 0.2126 // 0.7152 // 0.0722
// This class try's tp create a semantic map to senf to the landscape model


#define cR 0.2126
#define cB 0.7152
#define cG 0.0722

#include "mapMake.hpp"

MapMaker::MapMaker(){
    
}

MapMaker::~MapMaker(){}
//---------------------------
void MapMaker::setup(){
    
    segMapCols["clouds"] = clouds;
    segMapCols["grass"] =  grass;
    segMapCols["river"] = river;
    segMapCols["sea"] = sea;
    segMapCols["sky"] = sky;
    segMapCols["tree"] = tree;
    
    gClouds = clouds.r * cR + clouds.g * cG + clouds.b * cB;
    gGrass = grass.r * cR + grass.g * cG + grass.b * cG;
    gRiver = river.r * cR + river.g * cG + river.b * cB;
    gSea = sea.r * cR + sea.g * cG + sea.b * cB;
    gSky = sky.r * cR + sky.g * cG + sky.b * cB;
    gTree = tree.r * cR + tree.g * cG + tree.b * cB;
    
    ofLogNotice() <<gClouds << " " <<  gGrass << " " << gRiver << " " <<gSea << " " << gSky << " " << gTree;
//
    segGrays["clouds"] = gClouds; ofLogNotice() <<"gClouds are " << segGrays.at("clouds");
    segGrays["grass"] =  gGrass;
    segGrays["river"] = gRiver;
    segGrays["sea"] = gSea;
    segGrays["sky"] = gSky;
    segGrays["tree"] = gTree;
    
    greyLevels.push_back(gClouds);
    greyLevels.push_back(gGrass);
    greyLevels.push_back(gRiver);
    greyLevels.push_back(gSea);
    greyLevels.push_back(gSky);
    greyLevels.push_back(gTree);
    
     pix2Map.allocate(640, 360, 3);
    
}

//---------------------------
void MapMaker::update(){
    
}
//---------------------------
ofPixels MapMaker::imageToMap(ofPixels greyImg){
    
    greyImg.resize(640, 360);

//  3 channel image
//  each value in the grayscale needs mapping to a (r,g,b) value
//  so the output array will be 3x bigger
    
     for(size_t j=0; j<greyLevels.size();j++)
     {
         for(size_t i=0; i<greyImg.size(); i++)
        {
            size_t dist = abs(greyImg[i]-greyLevels[j]);
            if(dist>1 && dist < 10)
            {
                size_t xPos = floor(i % 640);
                size_t yPos = floor(i / 640);
                pix2Map.setColor(xPos,yPos,segMapCols[keys[j]]);
                //ofLogNotice() << "distance:" << dist;
            }
    
        }
    }
    return pix2Map;
}

//---------------------------
//---------------------------
