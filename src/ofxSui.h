#pragma once

#ifndef __ofxSui__
#define __ofxSui__

#include <algorithm>
#include <string>
#include <iostream>
#include "ofxEasing.h"
#include "ofMain.h"
#include <cmath>
#include <boost/range/adaptor/reversed.hpp>

using namespace std;

namespace SUI {
    
    struct Settings {
        bool liveReload = false;
    };
    
    extern Settings settings;
    
    void liveReload(bool reload = true);
    extern map<string, ofImage*> images;
    extern map<string, ofSoundPlayer*> sounds;
    
    //========================================================================================================
    //========================================================================================================
    
    static string cleanSelector(string selector){
        selector = ofJoinString(ofSplitString(selector,"("), "");
        selector = ofJoinString(ofSplitString(selector,")"), "");
        selector = ofJoinString(ofSplitString(selector,"["), "");
        selector = ofJoinString(ofSplitString(selector,"]"), "");
        return selector;
    };
    
    static string cleanArrayBrackets(string str){
        str = ofJoinString(ofSplitString(str, "["), "" );
        str = ofJoinString(ofSplitString(str, "]"), "" );
        return str;
    };
    
    static string cleanSemiColon(string str){
        str = ofJoinString(ofSplitString(str, ";"), "" );
        return str;
    };
    
    static void setImage(string filepath){
        //ofLog() << "SUI::setImage(" << id << "," << filepath << ") ---- ";
        if ( SUI::images.count(filepath) == 0 ) SUI::images[filepath] = new ofImage();
        SUI::images[filepath]->load(ofToDataPath(filepath));
    };
    
    static void setSound(string filepath){
        if ( SUI::sounds.count(filepath) == 0 ) SUI::sounds[filepath] = new ofSoundPlayer();
        SUI::sounds[filepath]->load(ofToDataPath(filepath));
    };
    
    static ofSoundPlayer* getSound(string id){
        return SUI::sounds[id];
    }
    
    static bool hasSound(string id){
        if (SUI::sounds[id] == NULL ) return false;
        return true;
    }
    
    static void playSound(string id){
        if ( SUI::sounds[id] != NULL ) SUI::sounds[id]->play();
    }
    
    static ofImage* getImage(string id){
        return SUI::images[id];
    }
    
    enum AnchorPoint {
        ANCHOR_LEFT_TOP,
        ANCHOR_LEFT_CENTER,
        ANCHOR_LEFT_BOTTOM,
        ANCHOR_RIGHT_TOP,
        ANCHOR_RIGHT_CENTER,
        ANCHOR_RIGHT_BOTTOM,
        ANCHOR_CENTER_TOP,
        ANCHOR_CENTER_CENTER,
        ANCHOR_CENTER_BOTTOM
    };
    
    enum CanvasEvent {
        CANVAS_EVENT_TRIGGER,
        CANVAS_EVENT_LOADED
    };
    
    enum Event {
        SUI_EVENT_MOUSE_OVER,
        SUI_EVENT_MOUSE_OUT,
        SUI_EVENT_MOUSE_ENTER,
        SUI_EVENT_MOUSE_EXIT,
        SUI_EVENT_MOUSE_PRESSED,
        SUI_EVENT_MOUSE_MOVED,
        SUI_EVENT_MOUSE_DRAGGED,
        SUI_EVENT_MOUSE_RELEASED,
        SUI_EVENT_MOUSE_CLICK,
        SUI_EVENT_MOUSE_DOUBLE_CLICK,
        
        SUI_EVENT_TOUCH_DOWN,
        SUI_EVENT_TOUCH_UP,
        SUI_EVENT_TOUCH_TAP,
        SUI_EVENT_TOUCH_DOUBLE_TAP,
        
        SUI_EVENT_STATE_CHANGE,
        SUI_EVENT_TEXT_CHANGE,
        SUI_EVENT_VALUE_CHANGE,
        SUI_EVENT_SLIDER_VALUE_CHANGE,
        SUI_EVENT_TOGGLE_CHANGE,
        SUI_EVENT_SUBMIT,
        
        SUI_EVENT_FOCUS,
        SUI_EVENT_UNFOCUS,
        
        SUI_EVENT_ANIMATE_COMPLETE,
        SUI_EVENT_ANIMATE_STEP,
        SUI_EVENT_ANIMATE_START
        
#ifdef USING_ofxTouchPadScroll
        ,SUI_EVENT_TOUCHPAD_SCROLL_START
        ,SUI_EVENT_TOUCHPAD_SCROLL
        ,SUI_EVENT_TOUCHPAD_SCROLL_END
        ,SUI_EVENT_TOUCHPAD_SCROLL_INERTIA
#endif
    };
    
    static string cleanString(string s, bool removeSpaces = false, bool removeTrailingComma = true){
        if ( removeSpaces ) s = ofJoinString(ofSplitString(s," "), "");
        if ( removeTrailingComma ) {
            if ( s.find_last_of(";") == s.length()-1 ) s = s.substr(0,s.length()-1);
        }
        s = ofJoinString(ofSplitString(s,"\n"), "");
        s = ofJoinString(ofSplitString(s,"\t"), "");
        s = ofJoinString(ofSplitString(s,"\""), "");
        return s;
    }
    
    class CustomParams {
    public:
        ~CustomParams(){};
        CustomParams(){};
        
        map<string,bool> bools;
        map<string,string> strings;
        map<string,int> ints;
        map<string,float> floats;
    };
    
    static vector<string> getStyles(vector<string> lines){
        vector <string> styleLines;
        
        string selector;
        bool blockOpen = false;
        bool commentOpen = false;
        
        int blockOpenCount = 0;
        int commentOpenCount = 0;
        
        for (auto& line : lines){
            
            string l = ofJoinString(ofSplitString(line," "), "");
            l = ofJoinString(ofSplitString(l,"\n"), "");
            l = ofJoinString(ofSplitString(l,"\t"), "");
            
            // skip comments
            if ( line.find("//") == 0 ) continue;
            if ( line.find("/*") != string::npos && line.find("*/") != string::npos ) continue;
            if ( !commentOpen ) {
                if ( line.find("/*") == 0 ) {
                    commentOpen = true;
                    commentOpenCount = 1;
                    continue;
                }
            } else {
                if ( line.find("*/") != string::npos ){
                    commentOpenCount--;
                    
                    if ( commentOpenCount == 0 ){
                        commentOpen = false;
                    }
                    
                    continue;
                } else if ( line.find("/*") != string::npos ){
                    commentOpenCount++;
                    continue;
                }
            }
            
            //
            if ( !blockOpen ){
                if ( l.find("{") != string::npos && l.find("}") == string::npos ){
                    selector = l.substr(0, l.find("{"));
                    selector = ofJoinString(ofSplitString(selector,"("), "");
                    blockOpen = true;
                    blockOpenCount = 1;
                } else {
                    styleLines.push_back(l);
                }
            } else {
                if ( l.find("{") != string::npos && l.find("}") != string::npos ){
                } else if ( l.find("{") != string::npos ){
                    blockOpenCount++;
                } else if ( l.find("}") != string::npos ){
                    blockOpenCount--;
                    if ( blockOpenCount == 0 ){
                        blockOpen = false;
                    } else {
                        //strLines.push_back(l);
                    }
                } else {
                    //strLines.push_back(l);
                }
            }
        }
        
        return styleLines;
    };
    
    
    static map<string, vector<string>> getBlocks(vector<string> lines){
        vector<string> keys;
        map<string, vector<string>> blocks;
        vector <string> strLines;
        
        string selector;
        bool blockOpen = false;
        bool commentOpen = false;
        
        int blockOpenCount = 0;
        int commentOpenCount = 0;
        
        for (auto& line : lines){
            
            string l = ofJoinString(ofSplitString(line," "), "");
            l = ofJoinString(ofSplitString(l,"\n"), "");
            l = ofJoinString(ofSplitString(l,"\t"), "");
            
            //string l = line;
            
            //ofLog() << "––– " << l;
            
            // skip comments
            if ( line.find("//") == 0 ) continue;
            if ( !commentOpen ) {
                if ( line.find("/*") == 0 ) {
                    commentOpen = true;
                    commentOpenCount = 1;
                    continue;
                }
            } else {
                if ( line.find("*/") != string::npos ){
                    commentOpenCount--;
                    
                    if ( commentOpenCount == 0 ){
                        commentOpen = false;
                    }
                    
                    continue;
                } else if ( line.find("/*") != string::npos ){
                    commentOpenCount++;
                    continue;
                }
            }
            
            //
            
            if ( !blockOpen ){
                if ( l.find("{") != string::npos && l.find("}") == string::npos ){
                    selector = l.substr(0, l.find("{"));
                    blockOpen = true;
                    blockOpenCount = 1;
                    strLines = vector<string>();
                    //ofLog() << "+++ BLOCK OPEN +++";
                }
            } else {
                
                
                if ( l.find("{") != string::npos && l.find("}") != string::npos ){
                    strLines.push_back(l);
                } else if ( l.find("{") != string::npos ){
                    strLines.push_back(l);
                    //ofLog() << blockOpenCount << "++";
                    blockOpenCount++;
                } else if ( l.find("}") != string::npos ){
                    //ofLog() << blockOpenCount << "--";
                    blockOpenCount--;
                    if ( blockOpenCount == 0 ){
                        //ofLog() << "--- BLOCK CLOSED ---";
                        
                        blocks[selector] = vector<string>(strLines);
                        //ofLog() << "[Block] " << selector << "    lines:" << blocks[selector].size();
                        //ofLog() << ofJoinString(blocks[selector], "\n");
                        blockOpen = false;
                        
                        keys.push_back(selector);
                        
                        blocks["[[-keys-]]"] = keys;
                        
                    } else {
                        strLines.push_back(l);
                    }
                } else {
                    strLines.push_back(l);
                }
            }
        }
        
        return blocks;
    };
    
    static map<string, map<string,string>> getActionsStr(vector<string> lines){
        //ofLog() << "getActionsStr(): " << ofJoinString(lines, " ");
        vector<string> keys;
        map<string, map<string,string>> actions;
        map<string, vector<string>> blocks = getBlocks(lines);
        vector <string> strLines;
        
        
        string selector;
        bool blockOpen = false;
        bool commentOpen = false;
        
        int blockOpenCount = 0;
        int commentOpenCount = 0;
        
        for (auto it=blocks.begin(); it!=blocks.end(); ++it){
            map<string, string> tempActions;
            string name = it->first;
            //ofLog() << "block START: " << name;
            if ( name == "[[-keys-]]" ) continue;
            for (auto& line : it->second){
                
                string l = ofJoinString(ofSplitString(line," "), "");
                l = ofJoinString(ofSplitString(l,"\n"), "");
                l = ofJoinString(ofSplitString(l,"\t"), "");
                
                //string l = line;
                
                //ofLog() << "––– " << l;
                
                // skip comments
                if ( line.find("//") == 0 ) continue;
                if ( !commentOpen ) {
                    if ( line.find("/*") == 0 ) {
                        commentOpen = true;
                        commentOpenCount = 1;
                        continue;
                    }
                } else {
                    if ( line.find("*/") != string::npos ){
                        commentOpenCount--;
                        
                        if ( commentOpenCount == 0 ){
                            commentOpen = false;
                        }
                        
                        continue;
                    } else if ( line.find("/*") != string::npos ){
                        commentOpenCount++;
                        continue;
                    }
                }
                
                //
                
                if ( !blockOpen ){
                    if ( l.find("[") != string::npos && l.find("]") == string::npos ){
                        selector = l.substr(0, l.find("[")-1 );
                        blockOpen = true;
                        blockOpenCount = 1;
                        strLines = vector<string>();
                        //ofLog() << "+++ BLOCK OPEN +++";
                    } else if ( (l.find("[") != string::npos && l.find("]") != string::npos) || (l.find("{") != string::npos && l.find("}") != string::npos) ) {
                        
                        selector = l.substr(0, l.find(":"));
                        tempActions[selector] = l.substr(l.find(":")+1);
                        //ofLog() << "[Action] " << selector << "   " << tempActions[selector];
                    }
                } else {
                    
                    
                    if ( l.find("[") != string::npos && l.find("]") != string::npos ){
                        strLines.push_back(l);
                        //actions[selector] = l;
                        //ofLog() << "[Action] " << selector << "   " << actions[selector];
                    } else if ( l.find("[") != string::npos ){
                        strLines.push_back(l);
                        //ofLog() << blockOpenCount << "++";
                        blockOpenCount++;
                    } else if ( l.find("]") != string::npos ){
                        //ofLog() << blockOpenCount << "--";
                        blockOpenCount--;
                        if ( blockOpenCount == 0 ){
                            //ofLog() << "--- BLOCK CLOSED ---";
                            
                            //blocks[selector] = vector<string>(strLines);
                            blockOpen = false;
                            
                            //keys.push_back(selector);
                            tempActions[selector] = ofJoinString(strLines, " ");
                            //ofLog() << "[Action] " << selector << "   " << tempActions[selector];
                            //blocks["[[-keys-]]"] = keys;
                            
                        } else {
                            strLines.push_back(l);
                        }
                    } else {
                        strLines.push_back(l);
                    }
                }
            }
            actions[it->first] = tempActions;
            //ofLog() << "[Copiled] " << name;
        }
        
        //ofLog() << "getActionsStr() DONE =================";
        return actions;
    };
    
    
    static map<string, vector<string>> getBlocks(ofBuffer buffer){
        vector<string> lines;
        for (auto& line : buffer.getLines()){
            lines.push_back(line);
        }
        return getBlocks(lines);
    }
    
    static void parseCustomParams(SUI::CustomParams& params, map<string, vector<string>> blocks){
        vector<string> lines = getStyles( blocks["-params"] );
        
        for (auto& line : lines ){
            //ofLog() << line;
            vector<string> keyValue = vector<string>(2);
            line = cleanString(line);
            
            int index = line.find_first_of(":");
            keyValue[0] = line.substr(0,index);
            keyValue[1] = line.substr(index+1);
            
            if ( keyValue[0].find("(string)") != string::npos ){
                
                string id = keyValue[0].substr(keyValue[0].find("(string)")+8);
                string value = cleanString(keyValue[1]);;
                
                //ofLog() << "[string:" << id << "] " << value;
                
                params.strings[id] = value;
            } else if ( keyValue[0].find("(bool)") != string::npos ){
                params.bools[ keyValue[0].substr(keyValue[0].find("(bool)")+6) ] = ofToBool(keyValue[1]);
            } else if ( keyValue[0].find("(int)") != string::npos ){
                params.ints[ keyValue[0].substr(keyValue[0].find("(int)")+5) ] = ofToInt(keyValue[1]);
            } else if ( keyValue[0].find("(float)") != string::npos ){
                params.floats[ keyValue[0].substr(keyValue[0].find("(float)")+7) ] = ofToFloat(keyValue[1]);
            }
            
            //ofLog() << keyValue[0] << " – " << keyValue[1];
            
        }
    }
    
    static void parseCustomParams(SUI::CustomParams& params, vector<string> lines){
        
        for (auto& line : lines ){
            //ofLog() << line;
            vector<string> keyValue = vector<string>(2);
            line = cleanString(line);
            
            int index = line.find_first_of(":");
            keyValue[0] = line.substr(0,index);
            keyValue[1] = line.substr(index+1);
            
            if ( keyValue[0].find("(string)") != string::npos ){
                params.strings[ keyValue[0].substr(keyValue[0].find("(string)")+8) ] = keyValue[1];
            } else if ( keyValue[0].find("(bool)") != string::npos ){
                params.bools[ keyValue[0].substr(keyValue[0].find("(bool)")+6) ] = ofToBool(keyValue[1]);
            } else if ( keyValue[0].find("(int)") != string::npos ){
                params.ints[ keyValue[0].substr(keyValue[0].find("(int)")+5) ] = ofToInt(keyValue[1]);
            } else if ( keyValue[0].find("(float)") != string::npos ){
                params.floats[ keyValue[0].substr(keyValue[0].find("(float)")+7) ] = ofToFloat(keyValue[1]);
            }
            
            //ofLog() << keyValue[0] << " – " << keyValue[1];
            
        }
    }
    
    
    
    //========================================================================================================
    //========================================================================================================
    
    class AnimatableParams {
    public:
        ~AnimatableParams(){};
        AnimatableParams(){};
        
        float x = nanf("");
        float y = nanf("");
        float width = nanf("");
        float height = nanf("");
        float scale = nanf("");
        float rotation = nanf("");
        float opacity = nanf("");
        float backgroundSizeX = nanf("");
        float backgroundSizeY = nanf("");
        
        void resetAnimatableParams(){
            x = nanf("");
            y = nanf("");
            width = nanf("");
            height = nanf("");
            scale = nanf("");
            rotation = nanf("");
            opacity = nanf("");
            backgroundSizeX = nanf("");
            backgroundSizeY = nanf("");
        }
    };
    
    class Element;
    
    class Action {
    public:
        ~Action(){};
        Action(){};
        
        string id;
        map<string,string> subactions;
        
        void parse(map<string,string> subactions){
            /*actions = vector<string>();
            for (auto it=subactions.begin(); it!=subactions.end(); ++it){
                //ofLog() << "subaction:" << it->first << ">" << it->second;
                actions.push_back(it->second);
            }*/
            
            this->subactions = subactions;
            
            for (auto it=subactions.begin(); it!=subactions.end(); ++it){
                //ofLog() << "[subaction] " << it->first << " > " << it->second;
            }
        }
        
        void run(Element& el);
        
    private:
        void _parse(string action){
            
        }
        
    };
    
    class Actions {
    public:
        ~Actions(){};
        Actions(){};
        
        map<string, vector<string>> blocks;
        map<string, Action> items;
        
        Action& get(string id){
            //ofLog() << "Action.get(): " << id << "  size:" << items.size();
            for (auto it=items.begin(); it!=items.end(); ++it){
                ofLog() << "• " << it->first;
            }
            return items[id];
        }
        
    };
    
    class Style : public AnimatableParams {
    public:
        ~Style(){};
        Style(){};
        
        
        
        void copy(Style& copyStyle){
            backgroundColor = copyStyle.backgroundColor;
            backgroundImage = copyStyle.backgroundImage;
            overflow = copyStyle.overflow;
            width = copyStyle.width;
            height = copyStyle.height;
            backgroundSizeX = copyStyle.backgroundSizeX;
            backgroundSizeY = copyStyle.backgroundSizeY;
            opacity = copyStyle.opacity;
            rotation = copyStyle.rotation;
            x = copyStyle.x;
            y = copyStyle.x;
            anchorPoint = copyStyle.anchorPoint;
            hasBackgroundColor = copyStyle.hasBackgroundColor;
        }
        
        void reset(bool resetPosition = true){
            if ( resetPosition ) {
                x = nanf("");
                y = nanf("");
            }
            width = nanf("");
            height = nanf("");
            scale = nanf("");
            rotation = nanf("");
            opacity = nanf("");
            backgroundSizeX = nanf("");
            backgroundSizeY = nanf("");
            anchorPoint = ANCHOR_LEFT_TOP;
            overflow = "auto";
            backgroundColor = ofColor::white;
            backgroundImage = "";
            hasBackgroundColor = false;
        }
        
        void resetBase(){
            resetAnimatableParams();
            /*
             
             x = nanf("");
             y = nanf("");
             width = nanf("");
             height = nanf("");
             scale = nanf("");
             rotation = nanf("");
             opacity = nanf("");
             backgroundSizeX = nanf("");
             backgroundSizeY = nanf("");
             
             */
            
            width = 0;
            height = 0;
            x = 0;
            y = 0;
            opacity = 1.0;
            rotation = 0.0;
            anchorPoint = ANCHOR_LEFT_TOP;
            overflow = "auto";
            backgroundColor = ofColor::white;
            backgroundImage = "";
            //hasBackgroundColor = true;
            
        }
        
        void debugLog(){
            ofLog() << " ";
            ofLog() << "<<-------------";
            ofLog() << "background-image: " << backgroundImage;
            ofLog() << "background-color: " << ofToString(backgroundColor.getHex());
            ofLog() << "background-size: " << backgroundSizeX << " " << backgroundSizeY;
            ofLog() << "width: " << width;
            ofLog() << "height: " << height;
            ofLog() << "overflow: " << overflow;
            ofLog() << "------------->>";
        }
        
        
        int anchorPoint = nan(0);
        
        void setBackgroundColor(string hex){
            string col = ofJoinString(ofSplitString(hex,"#"), "");
            float floatColor = stoul(col, nullptr, 16);
            backgroundColor.setHex(floatColor);
            hasBackgroundColor = true;
        }
        
        void removeBackgroundColor(){
            hasBackgroundColor = false;
        }
        
        void mergeStyles(Style& style, bool force = false ){
            if ( !force ) {
                if ( isnan(x) ) x = style.x;
                if ( isnan(y) ) y = style.y;
                if ( isnan(rotation) ) rotation = style.rotation;
                if ( isnan(opacity) ) opacity = style.opacity;
                if ( isnan(width) ) width = style.width;
                if ( isnan(height) ) height = style.height;
                if ( isnan(backgroundSizeX) ) backgroundSizeX = style.backgroundSizeX;
                if ( isnan(backgroundSizeY) ) backgroundSizeY = style.backgroundSizeY;
                if ( backgroundImage == "" && style.backgroundImage != "" ) {
                    backgroundImage = style.backgroundImage;
                    setImage(backgroundImage);
                }
                if ( overflow == "" ) overflow = style.overflow;
                if ( isnan(anchorPoint) ) anchorPoint = style.anchorPoint;
                if ( !hasBackgroundColor && style.hasBackgroundColor ) {
                    backgroundColor = style.backgroundColor;
                    hasBackgroundColor = true;
                }
            } else {
                if ( !isnan(style.x) ) x = style.x;
                if ( !isnan(style.y) ) y = style.y;
                if ( !isnan(style.rotation) ) rotation = style.rotation;
                if ( !isnan(style.opacity) ) opacity = style.opacity;
                if ( !isnan(style.width) ) width = style.width;
                if ( !isnan(style.height) ) height = style.height;
                if ( !isnan(style.backgroundSizeX) ) backgroundSizeX = style.backgroundSizeX;
                if ( !isnan(style.backgroundSizeY) ) backgroundSizeY = style.backgroundSizeY;
                if ( style.backgroundImage != "" ) {
                    backgroundImage = style.backgroundImage;
                    setImage(backgroundImage);
                }
                if ( style.overflow != "" ) overflow = style.overflow;
                if ( !isnan(style.anchorPoint) ) anchorPoint = style.anchorPoint;
                if ( style.hasBackgroundColor ) {
                    backgroundColor = style.backgroundColor;
                    hasBackgroundColor = true;
                }
            }
            
        };
        
        
        bool hasBackgroundColor = false;
        ofColor backgroundColor = ofColor::white;
        string backgroundImage = "";
        string overflow = "";
        vector<string> rawStyles;
        vector<string> styles;
        //map<string, vector<string>> blocks;
        
    };
    
    static void parseStyle(Style& style, vector<string> lines){
        for ( auto& line : lines ){
            //ofLog() << "s << " << s;
            if ( line.find("{") == string::npos && line.find("}") == string::npos && line != "" ){
                vector<string> keyValue = ofSplitString(line, ":");
                keyValue[1] = ofJoinString(ofSplitString(keyValue[1],";"), "");
                //ofLog() << keyValue[0] << ":" << keyValue[1];
                //return;
                keyValue[0] = ofToLower(keyValue[0]);
                
                if ( keyValue[0] == "background-color" ){
                    if ( keyValue[1].find("#") != string::npos ){
                        //string col = ofJoinString(ofSplitString(keyValue[1],"#"), "");
                        //float floatColor = stoul(col, nullptr, 16);
                        style.setBackgroundColor(keyValue[1]);
                    }
                } else if ( keyValue[0] == "anchorpoint" || keyValue[0] == "anchor-point" ){
                    //baseStyle.width = ofToInt(keyValue[1]);
                    string val = cleanString(ofToLower(keyValue[1]), true);
                    
                    /*ANCHOR_LEFT_TOP,
                     ANCHOR_LEFT_CENTER,
                     ANCHOR_LEFT_BOTTOM,
                     ANCHOR_RIGHT_TOP,
                     ANCHOR_RIGHT_CENTER,
                     ANCHOR_RIGHT_BOTTOM,
                     ANCHOR_CENTER_TOP,
                     ANCHOR_CENTER_CENTER,
                     ANCHOR_CENTER_BOTTOM*/
                    
                    if ( val == "left-top" || val == "left_top" ){
                        style.anchorPoint = ANCHOR_LEFT_TOP;
                    } else if ( val == "left-center" || val == "left_center" ){
                        style.anchorPoint = ANCHOR_LEFT_CENTER;
                    } else if ( val == "left-bottom" || val == "left_bottom" ){
                        style.anchorPoint = ANCHOR_LEFT_BOTTOM;
                    } else if ( val == "right-top" || val == "right_top" ){
                        style.anchorPoint = ANCHOR_RIGHT_TOP;
                    } else if ( val == "right-center" || val == "right_center" ){
                        style.anchorPoint = ANCHOR_RIGHT_CENTER;
                    } else if ( val == "right-bottom" || val == "right_bottom" ){
                        style.anchorPoint = ANCHOR_RIGHT_BOTTOM;
                    } else if ( val == "center-top" || val == "center_top" ){
                        style.anchorPoint = ANCHOR_CENTER_TOP;
                    } else if ( val == "center-center" || val == "center_center" ){
                        style.anchorPoint = ANCHOR_CENTER_CENTER;
                    } else if ( val == "center-bottom" || val == "center_bottom" ){
                        style.anchorPoint = ANCHOR_CENTER_BOTTOM;
                    }
                    
                } else if ( keyValue[0] == "opacity" ){
                    style.opacity = ofToFloat(keyValue[1]);
                } else if ( keyValue[0] == "rotation" || keyValue[0] == "rotate" ){
                    style.rotation = ofToFloat(keyValue[1]);
                } else if ( keyValue[0] == "width" ){
                    style.width = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "height" ){
                    style.height = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "x" ){
                    style.x = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "y" ){
                    style.y = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "background-image" || keyValue[0] == "backgroundimage" ){
                    style.backgroundImage = cleanString(keyValue[1], true);
                    //ofLog() << keyValue[1];
                    //ofLog() << ofToString(keyValue[1]);
                    //ofLog() << SUI::images.count(keyValue[1]);
                    setImage(style.backgroundImage);
                } else if ( keyValue[0] == "overflow" ){
                    style.overflow = cleanString(keyValue[1], true);
                } else if ( keyValue[0] == "background-size" ){
                    keyValue[1] = cleanString(keyValue[1]);
                    vector<string> values = ofSplitString(keyValue[1],"%");
                    style.backgroundSizeX = (float)ofToInt(values[0])/100.0;
                    style.backgroundSizeY = (float)ofToInt(values[1])/100.0;
                }
            }
            
        }
    }
    
    static void parseActions(Actions& actions, map<string, map<string,string>> lines){
        for (auto it=lines.begin(); it!=lines.end(); ++it){
            //ofLog() << it->first << ":" << it->second;
            /*for (auto actionIt=it->second.begin(); actionIt!=it->second.end(); ++actionIt){
                
            }*/
            string id = it->first;
            id = cleanArrayBrackets(id);
            actions.items[id].parse(it->second);
        }
    }
    
    class StyleBaseParams: public Style {
    public:
        ~StyleBaseParams(){};
        StyleBaseParams(){
            baseStyle.resetBase();
        };
        
        map<int, Style> stateStyles;
        Style baseStyle;
        
    };
    
    class StylerenderParams {
    public:
        ~StylerenderParams(){};
        StylerenderParams(){};
        
        int state = 0;
        
        Style inlineStyle;
        Style compiledStyle;
    };
    
    class StyleSheet;
    class StyleSelector;
    
    struct suiStyleSheetArgs;
    
    struct suiStyleSelectorArgs;
    struct suiTweenArgs;
    
    class BlocksBase {
    public:
        ~BlocksBase(){};
        BlocksBase(){}
        map<string, vector<string>> blocks;
        
        vector<string> getBlock(string selector){
            vector<string> vecStr;
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                if ( it->first == selector ){
                    return it->second;
                }
            }
            
            return vecStr;
        }
    };
    
    class StyleSelector: public StyleBaseParams, public BlocksBase {
    public:
        ~StyleSelector(){
            stylesheet = 0;
            stylesheet = NULL;
        };
        StyleSelector(){
            Style s1;
            Style s2;
            stateStyles[1] = s1;
            stateStyles[2] = s2;
        };
        
        Actions actions;
        
        StyleSheet* stylesheet = NULL;
        
        void bind(StyleSheet& stylesheet);
        
        void reloadStyle(suiStyleSheetArgs& args);
        
        
        ofEvent<suiStyleSelectorArgs> onupdate;
        
        Style compile(int state ){
            Style style;
            //baseStyle.debugLog();
            style.copy(baseStyle);
            //style.debugLog();
            
            if ( state > 0 ) {
                Style& stateStyle = stateStyles[state];
                
                if ( !isnan(stateStyle.width) ) style.width = stateStyle.width;
                if ( !isnan(stateStyle.height) ) style.height = stateStyle.height;
                if ( !isnan(stateStyle.opacity) ) style.opacity = stateStyle.opacity;
                if ( !isnan(stateStyle.rotation) ) style.rotation = stateStyle.rotation;
                if ( !isnan(stateStyle.backgroundSizeX) ) style.backgroundSizeX = stateStyle.backgroundSizeX;
                if ( !isnan(stateStyle.backgroundSizeY) ) style.backgroundSizeY = stateStyle.backgroundSizeY;
                if ( stateStyle.backgroundImage != "" ) style.backgroundImage = stateStyle.backgroundImage;
                if ( stateStyle.overflow != "" ) style.overflow = stateStyle.overflow;
                if ( !isnan(stateStyle.anchorPoint) ) style.anchorPoint = stateStyle.anchorPoint;
                
            }
            
            
            /*if ( !isnan(inlineStyle.width) ) style.width = inlineStyle.width;
            if ( !isnan(inlineStyle.height) ) style.height = inlineStyle.height;
            if ( !isnan(inlineStyle.backgroundSizeX) ) style.backgroundSizeX = inlineStyle.backgroundSizeX;
            if ( !isnan(inlineStyle.backgroundSizeY) ) style.backgroundSizeY = inlineStyle.backgroundSizeY;
            if ( inlineStyle.backgroundImage != "" ) style.backgroundImage = inlineStyle.backgroundImage;
            if ( inlineStyle.overflow != "" ) style.overflow = inlineStyle.overflow;
            if ( !isnan(inlineStyle.anchorPoint) ) style.anchorPoint = inlineStyle.anchorPoint;
            if ( inlineStyle.hasBackgroundColor ) style.backgroundColor = inlineStyle.backgroundColor;*/
            
            return style;
        }
        
        void reset(){
            baseStyle.resetBase();
        }
        
        void runAction(string id, Element& el){
            actions.get(id).run(el);
        }
        
        void parseBlocks(){
            reset();
            if ( blocks.count("-render") != 0 ) parseStyle( baseStyle, getStyles(blocks["-render"]) );
            if ( blocks.count("-actions") != 0 ) parseActions( actions, getActionsStr(blocks["-actions"]) );
        }
        
        
    };
    
    
    
    class StyleSheet: public BlocksBase {
    public:
        ~StyleSheet(){};
        StyleSheet(){};
        StyleSheet(string filepath){
            load(filepath);
        };
        
        
        
        ofEvent<suiStyleSheetArgs> onReload;
        ofBuffer dataBuffer;
        string data;
        time_t fileTime = 0;
        float nextupdateTime = 0.0;
        
        bool liveReloading = false;
        float updateInterval = .05;
        map<string, StyleSelector*> selectors;
        
        
        //
        
        void load(string filepath){
            this->filepath = filepath;
            ofBuffer buffer = ofBufferFromFile(ofToDataPath(filepath), false);
            blocks = getBlocks(buffer);
            
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                if ( it->first != "[canvas]" && it->first != "[elements]" ){
                    if ( selectors.count(it->first) ){
                        selectors[it->first]->blocks = getBlocks(it->second);
                        selectors[it->first]->parseBlocks();
                        //selectors[it->first]
                        //ofLog() << "reload! " << ofRandomf();
                    } else {
                        StyleSelector* ss = new StyleSelector();
                        ss->bind(*this);
                        ss->blocks = getBlocks(it->second);
                        ss->parseBlocks();
                        selectors[it->first] = ss;
                    }
                    
                }
            }
            
            //data = dataBuffer.getText();
            
            //data = ofJoinString(ofSplitString(data," "), "");
            //data = ofJoinString(ofSplitString(data,"\n"), "");
            //data = ofJoinString(ofSplitString(data,"\t"), "");
            //ofLog() << "StyleSheet.load(): " << data;
            
            
            parseSettings( getBlocks(getBlock("[canvas]"))["-settings"] );
            
            
            if ( !liveReloading && SUI::settings.liveReload == true ) {
                nextupdateTime = ofGetElapsedTimef() + updateInterval;
                liveReloading = true;
                ofAddListener(ofEvents().update, this, &StyleSheet::updateHandler);
            }
            
            fileTime = std::filesystem::last_write_time(ofToDataPath(filepath));
        }
        
        void parseSettings(vector<string> lines){
            ofLog() << "parseSettings()";
            for (auto& line : lines ){
                string l = cleanSemiColon(line);
                vector<string> keyValue = ofSplitString(l,":");
                ofLog() << keyValue[0] << " > " << keyValue[1];
                if ( keyValue[0] == "live-reload" && keyValue[1] == "true" ){
                    liveReload(true);
                }
                
            }
        }
        
        StyleSelector& getSelector(string selector){
            return *selectors[selector];
        }
        
        bool hasSelector(string selector){
            return selectors.count(selector);
        }
        
        //
        
    private:
        
        void updateHandler(ofEventArgs & args){
            updateHandler();
        }
        
        void updateHandler();
        
        
        string filepath;
    };
    
    struct suiStyleSheetArgs {
        StyleSheet& stylesheet;
        
        suiStyleSheetArgs(StyleSheet &stylesheet):stylesheet(stylesheet){}
    };
    
    class Tween;
    class Canvas;
    
    class Element : public StylerenderParams, public Style, public CustomParams {
    public:
        ~Element(){
            //stylesheet = 0;
            //delete stylesheet;
        };
        
        Element(){};
        
        Element(StyleSelector& styleSelector, string id, Canvas& canvas){
            setup(styleSelector, id, canvas);
        }
        
        void setup(StyleSelector& styleSelector, string id, Canvas& canvas){
            this->styleSelector = &styleSelector;
            this->canvas = &canvas;
            this->id = id;
            ofAddListener(styleSelector.onupdate, this, &Element::updateStyle);
            refresh();
        }
        
        string id;
        string selector;
        
        Actions actions;
        //CustomParams params;
        StyleSelector* styleSelector = NULL;
        Canvas* canvas = NULL;
        Element* parent = NULL;
        
        Element* getParent(){
            return parent;
        }
        
        template <typename ArgumentsType, class ListenerClass>
        Tween* animate( float timeSeconds, string params, ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&) );
        
        Tween* animate( float timeSeconds, string params );
        
        Tween* tween = NULL;
        
        void stopTween();
        
        void storeTween(Tween* tween);
        
        void runAction(string id){
            styleSelector->runAction(id, *this);
        }
        
        /*void copyInlineStyles(){
            if ( !isnan(inlineStyle.x) ) x = inlineStyle.x;
            if ( !isnan(inlineStyle.y) ) y = inlineStyle.y;
            if ( !isnan(inlineStyle.width) ) width = inlineStyle.width;
            if ( !isnan(inlineStyle.height) ) height = inlineStyle.height;
            if ( !isnan(inlineStyle.backgroundSizeX) ) backgroundSizeX = inlineStyle.backgroundSizeX;
            if ( !isnan(inlineStyle.backgroundSizeY) ) backgroundSizeY = inlineStyle.backgroundSizeY;
            if ( inlineStyle.backgroundImage != "" ) {
                backgroundImage = inlineStyle.backgroundImage;
                if ( SUI::images.count(backgroundImage) == 0 ) SUI::images[backgroundImage] = new ofImage();
                SUI::images[backgroundImage]->load(ofToDataPath(backgroundImage));
            }
            if ( inlineStyle.overflow != "" ) overflow = inlineStyle.overflow;
            if ( !isnan(inlineStyle.anchorPoint) ) anchorPoint = inlineStyle.anchorPoint;
            if ( inlineStyle.hasBackgroundColor ) backgroundColor = inlineStyle.backgroundColor;
        }*/
        
        
        //
        
        /*Element& Style(string selector, StyleSheet& stylesheet, string id = ""){
            this->selector = selector;
            //parseStyle(stylesheet.SelectBlock(selector));
            //parseActions(stateStyles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
            
            
            return *this;
        }*/
        
        Element& setPosition(float x, float y, float z = 0.0){
            this->x = x;
            this->y = y;
            updateStyle();
            return *this;
        }
        
        Element& setSize(float w, float h){
            width = w;
            height = h;
            updateStyle();
            return *this;
        }
        
        Element& setWidth(float w){
            width = w;
            updateStyle();
            return *this;
        }
        
        Element& setHeight(float h){
            height = h;
            updateStyle();
            return *this;
        }
        
        Element& setBackgroundImage(string idPath){
            inlineStyle.backgroundImage = idPath;
            updateStyle();
            return *this;
        }
        
        Element& setBackgroundColor(string hexString){
            Style::setBackgroundColor(hexString);
            updateStyle();
            return *this;
        }
        
        bool hitTest(Element& element){
            if ( element.boundingRect.x == -1 && element.boundingRect.y == -1 && element.boundingRect.width == -1 && element.boundingRect.height == -1 ) return false;
            if ( boundingRect.x == -1 && boundingRect.y == -1 && boundingRect.width == -1 && boundingRect.height == -1 ) return false;
            
            //if ( element.x == 0 && element.y == 0 && element.width == 0 && element.height == 0 ) return false;
            //if ( x == 0 && y == 0 && width == 0 && height == 0 ) return false;
            
            //float dx = abs(boundingRect.x - element.boundingRect.x);
            //float dy = abs(boundingRect.y - element.boundingRect.y);
            
            /*if ( boundingRect.y + height >= element.boundingRect.y || boundingRect.y <= element.boundingRect.y + height) {
                if ( boundingRect.x + width >= element.boundingRect.x || boundingRect.x <= element.boundingRect.x + width){
                    return true;
                }
            }*/
            
            if ( isCornerInside(element) ) return true;
            if ( element.isCornerInside(*this) ) return true;
            
            return false;
        }
        
        bool isCornerInside(Element& element){
            if ( isPointInside(element.boundingRect.x, element.boundingRect.y ) ) return true;
            if ( isPointInside(element.boundingRect.x + element.boundingRect.width, element.boundingRect.y ) ) return true;
            if ( isPointInside(element.boundingRect.x, element.boundingRect.y + element.boundingRect.height ) ) return true;
            if ( isPointInside(element.boundingRect.x + element.boundingRect.width, element.boundingRect.y + element.boundingRect.height ) ) return true;
            
            return false;
        }
        
        bool isPointInside(int x, int y){
            if ( x >= boundingRect.x && x <= boundingRect.x + width && y >= boundingRect.y && y <= boundingRect.y + height ) return true;
            return false;
        }
        
        void refresh(){
            if ( styleSelector == NULL ) return;
            parseCustomParams(*this, styleSelector->blocks );
            //ofLog() << "pings:" << params.ints["pings"];
            updateStyle();
        }
        
        ofFbo fbo;
        
        void updateStyle(bool savePosition = false){
            if ( styleSelector == NULL ) return;
            glm::vec2 pos;
            if ( savePosition ) {
                pos.x = x;
                pos.y = y;
            }
            compiledStyle = styleSelector->compile(state);
            mergeStyles(inlineStyle, true);
            mergeStyles(compiledStyle);
            
            if ( savePosition ){
                setPosition(pos.x, pos.y);
            }
            
            
            if ( width >= 1.0 && height >= 1.0 ){
                if ( (fbo.isAllocated() && (width != fbo.getWidth() || height != fbo.getHeight())) || !fbo.isAllocated() ){
                    fbo.allocate( (int)width, (int)height, GL_RGBA );
                    fbo.begin();
                    ofClear(0);
                    fbo.end();
                }
            }
            
            
            render();
            
            /*if ( id == "ball" ){
                compiledStyle.debugLog();
                debugLog();
            }*/
            
            //copyInlineStyles();
        }
        
        
        
        
        
        
        
        ofRectangle& getBoundingRect(){
            return boundingRect;
        }
        
        void render(){
            if ( width <= 1.0 || height <= 1.0 ) return;
            fbo.begin();
            ofClear(0);
            
            if ( hasBackgroundColor ) {
                ofSetColor( backgroundColor, 255.0 );
                ofDrawRectangle(0, 0, width, height);
            }
            
            ofEnableAlphaBlending();
            ofSetColor(255);
            if ( backgroundImage != "" ) {
                
                ofImage* img = SUI::getImage(backgroundImage);
                
                if ( !isnan(backgroundSizeX) && !isnan(backgroundSizeY) ){
                    img->drawSubsection(0,0, width, height, 0, 0, img->getWidth()*backgroundSizeX, img->getHeight()*backgroundSizeY);
                } else {
                    int w = img->getWidth() < width ? img->getWidth() : width;
                    int h = img->getHeight() < height ? img->getHeight() : height;
                    
                    if ( compiledStyle.overflow == "hidden" ) img->drawSubsection(0,0, w, h, 0, 0, w, h);
                    else img->draw(0,0);
                }
                
                
                //img->drawSubsection(0, 0, compiledStyle.width, compiledStyle.height, 0,0, img->getWidth(), img->getHeight());
                
                img = 0;
                img = NULL;
                delete img;
            }
            fbo.end();
        }
        
        void draw(float offsetX = 0.0, float offsetY = 0.0){
            //compiledStyle.debugLog();
            
            if ( width != fbo.getWidth() || height != fbo.getHeight() ){
                update();
            }
            
            //ofLog() << isnan(position.x);
            //ofTranslate(position.x, position.y);
            
            
            
            //============================================================
            //============================================================
            
            boundingRect.x = x;
            boundingRect.y = y;
            
            //-------
            
            //============================================================
            //============================================================
            
            /*
             
             ANCHOR_LEFT_TOP,
             ANCHOR_LEFT_CENTER,
             ANCHOR_LEFT_BOTTOM,
             ANCHOR_RIGHT_TOP,
             ANCHOR_RIGHT_CENTER,
             ANCHOR_RIGHT_BOTTOM,
             ANCHOR_CENTER_TOP,
             ANCHOR_CENTER_CENTER,
             ANCHOR_CENTER_BOTTOM
             */
            
            switch ( compiledStyle.anchorPoint ){
                case ANCHOR_LEFT_TOP:
                    break;
                case ANCHOR_LEFT_CENTER:
                    boundingRect.y -= height*.5;
                    break;
                case ANCHOR_LEFT_BOTTOM:
                    boundingRect.y -= height;
                    break;
                    
                case ANCHOR_RIGHT_TOP:
                    boundingRect.y -= width;
                    break;
                case ANCHOR_RIGHT_CENTER:
                    boundingRect.x -= width;
                    boundingRect.y -= height*.5;
                    break;
                case ANCHOR_RIGHT_BOTTOM:
                    boundingRect.x -= width;
                    boundingRect.y -= height;
                    break;
                    
                case ANCHOR_CENTER_TOP:
                    boundingRect.x -= width*.5;
                    break;
                case ANCHOR_CENTER_CENTER:
                    boundingRect.x -= width*.5;
                    boundingRect.y -= height*.5;
                    break;
                case ANCHOR_CENTER_BOTTOM:
                    boundingRect.x -= width*.5;
                    boundingRect.y -= height;
                    break;
            }
            
            boundingRect.width = width;
            boundingRect.height = height;
            
            
            if ( width >= 1.0 && height >= 1.0 ) {
                //ofLog() << boundingRect;
                ofEnableAlphaBlending();
                
                ofPushView();
                //ofTranslate(boundingRect.x, boundingRect.y);
                ofTranslate(x, y);
                ofRotate(rotation);
                //ofTranslate();
                ofSetColor(255,255,255, 255.0*opacity);
                fbo.draw(boundingRect.x-x, boundingRect.y-y);
                ofPopView();
            }
            
            
            ofSetColor(255);
        }
        
    private:
        friend class Tween;
        ofRectangle boundingRect = ofRectangle(-1,-1,-1,-1);
        
        void updateStyle(suiStyleSelectorArgs& args){
            refresh();
        }
        
        void reloadStyle(suiStyleSheetArgs& args){
            //parseStyle(args.stylesheet.SelectBlock(selector));
            //parseActions(stateStyles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
        }
        
        void parseStyle(vector<string> block){
            //stateStyles[state] = style;
            
            //if ( !isnan(style.x) ) position.x = style.x;
            //if ( !isnan(style.y) ) position.y = style.y;
        }
        
        
        
        void parseActions(SUI::Style style){
            //actions.parse(style.getRawActions());
        }
        
        void update(){
            if ( width >= 1.0 && height >= 1.0 ) {
                if ( (fbo.isAllocated() && (width != fbo.getWidth() || height != fbo.getHeight())) || !fbo.isAllocated() ){
                    fbo.allocate( (int)width, (int)height, GL_RGBA );
                    fbo.begin();
                    ofClear(0);
                    fbo.end();
                }
            
                render();
            }
        }
        
    };
    
    struct suiCanvasEventArgs {
        CanvasEvent type;
        string triggerId;
        
        suiCanvasEventArgs(CanvasEvent type):type(type){};
    };
    
    struct suiElementEventArgs {
        
    };
    
    class Canvas: public CustomParams {
    public:
        ~Canvas(){
            stylesheet = 0;
            stylesheet = NULL;
        };
        Canvas(){};
        
        //
        
        //vector<Element*> elements;
        map<string, Element*> elements = map<string, Element*>();
        vector<string> managedrenderElements = vector<string>();
        vector<string> renderElements = vector<string>();
        StyleSheet* stylesheet = NULL;
        map<string, string> sounds;
        
        
        //CustomParams global;
        ofEvent<suiCanvasEventArgs> onTrigger;
        ofEvent<suiCanvasEventArgs> onLoaded;
        
        
        void parseSounds(map<string, vector<string>> blocks){
            vector<string> lines = getStyles( blocks["-sounds"] );
            
            for (auto& line : lines ){
                vector<string> keyValue = vector<string>(2);
                line = cleanString(line);
                
                int index = line.find_first_of(":");
                keyValue[0] = line.substr(0,index);
                keyValue[1] = line.substr(index+1);
                
                ofLog() << "[sound:" << keyValue[0] + "] " << keyValue[1];
                SUI::setSound( keyValue[1] );
                sounds[ keyValue[0] ] = keyValue[1];
            }
        }
        
        bool elementExists(string id){
            bool found = false;
            for (auto& elementId : managedrenderElements ){
                if ( elementId == id ) found = true;
            }
            
            for (auto& elementId : renderElements ){
                if ( elementId == id ) found = true;
            }
            
            if ( !found ) return false;
            
            for (auto it=elements.begin(); it!=elements.end(); ++it){
                if ( it->second->id == id ) {
                    return true;
                }
            }
            
            return false;
        }
        
        Element* getElementById(string id){
            bool found = false;
            for (auto& elementId : managedrenderElements ){
                if ( elementId == id ) found = true;
            }
            
            for (auto& elementId : renderElements ){
                if ( elementId == id ) found = true;
            }
            
            if ( !found ) return NULL;
            
            for (auto it=elements.begin(); it!=elements.end(); ++it){
                if ( it->second->id == id ) return it->second;
            }
            return NULL;
        }
        
        void emitTriggerEvent(string id){
            suiCanvasEventArgs args(CANVAS_EVENT_TRIGGER);
            args.triggerId = id;
            ofNotifyEvent(onTrigger, args, this);
        };
        
        void emitloadedEvent(){
            suiCanvasEventArgs args(CANVAS_EVENT_LOADED);
            ofNotifyEvent(onLoaded, args, this);
        };
        
        
        
        /*Element& addElement(){
            Element el;
            elements.push_back(el);
            return elements.back();
        }
        
        Element& addElement(string id){
            Element el(id);
            elements.push_back(el);
            return elements.back();
        }*/
        
        Element& addElement(StyleSheet& stylesheet, string selector, string id, bool reloadManaged = false){
            if ( !stylesheet.hasSelector(selector) ) return;
            Element* el = new Element(stylesheet.getSelector(selector), id, *this );
            //elements.push_back(el);
            elements[id] = el;
            if ( !reloadManaged ) renderElements.push_back(id);
            return *elements[id];
            //return *elements.back();
        }
        
        Element& addElement(string selector, string id, bool reloadManaged = false){
            //ofLog() << "[Add Element]  selector:" << selector << "  id:" << id;
            if ( !stylesheet->hasSelector(selector) ) return;
            Element* el = new Element(stylesheet->getSelector(selector), id, *this );
            //elements.push_back(el);
            //return *elements.back();
            elements[id] = el;
            if ( !reloadManaged ) renderElements.push_back(id);
            return *elements[id];
        }
        
        void pushElement(Element* element, string selector, string id){
            if ( !stylesheet->hasSelector(selector) ) return;
            element->setup(stylesheet->getSelector(selector), id, *this);
            elements[id] = element;
            renderElements.push_back(id);
        }
        
        //
        
        
        void update(ofEventArgs & args){
            update();
        }
        
        void update();
        
        void render(){};
        
        void load(string filepath){
            stylesheet = new StyleSheet();
            stylesheet->load(filepath);
            
            ofAddListener(stylesheet->onReload, this, &Canvas::reloadElements);
            
            reloadData();
        };
        
        void makeElements(){
            map<string, vector<string>> blocks = getBlocks(stylesheet->getBlock("[elements]"));
            managedrenderElements = vector<string>();
            
            for ( auto& key : blocks["[[-keys-]]"] ){
                string selector = cleanSelector(key);
                vector<string> nameSelector = ofSplitString(selector, ":");
                
                
                
                vector<string> styles = getStyles(blocks[key]);
                
                //ofLog() << "[Make] " << nameSelector[0] << ":" << nameSelector[1];
                managedrenderElements.push_back(nameSelector[0]);
                
                if ( elementExists(nameSelector[0]) ) {
                    Element* el = getElementById(nameSelector[0]);
                    
                    el->reset();
                    el->inlineStyle.reset();
                    parseStyle(el->inlineStyle, styles);
                    el->updateStyle();
                    //el->copyInlineStyles();
                    
                    el = 0;
                    el = NULL;
                } else {
                    if ( !stylesheet->hasSelector(nameSelector[1]) ) continue;
                    
                    Element& el = addElement(nameSelector[1], nameSelector[0], true);
                    //ofLog() << nameSelector[1] << "============";
                    parseStyle(el.inlineStyle, styles);
                    //ofLog() << el.inlineStyle.y;
                    el.updateStyle();
                    //el.copyInlineStyles();
                    //ofLog() << "---- ";
                    //ofLog() << " ";
                }
            }
            
            for (auto& elementId : renderElements ){
                if ( elements[elementId] != NULL ){
                    elements[elementId]->reset(false);
                    elements[elementId]->updateStyle(true);
                }
            }
            
        }
        
        void draw(){
            //for (auto element : boost::adaptors::reverse(elements) ){
            /*for (auto element : elements ){
                element->draw();
            }*/
            
            for (auto& elementId : managedrenderElements ){
                //ofLog() << elementId;
                if ( elements[elementId] != NULL ) elements[elementId]->draw();
            }
            
            for (auto& elementId : renderElements ){
                //ofLog() << elementId;
                if ( elements[elementId] != NULL ) elements[elementId]->draw();
            }
        };
        
        void reloadElements(suiStyleSheetArgs& args){
            reloadData();
        }
        
        
        
        void reloadData(){
            makeElements();
            map<string, vector<string>> canvasBlocks = getBlocks(stylesheet->getBlock("[canvas]"));
            parseCustomParams(*this, canvasBlocks );
            parseSounds( canvasBlocks );
            emitloadedEvent();
        }
        
        
        
        
    };
    
    
    struct suiTweenArgs;
    
    class Tween : public AnimatableParams {
    public:
        ~Tween(){};
        Tween(){
            valueNames.clear();
        };
        
        string id = "";
        
        string getID() {
            return id;
        }
        
        ofEvent<suiTweenArgs> onComplete;
        ofEvent<suiTweenArgs> onstart;
        ofEvent<suiTweenArgs> onStep;
        
        int UID = abs((int)ofRandom(7, 7777777777));
        
        float delay = 0.0;
        float startTime = 0.0;
        float endTime = 0.0;
        float duration = 0.0;
        float perc = 0.0;
        
        float getPercentCompleted(){
            return perc;
        }
        
        float getProgress(){
            return perc;
        }
        
        vector<string> valueNames = vector<string>();
        map<string, float> endValues;
        map<string, float> startValues;
        
        std::function<float(float,float,float,float)> ease = ofxeasing::linear::easeNone;
        
        Element* el;
        bool active = false;
        
        string cmd;
        bool firstStep = false;
        
        void start( Element* el, float timeSeconds, string params, bool attachToElement = true );
        void stop();
        void update( float currTime );
        void updateValues();
        void storeValue( string param, string val );
        void storeValue( string param, float val );
        void storeStartValues();
        void setID( string name ){
            id = name;
        }
        
        
        template <typename ArgumentsType, class ListenerClass>
        void storeComplete( ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&)){
            
        }
        
        
    };
    
    
    
    
    extern vector<Tween*> tweens;
    
    Tween* animate( Element* el, float timeSeconds, string params );
    
    template <typename ArgumentsType, class ListenerClass>
    static Tween* animate( Element* el, float timeSeconds, string params, ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&) ){
        
        Tween *t = new Tween();
        t->storeComplete( listener, listenerMethod );
        
        ofAddListener( t->onComplete, listener, listenerMethod );
        
        t->start( el, timeSeconds, params );
        
        tweens.push_back( t );
        return t;
    }
    
    extern vector<Tween*> tweensToDestroy;
    
    static void shoulddestroyTween( Tween* t ){
        tweensToDestroy.push_back( t );
    }
    
    void update();
    
    static void destroyTween(const Tween* t){
        /*int index = 0;
         for ( vector<Tween*>::iterator it = tweens.begin(); it != tweens.end(); it++ ){
         if ( (*it) == t ){
         //ofLog() << "found tween - destroying...";
         t = nullptr;
         tweens.erase( tweens.begin() + index );
         return;
         }
         index++;
         }*/
        
        auto it = std::find(tweens.begin(), tweens.end(), t);
        if (it != tweens.end()) { tweens.erase(it); }
    }
    
}

#endif
