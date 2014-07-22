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

//    ofRectangle rect = font.getStringBoundingBox(currentTextLine.text, 0, 0);
//    float width = rect.width;
//    float height = rect.height;
//    ofLog(OF_LOG_NOTICE, ofToString(width));
//    ofLog(OF_LOG_NOTICE, ofToString(height));

    //sequence.at(currentSequenceIndex).xPos = (ofGetWindowWidth()/2) - (width/2);
    //sequence.at(currentSequenceIndex).yPos = (ofGetWindowHeight()/2) + (height/2);


    
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
    float standardFontSize = 30.0;
    TextLine dbText;
    dbText.font = "SourceSansPro-Black.otf";
    dbText.fontSize = standardFontSize;
    dbText.fColor = ofColor(255.0, 0.0, 0.0);
    dbText.bColor = ofColor(255.0, 255.0, 255.0);
    dbText.startTime = 0;
    dbText.delta = 1.0 * 1000;
    dbText.duration = 1.0 * 1000;
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
    //replace(code.code.begin(), code.code.end(), '\n', "¶");
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
    vector<string> currentFragments;

    if (words.size() > 0) {
        int runningChars = 0;
        currentWords.clear();
        currentFragments.clear();
        const char* const delim = " ";
        ostringstream imploded;

        int maxChars = 600; 
        for(int i = 0; i < words.size(); i++) {
            runningChars += words.at(i).size() + 1;

            if (runningChars <= maxChars) {
                currentWords.push_back(words.at(i));
            } else {
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

    for (int i = 0; i < currentFragments.size(); i++) {
        dbText.fontSize = standardFontSize;
        //ofLog(OF_LOG_NOTICE, currentFragments.at(i));

        // Annoying for string processing, but this is how it works...
        ostringstream oStringStream;
        oStringStream << DateTimeFormatter::format(code.code_dt.timestamp(), "%W, %e %B %Y") << "\n \n" << "Contributed by " << code.pseudonym << "\n \n" << currentFragments.at(i);
        completeText = oStringStream.str();
    
        dbText.xPos = 80;
        dbText.yPos = 80;

        font.setSize(dbText.fontSize);
        ofRectangle rect = font.getStringBoundingBox(dbText.text, dbText.xPos, dbText.yPos);
        rect.standardize();
        float rectHeight = abs(rect.y) + abs(rect.height);

        // FUDGE FACTOR BELOW!
        /*
        if ((rectHeight > (ofGetWindowHeight() - 2 * dbText.yPos)) || (rectHeight == dbText.yPos)) {
            ofLog(OF_LOG_NOTICE, "TOO HIGH");
            // If too high, scale font size a bit
            dbText.fontSize = 0.9 * dbText.fontSize;
        }
        */
        ofLog(OF_LOG_NOTICE, ofToString(rect));
        ofLog(OF_LOG_NOTICE, ofToString(rectHeight));

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
        textLine.xPos = centerX(font.stringWidth(textLine.text));
        textLine.yPos = centerY(font.stringHeight(textLine.text));

        font.setSize(textLine.fontSize);
        ofRectangle rect = font.getStringBoundingBox(textLine.text, 0, 0);
            
        float width = rect.width;
        float height = rect.height;
        ofLog(OF_LOG_NOTICE, ofToString(width));
        ofLog(OF_LOG_NOTICE, ofToString(height));
    
        textLine.xPos = (ofGetWindowWidth()/2) - (width/2);
        textLine.yPos = (ofGetWindowHeight()/2) + (height/2);


        textLine.fColor = ofColor(textLines.getValue("fRed", 255), textLines.getValue("fGreen", 0), textLines.getValue("fBlue", 0), 255);
        textLine.bColor = ofColor(textLines.getValue("bRed", 255), textLines.getValue("bGreen", 255), textLines.getValue("bBlue", 255), 255);

        textLine.startTime = 0;
        textLine.duration = textLines.getValue("duration", 2.0) * 1000;
        textLine.delta = textLines.getValue("delta", 0.25) * 1000;
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
    ofLog(OF_LOG_NOTICE, "Sequence size before pop: " + ofToString(sequence.size()));
    ofLog(OF_LOG_NOTICE, "codeFragmentsAdded: " + ofToString(codeFragmentsAdded));

    // Pop the random code fragments added
    for (int i = 0; i < codeFragmentsAdded; i++) {
        sequence.pop_back();
    }

    ofLog(OF_LOG_NOTICE, "Sequence size after erase: " + ofToString(sequence.size()));

    // Add new random code, update last time
    selectRandomCode(currentCode);
    addCodeToSequence(currentCode);
    setLastTime(sequence);

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


