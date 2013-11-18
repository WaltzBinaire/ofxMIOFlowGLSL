#pragma once

#include "ofMain.h"

#define STRINGIFY(A) #A

class FlowShader  {
public:
	void setup();

	string getVertShader();
	string getFlowShader();
	string getReposShader();
	string getBlurShader();


	ofShader repos, flow,blur;  
};
