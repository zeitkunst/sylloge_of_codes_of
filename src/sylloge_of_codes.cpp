#include "sylloge_of_codes.h"

//--------------------------------------------------------------
void sylloge_of_codes::setup(){
    completeText = "Aenean laoreet feugiat turpis eget ultrices. Curabitur viverra aliquam neque, quis interdum augue tempor bibendum. Integer tempus non sapien ut fringilla. Suspendisse potenti. Nullam ultricies pharetra accumsan. Donec aliquam ligula orci, quis aliquam urna bibendum eu. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed in quam sed risus sodales sollicitudin. Vivamus scelerisque lacinia eros, et vulputate magna laoreet sed. Praesent ultricies elit eu accumsan ornare. Aliquam consequat viverra magna, vitae egestas lorem dictum ut.\n Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nam egestas justo felis, et condimentum diam malesuada sit amet. Donec luctus imperdiet dignissim. Sed auctor, leo ac gravida placerat, odio nibh vestibulum nisl, ut dictum tortor dui ut nulla. Curabitur scelerisque quam erat, sed faucibus mi suscipit eu. Vestibulum tortor lacus, varius et orci a, cursus tempor risus. \n Curabitur nisl tortor, elementum sagittis felis eu, pharetra accumsan purus.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc molestie nec turpis ut euismod. Cras dignissim laoreet ipsum, ut facilisis nisl. Nulla rhoncus bibendum arcu fringilla tristique. Nullam mattis fringilla odio, tincidunt ullamcorper tellus elementum nec. Pellentesque sed lacinia ipsum. Integer at magna quis ante luctus convallis. Proin non urna in nunc dictum vestibulum. Nunc adipiscing mauris ante, a commodo leo dictum et. Pellentesque aliquam magna diam, quis volutpat ante egestas id. Fusce id scelerisque purus.";

    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    ofHideCursor();
	//ofSetColor(255, 255, 255, 255);
    alpha = 0;

    CodeDuration code;
    code.code = "Hello World";
    code.duration = 5.0;
    codes.push_back(code);
    code.code = "This is a test";
    code.duration = 5.0;
    codes.push_back(code);
    code.code = completeText;
    code.duration = 5.0;
    codes.push_back(code);

    std::string s("Sat, 1 Jan 2005 12:00:00 GMT");
    int tzd;
    DateTime dt;
    DateTimeParser::parse(DateTimeFormat::RFC1123_FORMAT, s, dt, tzd);
    Poco::Timestamp ts = dt.timestamp();
    Poco::LocalDateTime ldt(tzd, dt);

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

    ofxTextBlock intro;
    intro.init("SourceSansPro-Black.otf", 60);
    intro.setText(gettext("sylloge of codes"));
    intro.wrapTextX(ofGetWidth() - (0.1 * ofGetWidth()));
    intro.setColor(255, 255, 255, 255);
    Segment segment;
    segment.startTime = 0.0;
    segment.delta = 0.0;
    segment.duration = 10.0;
    segment.fadeDuration = 0.1;
    segment.fade = true;
    segment.textBlock = intro;
    segment.xPos = centerX(intro);
    segment.yPos = centerY(intro);
    addToSequence(segment, sequence);


    intro.init("AJensonPro-Regular.otf", 30);
    intro.setText(gettext("Consider this an invitation. An invitation to develop new codes for communication. In the wake of the revelations that the United States' National Security Agency (NSA) and the United Kingdom's General Communications Headquarters (GCHQ) monitor large swaths of our online communications, we cannot explicitly trust that what we think is safe from eavesdropping actually is."));
    intro.wrapTextX(0.7 * ofGetWidth());
    intro.setColor(255, 255, 255, 255);
    segment.startTime = 0.0;
    segment.delta = 3.0;
    segment.duration = 15.0;
    segment.fade = true;
    segment.textBlock = intro;
    segment.xPos = 0.25 * ofGetWidth();
    segment.yPos = 10;
    addToSequence(segment, sequence);

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
}

void sylloge_of_codes::addToSequence(Segment& segment, vector<Segment>& sequence) {
    ofLog(OF_LOG_NOTICE, "sequence length: %d", sequence.size());
    
    Segment previousSegment;
    if (sequence.size() != 0) {
        previousSegment = sequence.at(sequence.size() - 1);
        segment.startTime = previousSegment.startTime + previousSegment.duration + segment.delta;
    }

    if (segment.fadeDuration != 0) {
        segment.currentAlpha = 0.0;
    } else {
        segment.currentAlpha = 255.0;
    }
    sequence.push_back(segment);
}

float sylloge_of_codes::centerX(ofxTextBlock textBlock) {
    float textWidth = textBlock.getWidth();
    float textCenter = textWidth / 2.0f;

    return (ofGetWindowWidth()/2 - textCenter);
}

float sylloge_of_codes::centerY(ofxTextBlock textBlock) {
    float textHeight = textBlock.getHeight();
    float textCenter = textHeight/ 2.0f;

    return (ofGetWindowHeight()/2 - textCenter);
}

void sylloge_of_codes::setSyllogeCount() {
    ofxSQLiteSelect sel = sqlite->select("count(*) as total").from("sylloge");
    sel.execute().begin();
    syllogeCount = sel.getInt();
}

//--------------------------------------------------------------
void sylloge_of_codes::update(){

}

void sylloge_of_codes::segmentFadeIn(Segment& segment) {
    segment.currentAlpha = ofLerp(segment.currentAlpha, 255, 0.02);
    segment.textBlock.setColor(255, 255, 255, segment.currentAlpha);
}

//--------------------------------------------------------------
void sylloge_of_codes::draw(){
	ofBackground(0, 0, 0);
	ofFill();

    if (SYLLOGE_DEBUG) {
        elapsedTimeString = "Elapsed time: " + ofToString(ofGetElapsedTimef());
        ofDrawBitmapString(elapsedTimeString, 10, 10);
    }

    Segment segment;
    for (int index = 0; index < sequence.size(); ++index) {
        segment = sequence.at(index);
        if (segment.startTime < ofGetElapsedTimef()) {
            if (ofGetElapsedTimef() > (segment.startTime + segment.duration)) {
                continue;
            } else {
                // TODO
                // Fix fading in and out
                //if (segment.fade) {
                //    segmentFadeIn(segment);
                //}
                segment.textBlock.draw(segment.xPos, segment.yPos);
            }
        }
    }

    // Check if we need to loop back to the beginning
    segment = sequence.at(sequence.size() - 1);
    if (ofGetElapsedTimef() > (segment.startTime + segment.duration + segment.delta)) {
        ofResetElapsedTimeCounter();
    }

//    // To fade in and out: need to figure out way to draw text with an alpha
//    // Can also use this to calculate incremental changes: ofLerp()
//    alpha = ofLerp(alpha, 255, 0.02);
//	myText.setColor(255, 255, 255, alpha);
//	i18nText.setColor(255, 255, 255, alpha);
//
//    // Here's the sequence:
//    myText.draw(ofGetWidth()/2, 15);
//    i18nText.draw(5, 15);

}

//--------------------------------------------------------------
void sylloge_of_codes::keyPressed (int key){
}

//--------------------------------------------------------------
void sylloge_of_codes::keyReleased  (int key){
}

//--------------------------------------------------------------
void sylloge_of_codes::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void sylloge_of_codes::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void sylloge_of_codes::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void sylloge_of_codes::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void sylloge_of_codes::windowResized(int w, int h){

}

//--------------------------------------------------------------
void sylloge_of_codes::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void sylloge_of_codes::dragEvent(ofDragInfo dragInfo){

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

int sylloge_of_codes::basicCallback(void *data, int argc, char **argv, char **azColName) {
    int i;
    ofLog(OF_LOG_NOTICE, "%s: ", (const char*)data);
    for (i = 0; i < argc; i++) {
        ofLog(OF_LOG_NOTICE, "%s = %s\n", azColName[i], argv [i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

    */

