
#include "ofxMioFlowGLSL.h"


void ofxMioFlowGLSL::setup(int wI,int hI, int internalformat) {
    w=wI;
    h=hI;

    lastTex.allocate(w,h,internalformat);
    lastTex.setUseTexture(true);
    lastTex.begin();
    ofClear(0,0,0,0);
    lastTex.end();

    fboFlow.allocate(w,h,internalformat);
    fboFlow.setUseTexture(true);
    fboFlow.begin();
    ofClear(0,0,0,0);
    fboFlow.end();

    fboBlurH.allocate(w,h,internalformat);
    fboBlurH.setUseTexture(true);
    fboBlurH.begin();
    ofClear(0,0,0,0);
    fboBlurH.end();

    fboBlurV.allocate(w,h,internalformat);
    fboBlurV.setUseTexture(true);
    fboBlurV.begin();
    ofClear(0,0,0,0);
    fboBlurV.end();

    fboRepos.allocate(w,h,internalformat);
    fboRepos.setUseTexture(true);
    fboRepos.begin();
    ofClear(0,0,0,0);
    fboRepos.end();

    flowShader.setup();

    quad.set(w, -h, 2, 2); // vertical flip
    quad.setPosition(w/2, h/2, 0);
    quad.mapTexCoords(1, 1, w, h);
}


void ofxMioFlowGLSL::update(const ofTexture& cur) {
    if(!enabled) return;

    //flow Process
    ///////////////////////////////////////////////
    fboFlow.begin();
    ofClear(0);
    flowShader.flow.begin();

    flowShader.flow.setUniform2f("scale", 1, 1);
    flowShader.flow.setUniform2f("offset", 1.0,1.0);
    flowShader.flow.setUniform1f("lambda",lambda);
    flowShader.flow.setUniformTexture("tex0", cur, 0);
    flowShader.flow.setUniformTexture("tex1", lastTex, 1);
    quad.draw();
    flowShader.flow.end();
    fboFlow.end();


    //blur Process
    ///////////////////////////////////////////////
    if(doShaderBlur) {
        fboBlurH.begin();
        flowShader.blur.begin();
        flowShader.blur.setUniformTexture("texture", fboFlow, 0);
        flowShader.blur.setUniform1f("blurSize", blurAmount);
        flowShader.blur.setUniform1f("sigma", blurAmount/2.0);
        flowShader.blur.setUniform2f("texOffset",2.0,2.0);
        flowShader.blur.setUniform1f("horizontalPass", 1.0);
        quad.draw();
        fboBlurH.end();


        fboBlurV.begin();
        flowShader.blur.begin();
        flowShader.blur.setUniformTexture("texture",fboBlurH, 0);
        flowShader.blur.setUniform1f("blurSize", blurAmount);
        flowShader.blur.setUniform1f("sigma", blurAmount/2.0);
        flowShader.blur.setUniform2f("texOffset",2.0,2.0);
        flowShader.blur.setUniform1f("horizontalPass", 0.0);
        quad.draw();
        flowShader.blur.end();
        fboBlurV.end();
    }

    //repos Process
    ///////////////////////////////////////////////
    if(doShaderRepos) {
        fboRepos.begin();
        flowShader.repos.begin();
        flowShader.repos.setUniform2f("amt", displaceAmount, displaceAmount);
        flowShader.repos.setUniformTexture("tex0", cur, 0);
        flowShader.repos.setUniformTexture("tex1", fboBlurV, 1);
        quad.draw();
        flowShader.repos.end();
        fboRepos.end();
    }

    lastTex.begin();
    cur.draw(0,0);
    lastTex.end();

    if(doReadback) fboBlurV.readToPixels(flowPix);
}


void ofxMioFlowGLSL::draw(int x, int y) {
    if(!enabled) return;
    if(doDrawFlowGrid) drawFlowGrid(x, y);
    if(doDrawFlowGridRaw) drawFlowGridRaw(x, y);
    if(doDrawReposition) drawReposition(x, y);
    if(doDrawVectors) drawVectors(x, y);
}


void ofxMioFlowGLSL::drawFlowGridRaw(int x,int y) {
    fboFlow.draw(x,y);
}

void ofxMioFlowGLSL::drawFlowGrid(int x,int y) {
    fboBlurV.draw(x,y);
}

void ofxMioFlowGLSL::drawReposition(int x,int y) {
    fboRepos.draw(x,y);
}

void ofxMioFlowGLSL::drawVectors(int x,int y) {
    if(!flowPix.isAllocated()) {
        ofLogWarning("ofxMioFlowGLSL::drawVectors") << "flowPix not allocated. is doReadback enabled?";
        return;
    }

    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);
    for(int j=0; j<h; j+=drawVectorsStep) {
        for(int i=0; i<w; i+=drawVectorsStep) {
            ofFloatColor c = flowPix.getColor(i, j);
            ofVec2f v = colorToVel(c);
            v *= drawVectorsScale;
            ofLine(i, j, i+v.x, j+v.y);
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

//void ofxMioFlowGLSL::drawVectors(int x, int y, float scale, int step) {
//    drawVectorsScale = scale;
//    drawVectorsStep = step;
//    drawVectors(x, y);
//}


ofTexture& ofxMioFlowGLSL::getFlowBlurTexture() {
    return fboBlurV.getTexture();
}

ofTexture& ofxMioFlowGLSL::getFlowRawTexture() {
    return fboFlow.getTexture();
}
