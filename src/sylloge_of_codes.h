#pragma once
#include <ctime>
#include <cmath>
#include <libintl.h>
#include <locale.h>
#include <sqlite3.h>

#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>
#include <Poco/LocalDateTime.h>
#include <Poco/Timespan.h>
#include <Poco/Timezone.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>
#include <Poco/StringTokenizer.h>

#include "ofMain.h"
#include "ofxSQLiteHeaders.h"
#include "ofxXmlSettings.h"

#ifdef FTGLES
#include "ofxFTGLES.h"
#else
#include "ofxFTGL.h"
#endif

#define SYLLOGE_DEBUG 1

using Poco::Timestamp;
using Poco::DateTime;
using Poco::LocalDateTime;
using Poco::Timespan;
using Poco::Timezone;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::DateTimeParser;
using Poco::StringTokenizer;
using std::vector;

struct Sylloge {
    int id;
    string code;
    string comments;
    string pseudonym;
    DateTime code_dt;
    bool enabled;
};

struct TextLine {
    string text;
    float startTime; // in seconds
    float duration; // in seconds
    float delta; // in seconds
    bool fade;
    float fadeDuration; // in seconds
    float currentAlpha;
    ofColor fColor;
    ofColor bColor;
    string font;
    float fontSize;
    float xPos;
    float yPos;
};

struct Segment {
    float startTime;    // in seconds
    float delta;        // in seconds;
    float duration;     // in seconds
    float fadeDuration; // in seconds
    bool fade;
    ofColor backgroundColor;
    int xPos;
    int yPos;
    TextLine textLine;
};

class sylloge_of_codes : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased (int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void setSyllogeCount();

        void setCompleteText(char* text);
		
		ofTrueTypeFont testFont;
		ofTrueTypeFont testFont2;
		ofTTFCharacter testChar;
		char letter;

        // Text offset in pixels
        static const int offset = 15;

        unsigned int loopCounter;

        bool syllogeDebug;
        bool syllogeSounds;

        // Time zone differential
        int tzd;

        // sqlite
        ofxSQLite* sqlite;

        // Current number of entries in the database
        int syllogeCount;

        // Current sylloge item
        Sylloge currentCode;
        int currentSequenceIndex;
        float alpha;
        float centerX(float stringWidth);
        float centerY(float stringHeight);
        void addToSequence(TextLine& textLine, vector<TextLine>& sequence);
        void segmentFadeIn(vector<TextLine>& sequence, int index);
        void resetSequence(vector<TextLine>& sequence);
        void selectRandomCode(Sylloge& currentCode);
        void addCodeToSequence(Sylloge& code);
        void loadTextLines(vector<TextLine>& sequence);
        void setLastTime(vector<TextLine>& sequence);
    private:
#ifdef FTGLES
        //ofxFTGLESFont font;
        ofxFTGLESSimpleLayout font;
#else
        //ofxFTGLFont font;
        ofxFTGLSimpleLayout font;
#endif
        // Settings
        ofxXmlSettings settings;
        ofxXmlSettings textLines;
        static const string settingsFilename;
        static const string textLinesFilename;

        string elapsedTimeString;
        string fpsString;
        string loopCounterString;
        string playSoundsString;
        vector<TextLine> sequence;
        vector<string> textFragments;
        bool skipIntro;
        TextLine currentTextLine;
        unsigned long long frameRate;
        bool drawNow;
        unsigned long long lastTime;
        unsigned int codeFragmentsAdded;

};

// CRUFT BELOW:

