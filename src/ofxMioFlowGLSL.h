#pragma once

#include "ofMain.h"
#include "FlowShader.h"


class ofxMioFlowGLSL  {
public:
    bool enabled = true;
    float lambda = 0.5;
    float blurAmount = 5;
    float displaceAmount = 0.5;

    bool doDrawFlowGrid = false;
    bool doDrawFlowGridRaw = false;
    bool doDrawReposition = false;

	void setup(int wI,int hI, int internalformat=GL_RGBA32F);

	void update(ofTexture cur);
	void update(ofTexture cur, float lambdaI,float blurAmountI, float displaceAmountI );

    void draw(int x=0, int y=0);
    void drawFlowGrid(int x=0, int y=0);
    void drawFlowGridRaw(int x=0, int y=0);
    void drawReposition(int x=0, int y=0);
    void drawPassthrough(ofTexture& cur, int x=0, int y=0);

	ofTexture getFlowBlurTexture();
	ofTexture getFlowRawTexture();
     
    int getWidth() { return w; }
    int getHeight() { return h; }
     
private: 
	int w, h;  
    ofPlanePrimitive quad;

	FlowShader flowShader;
    ofFbo  lastTex;  
    ofFbo fboFlow,fboBlurH,fboBlurV,fboRepos; 
};
