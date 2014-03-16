#include <libintl.h>

#include "ofMain.h"
#include "sylloge_of_codes.h"

//========================================================================
int main( ){
    
    ofSetLogLevel(OF_LOG_VERBOSE);    
    bindtextdomain("sylloge_of_codes", "../");
    textdomain("sylloge_of_codes");
	ofSetupOpenGL(1280, 800, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new sylloge_of_codes());

}
