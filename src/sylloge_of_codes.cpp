#include <sstream>
#include <iostream>
#include <iterator>
#include <cmath>
#include <algorithm>

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

    // Current hack for getting and setting locale, given that we don't have a 1-1 match between English lines and other languages lines
    string textLinesFilename = "textLines.xml";
    string lang = getenv("LANG");
    if (lang.substr(0, 2) == "en") {
        textLinesFilename = "textLines.xml";
        locale = "en";
    } else if (lang.substr(0, 5) == "es_CL") {
        textLinesFilename = "textLines_es_CL.xml";
        locale = "es_CL";
    }

    // Misc setup
    frameRate = 30;
    drawNow = false;
    currentSequenceIndex = 0;
    loopCounter = 1;
    syllogeDebug = SYLLOGE_DEBUG;
    skipIntro = false;
    codeFragmentsAdded = 0;

	ofBackground(255,255,255);
    ofSetFrameRate(frameRate);
    ofEnableAlphaBlending();
    ofHideCursor();
    ofSeedRandom(time(0));

    // Output screen infos
    ofLog(OF_LOG_NOTICE, "window width: %d", ofGetWindowWidth());
    ofLog(OF_LOG_NOTICE, "window height: %d", ofGetWindowHeight());

    // TODO
    // Set real tzd based on system locale
    //
    // Right now we hack this by hand.
    // Currently it's 4; will change back to 5 when Chile goes to DST. But it shouldn't affect the output of things, since we're not outputing the time, only the date
    tzd = 4;

    //std::string s("Sat, 1 Jan 2005 12:00:00 GMT");
    //DateTime dt;
    //DateTimeParser::parse(DateTimeFormat::RFC1123_FORMAT, s, dt, tzd);
    //Poco::Timestamp ts = dt.timestamp();
    //Poco::LocalDateTime ldt(tzd, dt);


    settings.loadFile(settingsFilename);
    string databaseLocation = settings.getValue("settings:databaseLocation", "/home/nknouf/sylloge_of_codes.sqlite");

    // ensure we also have something to display
    TextLine initialLine;  
    initialLine.font = "SourceSansPro-Black.otf";
    initialLine.fontSize = 100.0;
    if (locale == "en") {
        initialLine.text = gettext("sylloge of codes");
    } else if (locale == "es_CL") {
        initialLine.text = gettext("sylloge de códigos");

    }

    font.setSize(initialLine.fontSize);
    ofRectangle rect = font.getStringBoundingBox(initialLine.text, 0, 0);
            
    float width = rect.width;
    float height = rect.height;
    
    initialLine.xPos = (ofGetWindowWidth()/2) - (width/2);
    initialLine.yPos = (ofGetWindowHeight()/2) + (height/2);

    initialLine.fColor = ofColor(255, 0, 0, 255);
    initialLine.bColor = ofColor(255, 255, 255, 255);

    initialLine.startTime = 0;
    initialLine.duration = 10 * 1000;
    initialLine.delta = 2 * 1000;
    initialLine.fade = false;
    addToSequence(initialLine, sequence);

    // Second line
    if (locale == "en") {
        initialLine.text = gettext("a project by\nnicholas knouf");
    } else if (locale == "es_CL") {
        initialLine.text = gettext("un proyecto por\nnicholas knouf");
    }

    initialLine.fontSize = 60;

    font.setSize(initialLine.fontSize);
    rect = font.getStringBoundingBox(initialLine.text, 0, 0);
            
    width = rect.width;
    height = rect.height;
    
    initialLine.xPos = (ofGetWindowWidth()/2) - (width/2);
    initialLine.yPos = (ofGetWindowHeight()/2) + (height/2);

    initialLine.fColor = ofColor(255, 0, 0, 255);
    initialLine.bColor = ofColor(255, 255, 255, 255);

    initialLine.startTime = 0;
    initialLine.duration = 5 * 1000;
    initialLine.delta = 2 * 1000;
    initialLine.fade = false;
    addToSequence(initialLine, sequence);

    // Third line
    if (locale == "en") {
        initialLine.text = gettext("spanish translation by claudia pederson");
    } else if (locale == "es_CL") {
        initialLine.text = gettext("traducido por claudia pederson");
    }

    initialLine.fontSize = 60;

    font.setSize(initialLine.fontSize);
    rect = font.getStringBoundingBox(initialLine.text, 0, 0);
            
    width = rect.width;
    height = rect.height;
    
    initialLine.xPos = (ofGetWindowWidth()/2) - (width/2);
    initialLine.yPos = (ofGetWindowHeight()/2) + (height/2);

    initialLine.fColor = ofColor(255, 0, 0, 255);
    initialLine.bColor = ofColor(255, 255, 255, 255);

    initialLine.startTime = 0;
    initialLine.duration = 5 * 1000;
    initialLine.delta = 2 * 1000;
    initialLine.fade = false;
    addToSequence(initialLine, sequence);

    // Read our XML file
    textLines.loadFile(textLinesFilename);

    // Load the file into our structure
    if (!skipIntro) {
        ofLog(OF_LOG_NOTICE, "Loading text from " + textLinesFilename); 
        loadTextLines(sequence);
    }

    // Setup database infos
    sqlite = new ofxSQLite(databaseLocation); 

    // Ensure that the settings are saved
    settings.setValue("settings:databaseLocation", databaseLocation);
    settings.saveFile(settingsFilename);

    // Update our random code
    updateRandomCode();

    // Ensure that things are set for the first runthrough
    currentTextLine = sequence.at(currentSequenceIndex);
    font.setSize(currentTextLine.fontSize);

    ofLog(OF_LOG_NOTICE, "Starting sylloge of codes...");
    setLastTime(sequence);
    
    ofResetElapsedTimeCounter();
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

    sequence.push_back(textLine);
}

void sylloge_of_codes::selectRandomCode(Sylloge& code) {
    float val = ofRandom(enabledIDs.size());
    int index = floor(val);

    ofxSQLiteSelect sel = sqlite->select("id, code, comments, pseudonym, code_date, enabled").from("sylloge").where("id", enabledIDs.at(index));
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

void sylloge_of_codes::updateRandomCode() {
    setSyllogeEnabled();
    
    if (enabledIDs.size() != 0) {
        selectRandomCode(currentCode);
        addCodeToSequence(currentCode);
    }

    setLastTime(sequence);

}

void sylloge_of_codes::setSyllogeEnabled() {
    enabledIDs.clear();
    ofxSQLiteSelect sel = sqlite->select("id").from("sylloge").where("enabled", "1").execute().begin();

    while (sel.hasNext()) {
        int id = sel.getInt();
        enabledIDs.push_back(id);
        sel.next();
    }

}

//--------------------------------------------------------------
void sylloge_of_codes::update(){
    unsigned long long currentTime = ofGetElapsedTimeMillis();
    
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
    if (currentTime < (startTime + duration)) {
        drawNow = true;
    } else if ((currentTime >= (startTime + duration)) && (currentTime < (startTime + duration + delta))) {
        drawNow = false;
    } else if (currentTime >= (startTime + duration + delta)) {
        currentSequenceIndex += 1;
        
        if (currentSequenceIndex != sequence.size()) {
            currentTextLine = sequence.at(currentSequenceIndex);
            font.setSize(currentTextLine.fontSize);
        }


    } 
    
    if (currentTime >= lastTime) {
        ofLog(OF_LOG_NOTICE, "Time difference: " + ofToString(currentTime - lastTime));
        currentSequenceIndex = 0;
        ofResetElapsedTimeCounter();
        resetSequence(sequence);
        loopCounter += 1;
    }
}


void sylloge_of_codes::setLastTime(vector<TextLine>& sequence) {
    TextLine lastLine = sequence.at(sequence.size() - 1);

    lastTime = (unsigned long long) (lastLine.startTime + lastLine.duration + lastLine.delta);
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
    float standardFontSize = 30.0;
    TextLine dbText;
    dbText.font = "SourceSansPro-Black.otf";
    dbText.fontSize = standardFontSize;
    dbText.fColor = ofColor(255.0, 0.0, 0.0);
    dbText.bColor = ofColor(255.0, 255.0, 255.0);
    dbText.startTime = 0;
    dbText.duration = 30.0 * 1000;
    dbText.delta = 1.0 * 1000;
    dbText.fade = false;

    // Set the font to be the correct size
    font.setSize(dbText.fontSize);

    // Vector of our appropriately sized lines
    vector<string> outputLines;
    outputLines.push_back(DateTimeFormatter::format(code.code_dt.timestamp(), "%W, %e %B %Y"));
    outputLines.push_back(code.pseudonym);

    // Split by space
    // from http://stackoverflow.com/questions/19137617/c-function-split-string-into-words 

    // Replace newlines with pilcrow, as it's eaiser than counting lines, etc.
    Poco::replaceInPlace(code.code, "\n", " \u00b6 ");
    vector<string> words;
    istringstream iss(code.code);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(words)); 
    

    // Okay, let's start combining our words together again into appropriately lengthed paragraphs
    // TODO
    // Still have to deal with newlines in the input text
    vector<string> currentWords;
    vector<string> currentFragments;

    if (words.size() > 0) {
        int runningChars = 0;
        currentWords.clear();
        currentFragments.clear();
        const char* const delim = " ";
        ostringstream imploded;

        int maxChars = 600; 
        for(unsigned int i = 0; i < words.size(); i++) {
            runningChars += words.at(i).size() + 1;

            if (runningChars <= maxChars) {
                currentWords.push_back(words.at(i));
            } else {
                currentWords.push_back("\u2192");
                imploded.clear();
                imploded.str("");
                copy(currentWords.begin(), currentWords.end(), ostream_iterator<string>(imploded, delim));
                currentFragments.push_back(imploded.str());

                currentWords.clear();
                currentWords.push_back(words.at(i));

                runningChars = 0;
            }
            
        }

        // last line
        imploded.clear();
        imploded.str("");
        copy(currentWords.begin(), currentWords.end(), ostream_iterator<string>(imploded, delim));
        currentFragments.push_back(imploded.str());
    }

    for (unsigned int i = 0; i < currentFragments.size(); i++) {
        dbText.fontSize = standardFontSize;

        // Annoying for string processing, but this is how it works...
        ostringstream oStringStream;
        oStringStream << DateTimeFormatter::format(code.code_dt.timestamp(), "%W, %e %B %Y") << "\n \n" << "Contributed by " << code.pseudonym << "\n \n" << currentFragments.at(i);
        completeText = oStringStream.str();
    
        dbText.xPos = 80;
        dbText.yPos = 80;

        font.setSize(dbText.fontSize);
        dbText.text = completeText;
        addToSequence(dbText, sequence);
    }

    codeFragmentsAdded = currentFragments.size();
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

        font.setSize(textLine.fontSize);
        ofRectangle rect = font.getStringBoundingBox(textLine.text, 0, 0);
            
        float width = rect.width;
        float height = rect.height;
    
        textLine.xPos = (ofGetWindowWidth()/2) - (width/2);
        textLine.yPos = (ofGetWindowHeight()/2) + (height/2);


        textLine.fColor = ofColor(textLines.getValue("fRed", 255), textLines.getValue("fGreen", 0), textLines.getValue("fBlue", 0), 255);
        textLine.bColor = ofColor(textLines.getValue("bRed", 255), textLines.getValue("bGreen", 255), textLines.getValue("bBlue", 255), 255);

        textLine.startTime = 0;
        textLine.duration = textLines.getValue("duration", 3.5) * 1000;
        textLine.delta = textLines.getValue("delta", 0.35) * 1000;
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
    // Pop the random code fragments added
    for (unsigned int i = 0; i < codeFragmentsAdded; i++) {
        sequence.pop_back();
    }
    
    // Reset code fragments for corner case of nothing enabled.
    codeFragmentsAdded = 0;

    // Add new random code, update last time
    updateRandomCode();

    // Ensure that the font size is correct for the first line
    font.setSize(sequence.at(0).fontSize);

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

    if (syllogeDebug) {
        ofSetColor(0, 0, 0, 255);
        elapsedTimeString = "Elapsed time: " + ofToString(ofGetElapsedTimef());
        ofDrawBitmapString(elapsedTimeString, 10, 10);
        fpsString = "frame rate: "+ ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapString(fpsString, 10, 30);
        loopCounterString = "Loop count: "+ ofToString(loopCounter);
        ofDrawBitmapString(loopCounterString, 10, 50);
    }


}

//--------------------------------------------------------------
void sylloge_of_codes::keyPressed (int key){
    if ((key == 'd') || (key == 'D')) {
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
