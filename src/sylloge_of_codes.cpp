#include "sylloge_of_codes.h"

//--------------------------------------------------------------
void sylloge_of_codes::setup(){
    completeText = "Aenean laoreet feugiat turpis eget ultrices. Curabitur viverra aliquam neque, quis interdum augue tempor bibendum. Integer tempus non sapien ut fringilla. Suspendisse potenti. Nullam ultricies pharetra accumsan. Donec aliquam ligula orci, quis aliquam urna bibendum eu. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed in quam sed risus sodales sollicitudin. Vivamus scelerisque lacinia eros, et vulputate magna laoreet sed. Praesent ultricies elit eu accumsan ornare. Aliquam consequat viverra magna, vitae egestas lorem dictum ut.\n Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nam egestas justo felis, et condimentum diam malesuada sit amet. Donec luctus imperdiet dignissim. Sed auctor, leo ac gravida placerat, odio nibh vestibulum nisl, ut dictum tortor dui ut nulla. Curabitur scelerisque quam erat, sed faucibus mi suscipit eu. Vestibulum tortor lacus, varius et orci a, cursus tempor risus. \n Curabitur nisl tortor, elementum sagittis felis eu, pharetra accumsan purus.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc molestie nec turpis ut euismod. Cras dignissim laoreet ipsum, ut facilisis nisl. Nulla rhoncus bibendum arcu fringilla tristique. Nullam mattis fringilla odio, tincidunt ullamcorper tellus elementum nec. Pellentesque sed lacinia ipsum. Integer at magna quis ante luctus convallis. Proin non urna in nunc dictum vestibulum. Nunc adipiscing mauris ante, a commodo leo dictum et. Pellentesque aliquam magna diam, quis volutpat ante egestas id. Fusce id scelerisque purus.";

    // Load a CSV File.
    //csv.loadFile(ofToDataPath("text.csv"));
    //csv.loadFile(ofToDataPath("text.txt"), "\t");

    // Sounds
    reader.loadSound("sounds/Violet.mp3");
    reader.setVolume(0.75f);
    reader.setMultiPlay(false);

	ofBackground(255,255,255);
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
    addToSequence(segment, sequence);

    lines.loadFile(linesFilename);

    // Go through each element in the XML file
    lines.pushTag("lines");
    for (int i = 0; i < lines.getNumTags("line"); i++) {
        lines.pushTag("line", i);
        intro.init(lines.getValue("font", "SourceSansPro-Black.otf"), 40);
        intro.setText(lines.getValue("text", ""));
        ofLog(OF_LOG_NOTICE, ofToString(lines.getValue("text", "")));
        intro.wrapTextX(ofGetWidth() - (0.2 * ofGetWidth()));
        intro.setColor(lines.getValue("fRed", 0), lines.getValue("fGreen", 0), lines.getValue("fBlue", 0), 255);
        Segment segment;
        segment.startTime = 0.0;
        segment.delta = lines.getValue("delta", 0.25);
        segment.duration = lines.getValue("duration", 2);
        segment.fade = false;
        segment.textBlock = intro;
        segment.xPos = centerX(intro);
        segment.yPos = centerY(intro);
        addToSequence(segment, sequence);
        lines.popTag();
    }

//    // Go through each element in the csv file
//    for (int i = 1; i <= csv.numRows; i++) {
//        //ofLog(OF_LOG_NOTICE, "Font" + ofToString(csv.getString(i, 9)));
//        //intro.init(csv.getString(i, 9), 40);
//        intro.init("SourceSansPro-Black.otf", 40);
//        intro.setText(csv.getString(i, 0));
//        intro.wrapTextX(ofGetWidth() - (0.2 * ofGetWidth()));
//        //intro.setColor(csv.getFloat(i, 3), csv.getFloat(i, 4), csv.getFloat(i, 5), 255.0);
//        intro.setColor(ofToInt(csv.data[i][3]), ofToInt(csv.data[i][4]), ofToInt(csv.data[i][5]), 255.0);
//        Segment segment;
//        segment.startTime = 0.0;
//        segment.delta = csv.getFloat(i, 2);
//        segment.duration = csv.getFloat(i, 1);
//        segment.fade = false;
//        segment.textBlock = intro;
//        segment.xPos = centerX(intro);
//        segment.yPos = centerY(intro);
//        addToSequence(segment, sequence);
//    }


    // please connect to the "sylloge_of_codes" wifi network
    // further instructions will await you there
    // or, you can continue to watch what you see here

    intro.init("SourceSansPro-Regular.otf", 30);
    intro.setText(gettext("To the person or people within this room:\n\nThis is a time when electronic communications are surveilled the world over. Little seems to escape the computer programs and human analysts that pour over our electronic data as it traverses national boundaries through undersea cables or the wireless ether. Sent in the clear, our data is understandable by anyone with a bit of technical knowledge. So we're asked to encrypt it, to obfuscate it in some way. But these same programs and analysts have made some of these same encryption techniques weak, opening backdoors into seemingly secure communications. What we're left feeling is helplessness, a distinct lack of power to respond, and a dangerous level of cynicism."));
    //intro.setText(gettext("Consider this an invitation. An invitation to develop new codes for communication. In the wake of the revelations that the United States' National Security Agency (NSA) and the United Kingdom's General Communications Headquarters (GCHQ) monitor large swaths of our online communications, we cannot explicitly trust that what we think is safe from eavesdropping actually is."));
    intro.wrapTextX(0.7 * ofGetWidth());
    intro.setColor(255, 0, 0, 255);
    segment.startTime = 0.0;
    segment.delta = 3.0;
    segment.duration = 15.0;
    segment.fade = false;
    segment.textBlock = intro;
    segment.xPos = 0.25 * ofGetWidth();
    segment.yPos = 10;
    addToSequence(segment, sequence);

    settings.loadFile(settingsFilename);
    string databaseLocation = settings.getValue("settings:databaseLocation", "/Users/nknouf/Dropbox/projects/sylloge_of_codes/web/sylloge_of_codes/sylloge_of_codes/sylloge_of_codes.sqlite");


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

    segment.textBlock.setColor(255, 0, 0, segment.currentAlpha);
    sequence.push_back(segment);
}

void sylloge_of_codes::selectRandomCode(Sylloge& code) {
    setSyllogeCount();
    int index = rand() % syllogeCount;

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
}

void sylloge_of_codes::segmentFadeIn(vector<Segment>& sequence, int index) {
    sequence.at(index).textBlock.setColor(255, 0, 0, sequence.at(index).currentAlpha);
    sequence.at(index).currentAlpha = ofLerp(sequence.at(index).currentAlpha, 255, 0.08);
}

void sylloge_of_codes::resetSequence(vector<Segment>& sequence) {
    // Reset the randomly chosen code
    // TODO
    // Make the match the number of opening segments (which is right now only two)
    sequence.erase(sequence.begin() + 2, sequence.begin() + sequence.size());
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
    for (int index = 0; index < sequence.size(); ++index) {
        if (sequence.at(index).fade) {
            sequence.at(index).currentAlpha = 0;
            sequence.at(index).textBlock.setColor(255, 0, 0, sequence.at(index).currentAlpha);
        }
    }
}

//--------------------------------------------------------------
void sylloge_of_codes::draw(){
	ofBackground(255, 255, 255);
	ofFill();

    if (syllogeDebug) {
        elapsedTimeString = "Elapsed time: " + ofToString(ofGetElapsedTimef());
        ofDrawBitmapString(elapsedTimeString, 10, 10);
        fpsString = "frame rate: "+ ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapString(fpsString, 10, 30);
        loopCounterString = "Loop count: "+ ofToString(loopCounter);
        ofDrawBitmapString(loopCounterString, 10, 50);
        playSoundsString = "Playing sounds: "+ ofToString(syllogeSounds);
        ofDrawBitmapString(playSoundsString, 10, 70);

    }

    Segment segment;
    
    for (int index = 0; index < sequence.size(); ++index) {
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
            }
        }
    }

    // Check if we need to loop back to the beginning
    segment = sequence.at(sequence.size() - 1);
    if (ofGetElapsedTimef() > (segment.startTime + segment.duration + segment.delta)) {
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

