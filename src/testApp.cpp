#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    char *sql;
    const char* msg = "Callback function called";
    completeText = "Aenean laoreet feugiat turpis eget ultrices. Curabitur viverra aliquam neque, quis interdum augue tempor bibendum. Integer tempus non sapien ut fringilla. Suspendisse potenti. Nullam ultricies pharetra accumsan. Donec aliquam ligula orci, quis aliquam urna bibendum eu. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed in quam sed risus sodales sollicitudin. Vivamus scelerisque lacinia eros, et vulputate magna laoreet sed. Praesent ultricies elit eu accumsan ornare. Aliquam consequat viverra magna, vitae egestas lorem dictum ut.\n Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nam egestas justo felis, et condimentum diam malesuada sit amet. Donec luctus imperdiet dignissim. Sed auctor, leo ac gravida placerat, odio nibh vestibulum nisl, ut dictum tortor dui ut nulla. Curabitur scelerisque quam erat, sed faucibus mi suscipit eu. Vestibulum tortor lacus, varius et orci a, cursus tempor risus. \n Curabitur nisl tortor, elementum sagittis felis eu, pharetra accumsan purus.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc molestie nec turpis ut euismod. Cras dignissim laoreet ipsum, ut facilisis nisl. Nulla rhoncus bibendum arcu fringilla tristique. Nullam mattis fringilla odio, tincidunt ullamcorper tellus elementum nec. Pellentesque sed lacinia ipsum. Integer at magna quis ante luctus convallis. Proin non urna in nunc dictum vestibulum. Nunc adipiscing mauris ante, a commodo leo dictum et. Pellentesque aliquam magna diam, quis volutpat ante egestas id. Fusce id scelerisque purus.";

    std::string s("Sat, 1 Jan 2005 12:00:00 GMT");
    int tzd;
    DateTime dt;
    DateTimeParser::parse(DateTimeFormat::RFC1123_FORMAT, s, dt, tzd);
    Poco::Timestamp ts = dt.timestamp();
    Poco::LocalDateTime ldt(tzd, dt);
    bool ok = DateTimeParser::tryParse("2006-10-22", dt, tzd);
    ok = DateTimeParser::tryParse("%e.%n.%Y", "22.10.2006", dt, tzd);

    rc = sqlite3_open("/Users/nknouf/src/sylloge_of_codes/sylloge_of_codes/sylloge_of_codes.sqlite", &db);

    if (rc) {
        ofLog(OF_LOG_NOTICE, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        ofLog(OF_LOG_NOTICE, "Opened database successfully\n");
    }

    sql = "SELECT * from sylloge";

    rc = sqlite3_exec(db, sql, basicCallback, (void*)msg, &zErrMsg);
    if (rc != SQLITE_OK) {
        ofLog(OF_LOG_ERROR, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        ofLog(OF_LOG_NOTICE, "Select done successfully\n");
    }

   // make DB QUERY
    sqlite = new ofxSQLite("/Users/nknouf/src/sylloge_of_codes/sylloge_of_codes/sylloge_of_codes.sqlite"); 

    setSyllogeCount();
    cout << "total number of entires: " << syllogeCount << endl;
    int index = rand() % syllogeCount;

    ofxSQLiteSelect sel = sqlite->select("id, code, comments, pseudonym, code_date, enabled").from("sylloge").where("id", index);
    sel.execute().begin();

    while (sel.hasNext()) {
        currentCode.id = sel.getInt();
        currentCode.code = sel.getString();
        currentCode.comments = sel.getString();
        currentCode.pseudonym = sel.getString();
        //currentCode.code_date = sel.getString();
        bool ok = DateTimeParser::tryParse(sel.getString(), currentCode.code_dt, tzd);
        currentCode.enabled = (bool) sel.getInt();
        sel.next();
    }

    // Annoying, but this is how it works...
    std::ostringstream stringStream;
    stringStream << DateTimeFormatter::format(currentCode.code_dt.timestamp(), "%W, %e %B %Y") << "\n" << currentCode.pseudonym << "\n" << currentCode.code;
    completeText = stringStream.str();
    
    // 23 lines visible right now...

	ofBackground(255,255,255);

	testFont.loadFont("AJensonPro-Regular.otf", 160, true, true, true);
	testFont2.loadFont("SourceSansPro-Regular.otf", 52, true, true, true);

    myText.init("AJensonPro-Regular.otf", 30);

    // Quick estimate of the number of words we can display onscreen at a time
    // String tokenizer in C++: http://stackoverflow.com/questions/10051679/c-tokenize-string
    // * Determine how many lines over we are, in terms of the desired height of our text block
    // * Divide the text into a smaller amount by a proportion related to how many lines over we are (e.g., if we are 34 lines total, and we want 23 lines on screen, we need to reduce it by that factor)
    // * Within the tokenized list of words, figure out how many words this is, and start keeping track of offsets within the list, so that we can easily move from one fragment of the text to another
    // * This probably needs to be done as a method within the ofxTextSuite class so that we can have access to these internal members
    ofLog(OF_LOG_NOTICE, "window width: %d", ofGetWindowWidth());
    ofLog(OF_LOG_NOTICE, "window height: %d", ofGetWindowHeight());
    ofLog(OF_LOG_NOTICE, "x-width: %f", myText.defaultFont.stringWidth("x"));
    ofLog(OF_LOG_NOTICE, "y-height: %f", myText.defaultFont.stringHeight("l"));
    ofLog(OF_LOG_NOTICE, "line-height: %f", myText.defaultFont.getLineHeight());

    myText.setText(completeText);
    int numLines = myText.wrapTextX(ofGetWidth()/2 - offset);
    if (numLines > ((float) ofGetWindowHeight() / (float) myText.defaultFont.getLineHeight())) {
        ofLog(OF_LOG_NOTICE, "Too many lines!");
    }
    //ofLog(OF_LOG_NOTICE, "numLines: %d", numLines);

    i18nText.init("SourceSansPro-Regular.otf", 30);
    i18nText.setText(gettext("This is a test that ought to be translated into another language."));
    i18nText.wrapTextX(ofGetWidth()/2 - 10);
	letter = '$';
	testChar = testFont.getCharacterAsPoints(letter);
	//ofSetFullscreen(true);


    sqlite3_close(db);


}

int testApp::basicCallback(void *data, int argc, char **argv, char **azColName) {
    int i;
    ofLog(OF_LOG_NOTICE, "%s: ", (const char*)data);
    for (i = 0; i < argc; i++) {
        ofLog(OF_LOG_NOTICE, "%s = %s\n", azColName[i], argv [i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void testApp::setSyllogeCount() {
    ofxSQLiteSelect sel = sqlite->select("count(*) as total").from("sylloge");
    sel.execute().begin();
    syllogeCount = sel.getInt();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0, 0, 0);
	ofSetColor(255, 255, 255);
	ofFill();

    myText.draw(ofGetWidth()/2, 15);
    i18nText.draw(5, 15);
/*
//	ofBeginShape();
//		ofVertex(100, 100);
//		ofVertex(200, 200);
//		ofVertex(100, 200);
//	ofEndShape(true);
//
	//ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate()), 10, 10);
	//ofDrawBitmapString("press a key to see it as a texture and as a vector. ", 10, 24);

	testFont2.drawString("Hello - I am bitmap", 15, 400);

	ofFill();
	testFont2.drawStringAsShapes("Hello - I am vector", 15, 480);
	ofNoFill();
	testFont2.drawStringAsShapes("Hello - I am vector", 15, 550);

	//lets draw the key pressed as a tex and a vector both fill and no fill
	//here we show how easy it is to get

	string str = "";
	str += char(letter);

	testFont.drawString(str, 50, 250);

	//okay lets get the character back as shapes
	testChar.setFilled(true);
    testChar.draw(200,250);
    testChar.setFilled(false);
    testChar.draw(350,250);


    // we can also access the individual points
	ofFill();
	ofPushMatrix();
		ofTranslate(550, 250, 0);
		ofBeginShape();
			for(int k = 0; k <(int)testChar.getOutline().size(); k++){
				if( k!= 0)ofNextContour(true) ;
				for(int i = 0; i < (int)testChar.getOutline()[k].size(); i++){
					ofVertex(testChar.getOutline()[k].getVertices()[i].x, testChar.getOutline()[k].getVertices()[i].y);
				}
			}
		ofEndShape( true );
	ofPopMatrix();

	ofNoFill();
	ofPushMatrix();
		ofTranslate(700, 250, 0);
		ofBeginShape();
			for(int k = 0; k <(int)testChar.getOutline().size(); k++){
				if( k!= 0)ofNextContour(true) ;
				for(int i = 0; i < (int)testChar.getOutline()[k].size(); i++){
					ofVertex(testChar.getOutline()[k].getVertices()[i].x, testChar.getOutline()[k].getVertices()[i].y);
				}
			}
		ofEndShape( true );
	ofPopMatrix();
*/

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if(key==OF_KEY_ESC) return;
	testChar = testFont.getCharacterAsPoints(key);
	letter = key;
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

// CRUFT BELOW!
//
//#include <sqlite3.h>
//#include "Poco/Data/Common.h"
//#include "Poco/Data/SQLite/Connector.h"
//using namespace Poco::Data;
//using namespace std;
//    char *sql;
//    const char* msg = "Callback function called";
//    Sylloge code;

//
//    SQLite::Connector::registerConnector();
//    Session session("SQLite", "/Users/nknouf/src/sylloge_of_codes/sylloge_of_codes/sylloge_of_codes.sqlite");
//    Statement select(session);
//    select << "SELECT * FROM sylloge",
//           into(code.id),
//           into(code.code),
//           into(code.comment),
//           into(code.pseudonym),
//           into(code.datetime),
//           into(code.enabled),
//           range(0, 1);
//    
//    while (!select.done()) {
//        select.execute();
//        ofLog(OF_LOG_NOTICE, "id: %d\n", code.id);
//        ofLog(OF_LOG_NOTICE, "code: %s\n", code.code.c_str());
//        ofLog(OF_LOG_NOTICE, "comment: %s\n", code.comment.c_str());
//        ofLog(OF_LOG_NOTICE, "pseudonym: %s\n", code.pseudonym.c_str());
//        ofLog(OF_LOG_NOTICE, "datetime: %s\n", code.datetime.c_str());
//        ofLog(OF_LOG_NOTICE, "enabled: %d\n", code.enabled);
//    }
//    SQLite::Connector::unregisterConnector();

    /*
    rc = sqlite3_open("/Users/nknouf/src/sylloge_of_codes/sylloge_of_codes/sylloge_of_codes.sqlite", &db);

    if (rc) {
        ofLog(OF_LOG_NOTICE, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        ofLog(OF_LOG_NOTICE, "Opened database successfully\n");
    }

    sql = "SELECT * from sylloge";

    rc = sqlite3_exec(db, sql, basicCallback, (void*)msg, &zErrMsg);
    if (rc != SQLITE_OK) {
        ofLog(OF_LOG_ERROR, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        ofLog(OF_LOG_NOTICE, "Select done successfully\n");
    }

    sqlite3_close(db);

int testApp::basicCallback(void *data, int argc, char **argv, char **azColName) {
    int i;
    ofLog(OF_LOG_NOTICE, "%s: ", (const char*)data);
    for (i = 0; i < argc; i++) {
        ofLog(OF_LOG_NOTICE, "%s = %s\n", azColName[i], argv [i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

    */

