#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.setGLVersion(4,3);
    settings.windowMode = OF_WINDOW;
    settings.multiMonitorFullScreen = true;
    ofCreateWindow(settings);			// <-------- setup the GL context
    ofRunApp( new testApp());
}
