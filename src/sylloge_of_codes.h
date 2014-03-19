#pragma once
#include <libintl.h>
#include <sqlite3.h>

#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>
#include <Poco/LocalDateTime.h>
#include <Poco/Timespan.h>
#include <Poco/Timezone.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

#include "ofMain.h"
#include "ofxTextSuite.h"
#include "ofxSQLiteHeaders.h"
#include "ofxXmlSettings.h"

#define SYLLOGE_DEBUG 1

using Poco::Timestamp;
using Poco::DateTime;
using Poco::LocalDateTime;
using Poco::Timespan;
using Poco::Timezone;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::DateTimeParser;
using std::vector;

struct Sylloge {
    int id;
    string code;
    string comments;
    string pseudonym;
    DateTime code_dt;
    bool enabled;
};

struct Segment {
    float startTime;    // in seconds
    float delta;        // in seconds;
    float duration;     // in seconds
    float fadeDuration; // in seconds
    bool fade;
    float currentAlpha;
    ofxTextBlock textBlock;    
    int xPos;
    int yPos;
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
		
        static int basicCallback(void *data, int argc, char **argv, char **azColName);
		ofTrueTypeFont testFont;
		ofTrueTypeFont testFont2;
		ofTTFCharacter testChar;
		char letter;
        ofxTextBlock myText;
        ofxTextBlock i18nText;

        ofxTextBlock leftTextBlock;
        ofxTextBlock rightTextBlock;

        // Strings for overall text and left and right text
        std::string completeText;
        std::string leftText;
        std::string rightText;

        // Text offset in pixels
        static const int offset = 15;

        unsigned int loopCounter;

        bool syllogeDebug;

        // Time zone differential
        int tzd;


        ofxSQLite* sqlite;

        // Current number of entries in the database
        int syllogeCount;

        // Current sylloge item
        Sylloge currentCode;
        float alpha;
        float centerX(ofxTextBlock textBlock);
        float centerY(ofxTextBlock textBlock);
        void addToSequence(Segment& segment, vector<Segment>& sequence);
        void segmentFadeIn(vector<Segment>& sequence, int index);
        void resetSequence(vector<Segment>& sequence);
        void selectRandomCode(Sylloge& currentCode);
    private:
        // Settings
        ofxXmlSettings settings;
        const string settingsFilename = "sylloge_of_codes_of_settings.xml";

        string elapsedTimeString;
        string fpsString;
        string loopCounterString;
        vector<Segment> sequence;

};

// CRUFT BELOW:

