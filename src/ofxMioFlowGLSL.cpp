
#include "ofxMioFlowGLSL.h"

// --------------------------------------------------
ofxMioFlowGLSL::ofxMioFlowGLSL()
{
	lambda = 0.1;
	blurAmount = 5;
	displaceAmount = 0.5;
	flowScale = 1.0;

	flowIsSetup = false;
}

// --------------------------------------------------
void ofxMioFlowGLSL::setup(int wI,int hI) 
{
	width = wI;
	height = hI;

	ofEnableArbTex();

	lastTex.allocate(width,height,GL_RGBA);  
	lastTex.setUseTexture(true);  
	lastTex.begin();  
		ofClear(0,0,0,0);  
	lastTex.end();

	fboFlow.allocate(width,height,GL_RGBA);  
	fboFlow.setUseTexture(true);  
	fboFlow.begin();  
		ofClear(0,0,0,0);  
	fboFlow.end();

	fboBlurH.allocate(width,height,GL_RGBA);  
	fboBlurH.setUseTexture(true);  
	fboBlurH.begin();  
		ofClear(0,0,0,0);  
	fboBlurH.end();

	fboBlurV.allocate(width,height,GL_RGBA);  
	fboBlurV.setUseTexture(true);  
	fboBlurV.begin();  
		ofClear(0,0,0,0);  
	fboBlurV.end();

	fboRepos.allocate(width,height,GL_RGBA);  
	fboRepos.setUseTexture(true);  
	fboRepos.begin();  
		ofClear(0,0,0,0);  
	fboRepos.end();

	flowShader.setup();

	flowIsSetup = true;

	//cout << "Setup flow " << width << ", " << height << endl;
}

// --------------------------------------------------
void ofxMioFlowGLSL::update( ofTexture& cur, bool runReposition ) 
{
	// Flow Process
	///////////////////////////////////////////////
	fboFlow.begin(); 
		ofClear(0);
		flowShader.flow.begin(); 

			flowShader.flow.setUniform2f("scale", flowScale, flowScale);  
			flowShader.flow.setUniform2f("offset", 1.0,1.0);  
			flowShader.flow.setUniform1f("lambda",lambda);  
			flowShader.flow.setUniformTexture("tex0", cur, 0);  
			flowShader.flow.setUniformTexture("tex1", lastTex, 1);        

			cur.draw(0,0,width,height);

		flowShader.flow.end();  
	fboFlow.end();  


	// Blur Process
	///////////////////////////////////////////////

	// Save the fame we're going to priocess into fboBlurV so that we can loop
	fboBlurV.begin();
		fboFlow.draw(0,0);
	fboBlurV.end();

	for( int i = 0; i < numBlurPasses; i++ )
	{
		fboBlurH.begin();  
			flowShader.blur.begin();      
				flowShader.blur.setUniformTexture("texture", fboFlow, 0); 
				flowShader.blur.setUniform1f("blurSize", blurAmount);
				flowShader.blur.setUniform1f("sigma", blurAmount/2.0); 
				flowShader.blur.setUniform2f("texOffset",2.0,2.0);
				flowShader.blur.setUniform1f("horizontalPass", 1.0);

				fboBlurV.draw(0,0,width,height);

			flowShader.blur.end();  
		fboBlurH.end(); 

		fboBlurV.begin();  
			flowShader.blur.begin();      
				flowShader.blur.setUniformTexture("texture",fboBlurH, 0); 
				flowShader.blur.setUniform1f("blurSize", blurAmount);
				flowShader.blur.setUniform1f("sigma", blurAmount/2.0); 
				flowShader.blur.setUniform2f("texOffset",2.0,2.0);
				flowShader.blur.setUniform1f("horizontalPass", 0.0);

				fboBlurH.draw(0,0,width,height);

			flowShader.blur.end();  
		fboBlurV.end(); 
	}

	// Save latest frame
	lastTex.begin();
		cur.draw(0,0);
	lastTex.end();

	if( runReposition )
	{
		//Reposition Process
		///////////////////////////////////////////////
		fboRepos.begin();  
			flowShader.repos.begin();       
				flowShader.repos.setUniform2f("amt", displaceAmount, displaceAmount);  
				flowShader.repos.setUniformTexture("tex0", cur, 0);  
				flowShader.repos.setUniformTexture("tex1", fboBlurV, 1);  

				cur.draw(0,0,width,height);

			flowShader.repos.end();  
		fboRepos.end(); 
	}	
}

// --------------------------------------------------
void ofxMioFlowGLSL::drawFlowGrid(const ofRectangle & rect)		{ fboBlurV.draw(rect); }

// --------------------------------------------------
void ofxMioFlowGLSL::drawFlowGridRaw(const ofRectangle & rect)  { fboFlow.draw(rect); }

// --------------------------------------------------
void ofxMioFlowGLSL::drawReposition(const ofRectangle & rect)	{ fboRepos.draw(rect); }

// --------------------------------------------------
void ofxMioFlowGLSL::drawFlowGrid(int x,int y)					{ drawFlowGrid( ofRectangle( x, y, fboBlurV.getWidth(), fboBlurV.getHeight() ) );	}

// --------------------------------------------------
void ofxMioFlowGLSL::drawFlowGridRaw(int x,int y)				{ drawFlowGridRaw( ofRectangle( x, y, fboFlow.getWidth(), fboFlow.getHeight() ) ); }

// --------------------------------------------------
void ofxMioFlowGLSL::drawReposition(int x,int y)				{ drawReposition( ofRectangle( x, y, fboRepos.getWidth(), fboRepos.getHeight() ) );}

// --------------------------------------------------
ofTexture& ofxMioFlowGLSL::getFlowBlurTexture()					{ return fboBlurV.getTextureReference(); }

// --------------------------------------------------
ofTexture& ofxMioFlowGLSL::getFlowRawTexture()					{ return fboFlow.getTextureReference(); }