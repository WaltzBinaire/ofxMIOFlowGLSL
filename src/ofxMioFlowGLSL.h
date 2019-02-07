#pragma once

#include "ofMain.h"
#include "FlowShader.h"

class ofxMioFlowGLSL  {
public:
    bool enabled = true;
    bool readback = false;
    float lambda = 0.5;
    float blurAmount = 5;
    float displaceAmount = 0.5;

    bool doDrawFlowGrid = false;
    bool doDrawFlowGridRaw = false;
    bool doDrawReposition = false;
    bool doDrawVectors = false;

    ofFloatPixels flowPix; // contains flow information in pixels (needs readback to be true)

    template <typename T>
    ofVec2f colorToVel(ofColor_<T> col) {
        if (col.a >0.95)  col.b *= -1;
        return ofVec2f(col.r-col.g, col.b);
    }

    void setup(int wI,int hI, int internalformat=GL_RGBA32F);

    void update(const ofTexture& cur);
    void update(const ofTexture& cur, float lambdaI, float blurAmountI, float displaceAmountI, bool readbackI);

    void draw(int x=0, int y=0);
    void drawFlowGrid(int x=0, int y=0);
    void drawFlowGridRaw(int x=0, int y=0);
    void drawReposition(int x=0, int y=0);
    void drawVectors(int x=0, int y=0, float scale=10, int step=1);


    ofTexture& getFlowBlurTexture();
    ofTexture& getFlowRawTexture();

    int getWidth() { return w; }
    int getHeight() { return h; }

private:
    int w, h;
    ofPlanePrimitive quad;

    FlowShader flowShader;
    ofFbo  lastTex;
    ofFbo fboFlow,fboBlurH,fboBlurV,fboRepos;
};
