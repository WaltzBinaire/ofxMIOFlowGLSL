#pragma once

#include "ofMain.h"
#include "FlowShader.h"

class ofxMioFlowGLSL  {
public:
    bool enabled = true;
    bool doShaderBlur = true; // run blurring shader
    bool doShaderRepos = true; // run repositioning (warping) shader
    bool doReadback = false; // TODO: which one to readback?
    float lambda = 0.5;
    float blurAmount = 5;
    float displaceAmount = 1000;

    bool doDrawFlowGrid = false;
    bool doDrawFlowGridRaw = false;
    bool doDrawReposition = false;
    bool doDrawVectors = false;
    float drawVectorsScale = 1000;
    int drawVectorsStep = 10;

    ofFloatPixels flowPix; // contains flow information in pixels (needs doReadback to be true)

    template <typename T>
    ofVec2f colorToVel(ofColor_<T> col) {
        return colorToVel(col.r, col.g, col.b, col.a);
    }

    ofVec2f colorToVel(float x, float y, float z, float w) {
        if (w >0.95)  z *= -1.0;
        return ofVec2f(x-y, z);
    }

    void setup(int wI,int hI, int internalformat=GL_RGBA32F);

    void update(const ofTexture& cur);

    void draw(int x=0, int y=0);
    void drawFlowGrid(int x=0, int y=0);
    void drawFlowGridRaw(int x=0, int y=0);
    void drawReposition(int x=0, int y=0);
    void drawVectors(int x=0, int y=0);


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
