
#include "ofxMioFlowGLSL.h"


void ofxMioFlowGLSL::setup(int wI,int hI) {
	w=wI;
	h=hI;

	lastTex.allocate(w,h,GL_RGBA);  
	lastTex.setUseTexture(true);  
	lastTex.begin();  
	ofClear(0,0,0,0);  
	lastTex.end();

	fboFlow.allocate(w,h,GL_RGBA);  
	fboFlow.setUseTexture(true);  
	fboFlow.begin();  
	ofClear(0,0,0,0);  
	fboFlow.end();

	fboBlurH.allocate(w,h,GL_RGBA);  
	fboBlurH.setUseTexture(true);  
	fboBlurH.begin();  
	ofClear(0,0,0,0);  
	fboBlurH.end();

	fboBlurV.allocate(w,h,GL_RGBA);  
	fboBlurV.setUseTexture(true);  
	fboBlurV.begin();  
	ofClear(0,0,0,0);  
	fboBlurV.end();

	fboRepos.allocate(w,h,GL_RGBA);  
	fboRepos.setUseTexture(true);  
	fboRepos.begin();  
	ofClear(0,0,0,0);  
	fboRepos.end();

	lambda=0.1;
	blurAmount=5;
	displaceAmount=0.5;
	flowShader.setup();

}
void ofxMioFlowGLSL::update(ofTexture cur, float lambdaI,float blurAmountI, float displaceAmountI ) {
	lambda=lambdaI;
	blurAmount=blurAmountI;
	displaceAmount=displaceAmountI; 
	update(cur); 
}

void ofxMioFlowGLSL::update(ofTexture cur) {

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
		glBegin( GL_QUADS );  
		glTexCoord2f(0,0);          glVertex3i(0, 0,0);  
		glTexCoord2f(0, h);          glVertex3i(0, h,0);  
		glTexCoord2f(w, h);          glVertex3i(w, h,0);  
		glTexCoord2f(w, 0);          glVertex3i(w, 0,0);  
		glEnd();  
		flowShader.flow.end();  
		fboFlow.end();  


		//blur Process
		///////////////////////////////////////////////
		fboBlurH.begin();  
		flowShader.blur.begin();      
		flowShader.blur.setUniformTexture("texture", fboFlow, 0); 
		flowShader.blur.setUniform1f("blurSize", blurAmount);
        flowShader.blur.setUniform1f("sigma", blurAmount/2.0); 
		flowShader.blur.setUniform2f("texOffset",2.0,2.0);
        flowShader.blur.setUniform1f("horizontalPass", 1.0);

		glBegin( GL_QUADS );  
		glTexCoord2f(0,0);          glVertex3i(0, 0,0);  
		glTexCoord2f(0, h);          glVertex3i(0, h,0);  
		glTexCoord2f(w, h);          glVertex3i(w, h,0);  
		glTexCoord2f(w, 0);          glVertex3i(w, 0,0);  
		glEnd();
		flowShader.blur.end();  
		fboBlurH.end(); 


		fboBlurV.begin();  
		flowShader.blur.begin();      
		flowShader.blur.setUniformTexture("texture",fboBlurH, 0); 
		flowShader.blur.setUniform1f("blurSize", blurAmount);
        flowShader.blur.setUniform1f("sigma", blurAmount/2.0); 
		flowShader.blur.setUniform2f("texOffset",2.0,2.0);
        flowShader.blur.setUniform1f("horizontalPass", 0.0);

		glBegin( GL_QUADS );  
		glTexCoord2f(0,0);          glVertex3i(0, 0,0);  
		glTexCoord2f(0, h);          glVertex3i(0, h,0);  
		glTexCoord2f(w, h);          glVertex3i(w, h,0);  
		glTexCoord2f(w, 0);          glVertex3i(w, 0,0);  
		glEnd();
		flowShader.blur.end();  
		fboBlurV.end(); 

		//repos Process
		///////////////////////////////////////////////
		fboRepos.begin();  
		flowShader.repos.begin();       
		flowShader.repos.setUniform2f("amt", displaceAmount*2000, displaceAmount*2000);  
		flowShader.repos.setUniformTexture("tex0", cur, 0);  
		flowShader.repos.setUniformTexture("tex1", fboBlurV, 1);  
		glBegin( GL_QUADS );  
		glTexCoord2f(0,0);          glVertex3i(0, 0,0);  
		glTexCoord2f(0, h);          glVertex3i(0, h,0);  
		glTexCoord2f(w, h);          glVertex3i(w, h,0);  
		glTexCoord2f(w, 0);          glVertex3i(w, 0,0);  
		glEnd();  
		flowShader.repos.end();  
		fboRepos.end(); 

		lastTex.begin();
		cur.draw(0,0);
		lastTex.end();

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
ofTexture ofxMioFlowGLSL::getFlowBlurTexture() {
	return fboBlurV.getTexture();
}

ofTexture ofxMioFlowGLSL::getFlowRawTexture() {
	return fboFlow.getTexture();
}