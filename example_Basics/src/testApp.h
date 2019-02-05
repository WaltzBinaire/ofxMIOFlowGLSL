#pragma once

#include "ofMain.h"
#include "ofxMioFlowGLSL.h"


class testApp : public ofBaseApp{  
public:
    void setup();
    void update();
    void draw();

    ofVideoGrabber cam;
    ofxMioFlowGLSL mioFlow;
};
