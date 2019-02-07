
#include "testApp.h"  


void testApp::setup(){  
    int w,h;
    w=640;
    h=480;
    cam.initGrabber(w, h, true);
    cam.setUseTexture(true);


    ofSetBackgroundAuto(false);
    ofEnableAlphaBlending();

    mioFlow.setup(w,h);
} 



//--------------------------------------------------------------  
void testApp::update(){  
    cam.update();

    if (cam.isFrameNew()) {
        mioFlow.enabled = true;
        mioFlow.doShaderBlur = true; // smooth optical flow
        mioFlow.doShaderRepos = true; // warp original image
        mioFlow.doReadback = true; // needed for vector drawing
        mioFlow.lambda = 1.0 * ofGetMouseX() / ofGetWidth();
        mioFlow.blurAmount = 10.0 * ofGetMouseY() / ofGetHeight();
        mioFlow.displaceAmount = 1000;

        mioFlow.update(cam.getTexture());
    }
}  

//--------------------------------------------------------------  
void testApp::draw(){  
    ofBackground(0);

    mioFlow.drawReposition(0, 0);
    mioFlow.drawReposition(640, 0);
    mioFlow.drawVectors(640, 0);

    mioFlow.drawFlowGridRaw(0, 480);
    mioFlow.drawFlowGrid(640, 480);
    ofSetColor(255);
    ofDrawBitmapString("oi -> " + ofToString(ofGetFrameRate()),20,20,0);
}  
