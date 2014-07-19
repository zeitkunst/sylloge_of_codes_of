#include <libintl.h>
#include<locale.h>

#include "ofMain.h"
#include "sylloge_of_codes.h"

//========================================================================
int main( ){
    
    setlocale(LC_ALL, "");
    bindtextdomain("sylloge_of_codes", "/home/nknouf/development/of_v0.8.3_linux64_release/apps/myApps/sylloge_of_codes_of/po");
    //bindtextdomain("sylloge_of_codes", "/usr/share/locale");
    textdomain("sylloge_of_codes");
    ofSetLogLevel(OF_LOG_NOTICE);
	ofSetupOpenGL(1280, 800, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new sylloge_of_codes());

}
