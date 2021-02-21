
#include "testApp.h"  


void testApp::setup(){  
    cam.initGrabber(640, 480, true);
    cam.setUseTexture(true);

    mioFlow.setup(cam.getWidth(), cam.getHeight());

	ofSetBackgroundColor(50);
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
    mioFlow.drawReposition(0, 0);
    mioFlow.drawReposition(mioFlow.getWidth(), 0);
    mioFlow.drawVectors(mioFlow.getWidth(), 0);

    mioFlow.drawFlowGridRaw(0, mioFlow.getHeight());
    mioFlow.drawFlowGrid(mioFlow.getWidth(), mioFlow.getHeight());
    ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20,0);
}  
