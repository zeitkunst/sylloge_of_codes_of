#include "sylloge_of_codes.h"

//--------------------------------------------------------------
void sylloge_of_codes::setup(){
    const string settingsFilename = "sylloge_of_codes_settings.xml";
    const string textLinesFilename = "textLines.xml";

    // Sounds
    reader.loadSound("sounds/Violet.mp3");
    reader.setVolume(0.75f);
    reader.setMultiPlay(false);

	ofBackground(255,255,255);
    currentSequenceIndex = -1;
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    ofHideCursor();
	//ofSetFullscreen(true);
    loopCounter = 0;
    syllogeDebug = SYLLOGE_DEBUG;
    syllogeSounds = SYLLOGE_SOUNDS;

    // Quick estimate of the number of words we can display onscreen at a time
    // String tokenizer in C++: http://stackoverflow.com/questions/10051679/c-tokenize-string
    // * Determine how many lines over we are, in terms of the desired height of our text block
    // * Divide the text into a smaller amount by a proportion related to how many lines over we are (e.g., if we are 34 lines total, and we want 23 lines on screen, we need to reduce it by that factor)
    // * Within the tokenized list of words, figure out how many words this is, and start keeping track of offsets within the list, so that we can easily move from one fragment of the text to another
    // * This probably needs to be done as a method within the ofxTextSuite class so that we can have access to these internal members

    // Output screen infos
    ofLog(OF_LOG_NOTICE, "window width: %d", ofGetWindowWidth());
    ofLog(OF_LOG_NOTICE, "window height: %d", ofGetWindowHeight());
    //ofLog(OF_LOG_NOTICE, "x-width: %f", myText.defaultFont.stringWidth("x"));
    //ofLog(OF_LOG_NOTICE, "y-height: %f", myText.defaultFont.stringHeight("l"));
    //ofLog(OF_LOG_NOTICE, "line-height: %f", myText.defaultFont.getLineHeight());

    // TODO
    // Set real tzd
    tzd = 5;

    //std::string s("Sat, 1 Jan 2005 12:00:00 GMT");
    //DateTime dt;
    //DateTimeParser::parse(DateTimeFormat::RFC1123_FORMAT, s, dt, tzd);
    //Poco::Timestamp ts = dt.timestamp();
    //Poco::LocalDateTime ldt(tzd, dt);

    // SETUP INTRO TEXT

    ofxTextBlock intro;
    intro.init("SourceSansPro-Black.otf", 60);
    intro.setText(gettext("sylloge of codes"));
    intro.wrapTextX(ofGetWidth() - (0.1 * ofGetWidth()));
    intro.setColor(255, 0, 0, 255);
    Segment segment;
    segment.startTime = 0.0;
    segment.delta = 0.0;
    segment.duration = 10.0;
    segment.fade = false;
    segment.textBlock = intro;
    segment.xPos = centerX(intro);
    segment.yPos = centerY(intro);
    segment.backgroundColor = ofColor::white;
    addToSequence(segment, sequence);

    textLines.loadFile(textLinesFilename);

    ofLog(OF_LOG_NOTICE, "Loading text...");
    loadTextLines(sequence);

    settings.loadFile(settingsFilename);
    string databaseLocation = settings.getValue("settings:databaseLocation", "/home/nknouf/sylloge_of_codes.sqlite");

    sqlite = new ofxSQLite(databaseLocation); 

    // Ensure that the settings are saved
    settings.setValue("settings:databaseLocation", databaseLocation);
    settings.saveFile(settingsFilename);

    // Get first random selection from database
    selectRandomCode(currentCode);

    // Annoying for string processing, but this is how it works...
    std::ostringstream stringStream;
    stringStream << DateTimeFormatter::format(currentCode.code_dt.timestamp(), "%W, %e %B %Y") << "\n" << currentCode.pseudonym << "\n" << currentCode.code;
    completeText = stringStream.str();

    // Setup random selection segment
    intro.setText(completeText);
    intro.wrapTextX(0.7 * ofGetWidth());
    intro.setColor(255, 0, 0, 255);
    segment.startTime = 0.0;
    segment.delta = 3.0;
    segment.duration = 25.0;
    segment.fade = false;
    segment.textBlock = intro;
    segment.xPos = 0.25 * ofGetWidth();
    segment.yPos = 10;
    segment.backgroundColor = ofColor::white;
    addToSequence(segment, sequence);
   
    // 23 lines visible right now...
    myText.init("SourceSansPro-Regular.otf", 30);
    myText.setText(completeText);
    int numLines = myText.wrapTextX(ofGetWidth()/2 - offset);
    if (numLines > ((float) ofGetWindowHeight() / (float) myText.defaultFont.getLineHeight())) {
        ofLog(OF_LOG_NOTICE, "Too many lines!");
    }
    //ofLog(OF_LOG_NOTICE, "numLines: %d", numLines);

    i18nText.init("SourceSansPro-Regular.otf", 30);
    i18nText.setText(gettext("This is a test that ought to be translated into another language."));
    i18nText.wrapTextX(ofGetWidth()/2 - 10);
    
    ofLog(OF_LOG_NOTICE, "Starting...");
    ofResetElapsedTimeCounter();
}

void sylloge_of_codes::addToSequence(Segment& segment, vector<Segment>& sequence) {
    Segment previousSegment;
    if (sequence.size() != 0) {
        previousSegment = sequence.at(sequence.size() - 1);
        segment.startTime = previousSegment.startTime + previousSegment.duration + segment.delta;
    }

    if (segment.fade) {
        segment.currentAlpha = 0.0;
    } else {
        segment.currentAlpha = 255.0;
    }

    // TODO
    // Not sure why this was here in the first place
    //segment.textBlock.setColor(255, 0, 0, segment.currentAlpha);
    sequence.push_back(segment);
}

void sylloge_of_codes::selectRandomCode(Sylloge& code) {
    setSyllogeCount();
    srand(time(0));
    int index = 1 + rand() % syllogeCount;

    ofxSQLiteSelect sel = sqlite->select("id, code, comments, pseudonym, code_date, enabled").from("sylloge").where("id", index);
    sel.execute().begin();

    while (sel.hasNext()) {
        code.id = sel.getInt();
        code.code = sel.getString();
        code.comments = sel.getString();
        code.pseudonym = sel.getString();
        //code.code_date = sel.getString();
        DateTimeParser::tryParse(sel.getString(), code.code_dt, tzd);
        code.enabled = (bool) sel.getInt();
        sel.next();
    }

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
    ofSoundUpdate();

    if (currentSequenceIndex != -1) {
        Segment current;
        current = sequence.at(currentSequenceIndex);
        ofBackground(current.backgroundColor.r,
                current.backgroundColor.g,
                current.backgroundColor.b);
        ofFill();
    } else {
        ofBackground(255, 255, 255);
        ofFill();
    }
}

void sylloge_of_codes::segmentFadeIn(vector<Segment>& sequence, int index) {
    sequence.at(index).textBlock.setColor(255, 0, 0, sequence.at(index).currentAlpha);
    sequence.at(index).currentAlpha = ofLerp(sequence.at(index).currentAlpha, 255, 0.08);
}

void sylloge_of_codes::loadTextLines(vector<Segment>& sequence) {
    // Go through each element in the XML file
    
    ofxTextBlock text;
    textLines.pushTag("lines");
    for (int i = 0; i < textLines.getNumTags("line"); i++) {
        textLines.pushTag("line", i);
        text.init(textLines.getValue("font", "SourceSansPro-Black.otf"), 40);
        const char* line = textLines.getValue("text", "").c_str();
        text.setText(gettext(line));
        ofLog(OF_LOG_NOTICE, ofToString(textLines.getValue("text", "")));
        text.wrapTextX(ofGetWidth() - (0.2 * ofGetWidth()));
        text.setColor(textLines.getValue("fRed", 255), textLines.getValue("fGreen", 0), textLines.getValue("fBlue", 0), 255);
        Segment segment;
        segment.startTime = 0.0;
        segment.delta = textLines.getValue("delta", 0.25);
        segment.duration = textLines.getValue("duration", 2);
        segment.fade = false;
        segment.textBlock = text;
        segment.xPos = centerX(text);
        segment.yPos = centerY(text);
        ofColor bColor;
        bColor.r = textLines.getValue("bRed", 255);
        bColor.g = textLines.getValue("bGreen", 255);
        bColor.b = textLines.getValue("bBlue", 255);
        segment.backgroundColor = bColor;
        addToSequence(segment, sequence);
        textLines.popTag();
    }
    textLines.popTag();
}

void sylloge_of_codes::resetSequence(vector<Segment>& sequence) {
    // Reset the randomly chosen code
    // TODO
    // Make the match the number of opening segments (which is right now only one)
    sequence.erase(sequence.begin() + sequence.size() - 1, sequence.begin() + sequence.size());
    //loadTextLines(sequence);
    selectRandomCode(currentCode);

    Segment newSegment;
    ofxTextBlock textBlock;
    textBlock.init("SourceSansPro-Regular.otf", 30);
    // Annoying for string processing, but this is how it works...
    std::ostringstream stringStream;
    stringStream << DateTimeFormatter::format(currentCode.code_dt.timestamp(), "%W, %e %B %Y") << "\n" << currentCode.pseudonym << "\n" << currentCode.code;
    completeText = stringStream.str();
    textBlock.setText(completeText);
    textBlock.wrapTextX(0.7 * ofGetWidth());
    textBlock.setColor(255, 0, 0, 255);
    newSegment.startTime = 0.0;
    newSegment.delta = 3.0;
    newSegment.duration = 25.0;
    newSegment.fade = true;
    newSegment.textBlock = textBlock;
    newSegment.xPos = 0.25 * ofGetWidth();
    newSegment.yPos = 10;
    addToSequence(newSegment, sequence);

    // Reset the fades
    for (unsigned int index = 0; index < sequence.size(); ++index) {
        if (sequence.at(index).fade) {
            sequence.at(index).currentAlpha = 0;
            sequence.at(index).textBlock.setColor(255, 0, 0, sequence.at(index).currentAlpha);
        }
    }
}

//--------------------------------------------------------------
void sylloge_of_codes::draw(){
	//ofBackground(255, 255, 255);
	//ofFill();


    Segment segment;
    
    for (unsigned int index = 0; index < sequence.size(); ++index) {
        //segment = sequence.at(index);
        if (sequence.at(index).startTime < ofGetElapsedTimef()) {
            if (ofGetElapsedTimef() > (sequence.at(index).startTime + sequence.at(index).duration)) {
                continue;
            } else {
                // TODO
                // Enable fading over a duration; enable fade-out
                if (sequence.at(index).fade) {
                    segmentFadeIn(sequence, index);
                }
                sequence.at(index).textBlock.draw(sequence.at(index).xPos, sequence.at(index).yPos);
                currentSequenceIndex = index;
            }
        }
    }

    if (syllogeDebug) {
        ofSetColor(0, 0, 0, 255);
        elapsedTimeString = "Elapsed time: " + ofToString(ofGetElapsedTimef());
        ofDrawBitmapString(elapsedTimeString, 10, 10);
        fpsString = "frame rate: "+ ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapString(fpsString, 10, 30);
        loopCounterString = "Loop count: "+ ofToString(loopCounter);
        ofDrawBitmapString(loopCounterString, 10, 50);
        playSoundsString = "Playing sounds: "+ ofToString(syllogeSounds);
        ofDrawBitmapString(playSoundsString, 10, 70);

    }

    // Check if we need to loop back to the beginning
    segment = sequence.at(sequence.size() - 1);
    if (ofGetElapsedTimef() > (segment.startTime + segment.duration + segment.delta)) {
        currentSequenceIndex = -1;
        ofResetElapsedTimeCounter();
        resetSequence(sequence);
        loopCounter += 1;
    }
}

//--------------------------------------------------------------
void sylloge_of_codes::keyPressed (int key){
    if ((key == 'f') || (key == 'F')) {
        // TODO
        // Make this update window boxes, layout, etc.
	    ofToggleFullscreen();
    } else if ((key == 'd') || (key == 'D')) {
       syllogeDebug = !syllogeDebug;
    } else if ((key == 's') || (key == 'S')) {
        syllogeSounds = !syllogeSounds;
    } else if ((key == 'p') || (key == 'P')) {
        reader.play();
    }
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

