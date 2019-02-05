#pragma once

#include "ofMain.h"
#include "FlowShader.h"


class ofxMioFlowGLSL  {
public:
	void setup(int wI,int hI, int internalformat=GL_RGBA32F);

	void update(ofTexture cur);
	void update(ofTexture cur, float lambdaI,float blurAmountI, float displaceAmountI );

	void drawFlowGrid(int x,int y);
	void drawFlowGridRaw(int x,int y);
	void drawReposition(int x,int y);

	ofTexture getFlowBlurTexture();
	ofTexture getFlowRawTexture();
     
    int getWidth() { return w; }
    int getHeight() { return h; }
     
private: 

	int w, h;  

	float lambda;
	float blurAmount;
	float displaceAmount; 


	FlowShader flowShader;
    ofFbo  lastTex;  
    ofFbo fboFlow,fboBlurH,fboBlurV,fboRepos; 
};
