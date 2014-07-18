#include <libintl.h>

#include "ofMain.h"
#include "sylloge_of_codes.h"

//========================================================================
int main( ){
    
    ofSetLogLevel(OF_LOG_NOTICE);
    setlocale(LC_ALL, "");
    bindtextdomain("sylloge_of_codes", "/Users/nknouf/development/of_v0.8.3_osx_release/apps/myApps/sylloge_of_codes_of/po");
    textdomain("sylloge_of_codes");
	ofSetupOpenGL(1280, 800, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new sylloge_of_codes());

}
