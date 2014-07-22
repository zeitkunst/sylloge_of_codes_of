#include <sstream>
#include <iostream>

#include "sylloge_of_codes.h"

using namespace std;

//--------------------------------------------------------------
void sylloge_of_codes::setup(){
#ifdef FTGLES
    // For FTGLES
    font.loadFont("SourceSansPro-Black.otf", 36, true, true);        
    font.setLineLength(0.8 * ofGetWindowWidth());
#else
    font.loadFont("SourceSansPro-Black.otf", 56);
    font.setLineLength(0.85 * ofGetWindowWidth());
#endif

    // Setup settings filenames
    const string settingsFilename = "sylloge_of_codes_settings.xml";
    const string textLinesFilename = "textLines.xml";

    // Misc setup
    frameRate = 30;
    drawNow = false;
    currentSequenceIndex = 0;
    loopCounter = 1;
    syllogeDebug = SYLLOGE_DEBUG;
    skipIntro = true;
    codeFragmentsAdded = 0;

	ofBackground(255,255,255);
    ofSetFrameRate(frameRate);
    ofEnableAlphaBlending();
    ofHideCursor();

    // Quick estimate of the number of words we can display onscreen at a time
    // String tokenizer in C++: http://stackoverflow.com/questions/10051679/c-tokenize-string
    // * Determine how many lines over we are, in terms of the desired height of our text block
    // * Divide the text into a smaller amount by a proportion related to how many lines over we are (e.g., if we are 34 lines total, and we want 23 lines on screen, we need to reduce it by that factor)
    // * Within the tokenized list of words, figure out how many words this is, and start keeping track of offsets within the list, so that we can easily move from one fragment of the text to another
    // * This probably needs to be done as a method within the ofxTextSuite class so that we can have access to these internal members

    // Output screen infos
    ofLog(OF_LOG_NOTICE, "window width: %d", ofGetWindowWidth());
    ofLog(OF_LOG_NOTICE, "window height: %d", ofGetWindowHeight());

    // TODO
    // Set real tzd
    tzd = 5;

    //std::string s("Sat, 1 Jan 2005 12:00:00 GMT");
    //DateTime dt;
    //DateTimeParser::parse(DateTimeFormat::RFC1123_FORMAT, s, dt, tzd);
    //Poco::Timestamp ts = dt.timestamp();
    //Poco::LocalDateTime ldt(tzd, dt);

    /*
    // SETUP INTRO TEXT
    TextLine introText;
    introText.text = gettext("sylloge of codes");
    introText.font = "SourceSansPro-Black.otf";
    introText.fontSize = 56.0;
    introText.fColor = ofColor(255.0, 0.0, 0.0);
    introText.bColor = ofColor(255.0, 255.0, 255.0);
    introText.fade = false;
    introText.startTime = 0.0;
    introText.duration = 10.0; // original, desired duration
    //introText.duration = 1.0; // testing duration
    introText.delta = 2.0;
    introText.xPos = centerX(font.stringWidth(introText.text));
    introText.yPos = centerY(font.stringHeight(introText.text));
    addToSequence(introText, sequence);
    */

    // Read our XML file
    textLines.loadFile(textLinesFilename);

    // Load the file into our structure
    if (!skipIntro) {
        ofLog(OF_LOG_NOTICE, "Loading text...");
        loadTextLines(sequence);
    }

    settings.loadFile(settingsFilename);
    string databaseLocation = settings.getValue("settings:databaseLocation", "/home/nknouf/sylloge_of_codes.sqlite");

    sqlite = new ofxSQLite(databaseLocation); 

    // Ensure that the settings are saved
    settings.setValue("settings:databaseLocation", databaseLocation);
    settings.saveFile(settingsFilename);

    // Get first random selection from database
    selectRandomCode(currentCode);
    addCodeToSequence(currentCode);

    currentTextLine = sequence.at(currentSequenceIndex);
    font.setSize(currentTextLine.fontSize);
    ofRectangle rect = font.getStringBoundingBox(currentTextLine.text, 0, 0);
    
    float width = rect.width;
    float height = rect.height;
    ofLog(OF_LOG_NOTICE, ofToString(width));
    ofLog(OF_LOG_NOTICE, ofToString(height));

    sequence.at(currentSequenceIndex).xPos = (ofGetWindowWidth()/2) - (width/2);
    sequence.at(currentSequenceIndex).yPos = (ofGetWindowHeight()/2) + (height/2);


    
    ofLog(OF_LOG_NOTICE, "Starting sylloge of codes...");
    setLastTime(sequence);
    
    ofResetElapsedTimeCounter();
    ofLog(OF_LOG_NOTICE, "Sequence size at end of setup: " + ofToString(sequence.size()));
}

void sylloge_of_codes::addToSequence(TextLine& textLine, vector<TextLine>& sequence) {
    TextLine previousTextLine;
    if (sequence.size() != 0) {
        previousTextLine = sequence.at(sequence.size() - 1);
        textLine.startTime = previousTextLine.startTime + previousTextLine.duration + previousTextLine.delta;
    } else {
        textLine.startTime = 0;
    }

    if (textLine.fade) {
        textLine.currentAlpha = 0.0;
    } else {
        textLine.currentAlpha = 255.0;
    }

    // TODO
    // Not sure why this was here in the first place
    //textLine.textBlock.setColor(255, 0, 0, textLine.currentAlpha);
    sequence.push_back(textLine);
}

void sylloge_of_codes::selectRandomCode(Sylloge& code) {
    setSyllogeCount();
    srand(time(0));
    int index = 1 + rand() % syllogeCount;

    ofxSQLiteSelect sel = sqlite->select("id, code, comments, pseudonym, code_date, enabled").from("sylloge").where("id", 46);
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

float sylloge_of_codes::centerX(float stringWidth) {
    float textCenter = stringWidth / 2.0f;

    //return (ofGetWindowWidth()/2 - textCenter);
    return (ofGetWindowWidth()/2 - textCenter);
}

float sylloge_of_codes::centerY(float stringHeight) {
    //float textCenter = stringHeight/ 2.0f;

    return (ofGetWindowHeight()/2);
}

void sylloge_of_codes::setSyllogeCount() {
    ofxSQLiteSelect sel = sqlite->select("count(*) as total").from("sylloge");
    sel.execute().begin();
    syllogeCount = sel.getInt();
}

//--------------------------------------------------------------
void sylloge_of_codes::update(){
    unsigned int currentFrame = ofGetFrameNum();

    currentTextLine = sequence.at(currentSequenceIndex);
    float startTime = currentTextLine.startTime;
    float duration = currentTextLine.duration;
    float delta = currentTextLine.delta;

    // Set color
    ofBackground(currentTextLine.bColor.r,
            currentTextLine.bColor.g,
            currentTextLine.bColor.b);
    ofFill();

    // Determine draw or not
    if (currentFrame < (startTime + duration)) {
        drawNow = true;
    } else if ((currentFrame >= (startTime + duration)) && (currentFrame < (startTime + duration + delta))) {
        drawNow = false;
    } else if (currentFrame >= (startTime + duration + delta)) {
        currentSequenceIndex += 1;
        
        if (currentSequenceIndex != sequence.size()) {
            currentTextLine = sequence.at(currentSequenceIndex);
            font.setSize(currentTextLine.fontSize);
            ofRectangle rect = font.getStringBoundingBox(currentTextLine.text, 0, 0);
            
            float width = rect.width;
            float height = rect.height;
            ofLog(OF_LOG_NOTICE, ofToString(width));
            ofLog(OF_LOG_NOTICE, ofToString(height));
    
            sequence.at(currentSequenceIndex).xPos = (ofGetWindowWidth()/2) - (width/2);
            sequence.at(currentSequenceIndex).yPos = (ofGetWindowHeight()/2) + (height/2);
        }


    } 
    
    if (currentFrame >= lastTime) {
        currentSequenceIndex = 0;
        ofResetElapsedTimeCounter();
        resetSequence(sequence);
        loopCounter += 1;
    }
}


void sylloge_of_codes::setLastTime(vector<TextLine>& sequence) {
    ofLog(OF_LOG_NOTICE, "sequence size in setLastTime: " + ofToString(sequence.size()));
    TextLine lastLine = sequence.at(sequence.size() - 1);

    lastTime = (unsigned long long) (lastLine.startTime + lastLine.duration + lastLine.delta);
    ofLog(OF_LOG_NOTICE, "lastTime is: " + ofToString(lastTime));
}

void sylloge_of_codes::segmentFadeIn(vector<TextLine>& sequence, int index) {
    TextLine current;
    current = sequence.at(index);
#ifdef FTGLES
    font.setColor(current.fColor.r, current.fColor.g, current.fColor.b, current.currentAlpha);
#else
    ofSetColor(current.fColor.r, current.fColor.g, current.fColor.b, current.currentAlpha);
#endif
    sequence.at(index).currentAlpha = ofLerp(sequence.at(index).currentAlpha, 255, 0.08);
}

void sylloge_of_codes::addCodeToSequence(Sylloge& code) {
    string completeText;

    // Setup all of our properties, except for the text
    TextLine dbText;
    dbText.font = "SourceSansPro-Black.otf";
    dbText.fontSize = 30.0;
    dbText.fColor = ofColor(255.0, 0.0, 0.0);
    dbText.bColor = ofColor(255.0, 255.0, 255.0);
    dbText.startTime = 0.0;
    dbText.delta = floor(1.0 * frameRate);;
    dbText.duration = floor(1.0 * frameRate);
    dbText.fade = false;

    // Set the font to be the correct size
    font.setSize(dbText.fontSize);

    // Vector of our appropriately sized lines
    vector<string> outputLines;
    outputLines.push_back(DateTimeFormatter::format(code.code_dt.timestamp(), "%W, %e %B %Y"));
    outputLines.push_back(code.pseudonym);

    /*
    StringTokenizer st(code.code, "\n");

    for (int i = 0; i < st.count(); i++) {
        ofLog(OF_LOG_NOTICE, st[i]);
    }
    */
  
    // Split by space
    // from http://stackoverflow.com/questions/19137617/c-function-split-string-into-words 
    vector<string> words;
    istringstream iss(code.code);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(words)); 
    
    /*
    for (int i = 0; i < words.size(); i++) {
        ofLog(OF_LOG_NOTICE, words.at(i));
    }
    */

    // Okay, let's start combining our words together again into appropriately lengthed paragraphs
    // TODO
    // Still have to deal with newlines in the input text
    vector<string> currentWords;
    vector<string> currentParagraphs;

    if (words.size() > 0) {
        int runningChars = 0;
        currentWords.clear();
        currentParagraphs.clear();
        const char* const delim = " ";
        ostringstream imploded;

        int maxChars = 300; 
        for(int i = 0; i < words.size(); i++) {
            runningChars += words.at(i).size() + 1;

            if (runningChars <= maxChars) {
                currentWords.push_back(words.at(i));
            } else {
                imploded.clear();
                imploded.str("");
                copy(currentWords.begin(), currentWords.end(), ostream_iterator<string>(imploded, delim));
                currentParagraphs.push_back(imploded.str());

                currentWords.clear();
                currentWords.push_back(words.at(i));

                runningChars = 0;
            }
            
        }

        // last line
        imploded.clear();
        imploded.str("");
        copy(currentWords.begin(), currentWords.end(), ostream_iterator<string>(imploded, delim));
        currentParagraphs.push_back(imploded.str());
    }

    for (int i = 0; i < currentParagraphs.size(); i++) {
        //ofLog(OF_LOG_NOTICE, currentParagraphs.at(i));

        // Annoying for string processing, but this is how it works...
        ostringstream oStringStream;
        oStringStream << DateTimeFormatter::format(code.code_dt.timestamp(), "%W, %e %B %Y") << "\n" << code.pseudonym << "\n" << currentParagraphs.at(i);
        completeText = oStringStream.str();
    
        //dbText.xPos = 0.25 * ofGetWidth();
        //dbText.yPos = 10;
        dbText.text = completeText;
        addToSequence(dbText, sequence);

    }

    codeFragmentsAdded = sequence.size();


}

void sylloge_of_codes::loadTextLines(vector<TextLine>& sequence) {
    // Go through each element in the XML file
  
    TextLine textLine;

    textLines.pushTag("lines");
    for (int i = 0; i < textLines.getNumTags("line"); i++) {
        textLines.pushTag("line", i);

        textLine.font = textLines.getValue("font", "SourceSansPro-Black.otf");
        textLine.fontSize = textLines.getValue("fontSize", 48.0);
        const char *line = textLines.getValue("text", "").c_str();
        textLine.text = gettext(line);
        textLine.xPos = centerX(font.stringWidth(textLine.text));
        textLine.yPos = centerY(font.stringHeight(textLine.text));
        textLine.fColor = ofColor(textLines.getValue("fRed", 255), textLines.getValue("fGreen", 0), textLines.getValue("fBlue", 0), 255);
        textLine.bColor = ofColor(textLines.getValue("bRed", 255), textLines.getValue("bGreen", 255), textLines.getValue("bBlue", 255), 255);

        textLine.startTime = 0.0;
        textLine.duration = floor(textLines.getValue("duration", 2.0) * frameRate);
        textLine.delta = floor(textLines.getValue("delta", 0.25) * frameRate);
        textLine.fade = false;
        /*
        ofColor bColor;
        bColor.r = textLines.getValue("bRed", 255);
        bColor.g = textLines.getValue("bGreen", 255);
        bColor.b = textLines.getValue("bBlue", 255);
        segment.backgroundColor = bColor;
        */
        addToSequence(textLine, sequence);
        textLines.popTag();
    }
    textLines.popTag();
}

void sylloge_of_codes::resetSequence(vector<TextLine>& sequence) {
    ofLog(OF_LOG_NOTICE, "in reset sequence");
    // Reset the randomly chosen code
    // TODO
    // Make the match the number of opening segments (which is right now only one)
    sequence.erase(sequence.begin() + sequence.size() - codeFragmentsAdded, sequence.begin() + sequence.size());
    ofLog(OF_LOG_NOTICE, "Sequence size after erase: " + ofToString(sequence.size()));

    // TODO
    // Ensure that we also add the selected random code to the sequence
    // Also ensure that it's the appropriate number of lines
    selectRandomCode(currentCode);
    addCodeToSequence(currentCode);
    setLastTime(sequence);

    // Reset the fades
    for (unsigned int index = 0; index < sequence.size(); ++index) {
        if (sequence.at(index).fade) {
            sequence.at(index).currentAlpha = 0;
            //sequence.at(index).textBlock.setColor(255, 0, 0, sequence.at(index).currentAlpha);
        }
    }
}

//--------------------------------------------------------------
void sylloge_of_codes::draw() {
    if (syllogeDebug) {
        ofSetColor(0, 0, 0, 255);
        elapsedTimeString = "Elapsed time: " + ofToString(ofGetElapsedTimef());
        ofDrawBitmapString(elapsedTimeString, 10, 10);
        fpsString = "frame rate: "+ ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapString(fpsString, 10, 30);
        loopCounterString = "Loop count: "+ ofToString(loopCounter);
        ofDrawBitmapString(loopCounterString, 10, 50);
    }

    currentTextLine = sequence.at(currentSequenceIndex);

    if (drawNow) {
        if (currentTextLine.fade) {
            segmentFadeIn(sequence, currentSequenceIndex);
        }
#ifdef FTGLES
        font.setColor(currentTextLine.fColor.r, currentTextLine.fColor.g, currentTextLine.fColor.b, currentTextLine.currentAlpha);
#else
        ofSetColor(currentTextLine.fColor.r, currentTextLine.fColor.g, currentTextLine.fColor.b, currentTextLine.currentAlpha);
#endif
        font.drawString(currentTextLine.text, currentTextLine.xPos, currentTextLine.yPos);
    }

}

/* ORIGINAL DRAW METHOD
void sylloge_of_codes::draw(){
    TextLine textLine;
    
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
                textLine = sequence.at(index);
#ifdef FTGLES
                font.setColor(textLine.fColor.r, textLine.fColor.g, textLine.fColor.b, textLine.currentAlpha);
#else
                ofSetColor(textLine.fColor.r, textLine.fColor.g, textLine.fColor.b, textLine.currentAlpha);
#endif
                //font.drawString(textLine.text, textLine.xPos, textLine.yPos);
                //ofRectangle foo = font.getStringBoundingBox(textLine.text, 0, textLine.yPos);
                //ofLog(OF_LOG_NOTICE, ofToString(foo));
                font.setSize(textLine.fontSize);
                font.drawString(textLine.text, textLine.xPos, textLine.yPos);
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
    }

    // Check if we need to loop back to the beginning
    textLine = sequence.at(sequence.size() - 1);
    if (ofGetElapsedTimef() > (textLine.startTime + textLine.duration + textLine.delta)) {
        currentSequenceIndex = -1;
        ofResetElapsedTimeCounter();
        resetSequence(sequence);
        loopCounter += 1;
    }
}
*/

//--------------------------------------------------------------
void sylloge_of_codes::keyPressed (int key){
    if ((key == 'f') || (key == 'F')) {
        // TODO
        // Make this update window boxes, layout, etc.
	    ofToggleFullscreen();
    } else if ((key == 'd') || (key == 'D')) {
       syllogeDebug = !syllogeDebug;
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

