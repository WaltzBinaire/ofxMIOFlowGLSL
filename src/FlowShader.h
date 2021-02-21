#pragma once

#include "ofMain.h"

class FlowShader  {
public:
	void setup();

    string getVertShader();
	string getFlowShader();
	string getReposShader();
    string getBlurShader();

    ofShader repos, flow, blur;
};
