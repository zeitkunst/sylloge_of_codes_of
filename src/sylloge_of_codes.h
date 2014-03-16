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
    ofxTextBlock textBlock;    
    int xPos;
    int yPos;
};

struct CodeDuration {
    string code;
    float duration;
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

        sqlite3 *db;
        char *zErrMsg;
        int rc;
        char *code1;
        char *code2;

        ofxSQLite* sqlite;

        // Current number of entries in the database
        int syllogeCount;

        // Current sylloge item
        Sylloge currentCode;
        float alpha;
        float centerX(ofxTextBlock textBlock);
        float centerY(ofxTextBlock textBlock);
        void addToSequence(Segment& segment, vector<Segment>& sequence);
    private:
        string elapsedTimeString;
        vector<CodeDuration> codes;
        vector<Segment> sequence;

};

// CRUFT BELOW:

