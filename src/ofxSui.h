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
    
    void LiveReload(bool reload = true);
    extern map<string, ofImage*> images;
    
    
    //========================================================================================================
    //========================================================================================================
    
    static string CleanSelector(string selector){
        selector = ofJoinString(ofSplitString(selector,"("), "");
        selector = ofJoinString(ofSplitString(selector,")"), "");
        selector = ofJoinString(ofSplitString(selector,"["), "");
        selector = ofJoinString(ofSplitString(selector,"]"), "");
        return selector;
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
    
    static string CleanString(string s, bool removeSpaces = false, bool removeTrailingComma = true){
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
    
    static vector<string> GetStyles(vector<string> lines){
        vector <string> styleLines;
        
        string selector;
        bool blockOpen = false;
        bool commentOpen = false;
        
        int blockOpenCount = 0;
        int commentOpenCount = 0;
        
        for (auto line : lines){
            
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
    
    static map<string, vector<string>> GetBlocks(vector<string> lines){
        vector<string> keys;
        map<string, vector<string>> blocks;
        vector <string> strLines;
        
        string selector;
        bool blockOpen = false;
        bool commentOpen = false;
        
        int blockOpenCount = 0;
        int commentOpenCount = 0;
        
        for (auto line : lines){
            
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
                        ofLog() << "[Block] " << selector << "    lines:" << blocks[selector].size();
                        ofLog() << ofJoinString(blocks[selector], "\n");
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
    
    
    static map<string, vector<string>> GetBlocks(ofBuffer buffer){
        vector<string> lines;
        for (auto line : buffer.getLines()){
            lines.push_back(line);
        }
        return GetBlocks(lines);
    }
    
    static void ParseCustomParams(SUI::CustomParams& params, map<string, vector<string>> blocks){
        vector<string> lines = GetStyles( blocks["-params"] );
        
        for (auto line : lines ){
            //ofLog() << line;
            vector<string> keyValue = vector<string>(2);
            line = CleanString(line);
            
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
            
            ofLog() << keyValue[0] << " – " << keyValue[1];
            
        }
    }
    
    static void ParseCustomParams(SUI::CustomParams& params, vector<string> lines){
        
        for (auto line : lines ){
            //ofLog() << line;
            vector<string> keyValue = vector<string>(2);
            line = CleanString(line);
            
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
        
        void ResetAnimatableParams(){
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
    
    class TweenParams {
    public:
        ~TweenParams(){};
        TweenParams(){};
        
        float duration = .5;
        string easing = "Quad.easeOut";
        
        vector<string> onComplete = vector<string>();
        vector<string> onStart = vector<string>();
    };
    
    
    
    
    
    class Action : public AnimatableParams, public TweenParams{
    public:
        ~Action(){};
        Action(){};
        
        string id;
        
        void Parse(vector<string> params){
            ofLog() << "================";
            
            for (auto line : params){
                ofLog() << line;
                vector<string> keyValue = vector<string>(2);
                int startPos = line.find_first_of(":");
                string str = line;
                keyValue[0] = str.substr(0,startPos);
                keyValue[1] = str.substr(startPos+1);
                keyValue[1] = ofJoinString(ofSplitString(keyValue[1],";"), "");
                //ofLog() << keyValue[0] << ":" << keyValue[1];
                //return;
                if ( keyValue[0] == "background-color" ){
                    /*if ( keyValue[1].find("#") != string::npos ){
                        string col = ofJoinString(ofSplitString(keyValue[1],"#"), "");
                        float floatColor = stoul(col, nullptr, 16);
                        backgroundColor.setHex(floatColor);
                    }*/
                } else if ( keyValue[0] == "width" ){
                    width = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "height" ){
                    height = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "x" ){
                    x = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "y" ){
                    y = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "duration" ){
                    duration = ofToFloat(keyValue[1]);
                } else if ( keyValue[0] == "easing" ){
                    //duration = ofToFloat(keyValue[1]);
                } else if ( keyValue[0] == "oncomplete" ){
                    onComplete = vector<string>();
                    string s = ofJoinString(ofSplitString(keyValue[1],"["), "");
                    s = ofJoinString(ofSplitString(s,"]"), "");
                    s = ofJoinString(ofSplitString(s,"\""), "");
                    onComplete = ofSplitString(s,",");
                    ofLog() << ofJoinString(onComplete, " || ");
                }
                
            }
            
        }
        
    };
    
    class Actions {
    public:
        ~Actions(){};
        Actions(){};
        
        map<string, vector<string>> blocks;
        map<string, Action> actionsMap;
        
        void Parse(vector<string> rawActions){
            ofLog() << "rawActions: " << ofJoinString(rawActions, "");
            
            vector <string> strLines;
            string selector;
            bool blockOpen = false;
            int openCount = 0;
            
            for (auto line : rawActions){
                if ( !blockOpen ){
                    if ( line.find("{") != string::npos ){
                        selector = line.substr(1, line.find("{")-2);
                        blockOpen = true;
                        openCount = 1;
                        strLines = vector<string>();
                    }
                } else {
                    
                    
                    if ( line.find("{") != string::npos ){
                        openCount++;
                        strLines.push_back(line);
                    } else if ( line.find("}") != string::npos ){
                        openCount--;
                        if ( openCount == 0 ){
                            blocks[selector] = vector<string>(strLines);
                            ofLog() << "Action: " << selector << "  lines:" << blocks[selector].size();
                            ofLog() << ofJoinString(blocks[selector], " ");
                            blockOpen = false;
                        } else {
                            strLines.push_back(line);
                        }
                        
                    } else {
                        strLines.push_back(line);
                    }
                }
            }
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                Action action;
                action.id = it->first;
                action.Parse(it->second);
                
                actionsMap[it->first] = action;
                
            }
            
        }
    };
    
    class Style : public AnimatableParams {
    public:
        ~Style(){};
        Style(){};
        
        
        
        void Copy(Style& copyStyle){
            backgroundColor = copyStyle.backgroundColor;
            backgroundImage = copyStyle.backgroundImage;
            overflow = copyStyle.overflow;
            width = copyStyle.width;
            height = copyStyle.height;
            backgroundSizeX = copyStyle.backgroundSizeX;
            backgroundSizeY = copyStyle.backgroundSizeY;
            x = copyStyle.x;
            y = copyStyle.x;
            anchorPoint = copyStyle.anchorPoint;
            hasBackgroundColor = copyStyle.hasBackgroundColor;
        }
        
        void Reset(){
            x = nanf("");
            y = nanf("");
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
        }
        
        void ResetBase(){
            ResetAnimatableParams();
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
            
            anchorPoint = ANCHOR_LEFT_TOP;
            overflow = "auto";
            backgroundColor = ofColor::white;
            backgroundImage = "";
            //hasBackgroundColor = true;
            
        }
        
        void DebugLog(){
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
        
        void SetBackgroundColor(string hex){
            string col = ofJoinString(ofSplitString(hex,"#"), "");
            float floatColor = stoul(col, nullptr, 16);
            backgroundColor.setHex(floatColor);
            hasBackgroundColor = true;
        }
        
        
        bool hasBackgroundColor = false;
        ofColor backgroundColor = ofColor::white;
        string backgroundImage = "";
        string overflow = "";
        vector<string> rawStyles;
        vector<string> styles;
        //map<string, vector<string>> blocks;
        
    };
    
    static void ParseStyle(Style& style, vector<string> lines){
        for ( auto line : lines ){
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
                        style.SetBackgroundColor(keyValue[1]);
                    }
                } else if ( keyValue[0] == "anchorpoint" ){
                    //baseStyle.width = ofToInt(keyValue[1]);
                    string val = CleanString(ofToLower(keyValue[1]), true);
                    
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
                    
                } else if ( keyValue[0] == "width" ){
                    style.width = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "height" ){
                    style.height = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "x" ){
                    style.x = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "y" ){
                    style.y = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "background-image" || keyValue[0] == "backgroundimage" ){
                    style.backgroundImage = CleanString(keyValue[1], true);
                    //ofLog() << keyValue[1];
                    //ofLog() << ofToString(keyValue[1]);
                    //ofLog() << SUI::images.count(keyValue[1]);
                    if ( SUI::images.count(keyValue[1]) == 0 ) SUI::images[style.backgroundImage] = new ofImage();
                    SUI::images[style.backgroundImage]->load(ofToDataPath(style.backgroundImage));
                } else if ( keyValue[0] == "overflow" ){
                    style.overflow = CleanString(keyValue[1], true);
                } else if ( keyValue[0] == "background-size" ){
                    keyValue[1] = CleanString(keyValue[1]);
                    vector<string> values = ofSplitString(keyValue[1],"%");
                    style.backgroundSizeX = (float)ofToInt(values[0])/100.0;
                    style.backgroundSizeY = (float)ofToInt(values[1])/100.0;
                }
            }
            
        }
    }
    
    class StyleBaseParams: public Style {
    public:
        ~StyleBaseParams(){};
        StyleBaseParams(){
            baseStyle.ResetBase();
        };
        
        map<int, Style> stateStyles;
        Style baseStyle;
        
    };
    
    class StyleRenderParams {
    public:
        ~StyleRenderParams(){};
        StyleRenderParams(){};
        
        int state = 0;
        
        Style inlineStyle;
        Style compiledStyle;
    };
    
    class StyleSheet;
    class StyleSelector;
    
    struct suiStyleSheetArgs;
    
    struct suiStyleSelectorArgs;
    
    class BlocksBase {
    public:
        ~BlocksBase(){};
        BlocksBase(){}
        map<string, vector<string>> blocks;
        
        vector<string> GetBlock(string selector){
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
        
        StyleSheet* stylesheet = NULL;
        
        void Bind(StyleSheet& stylesheet);
        
        void ReloadStyle(suiStyleSheetArgs& args);
        
        
        ofEvent<suiStyleSelectorArgs> onUpdate;
        
        Style Compile(int state, Style& inlineStyle ){
            Style style;
            //baseStyle.DebugLog();
            style.Copy(baseStyle);
            //style.DebugLog();
            
            if ( state > 0 ) {
                Style& stateStyle = stateStyles[state];
                
                if ( !isnan(stateStyle.width) ) style.width = stateStyle.width;
                if ( !isnan(stateStyle.height) ) style.height = stateStyle.height;
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
        
        void Reset(){
            baseStyle.ResetBase();
        }
        
        void ParseBlocks(){
            Reset();
            ParseStyle( baseStyle, GetStyles(blocks["-render"]) );
        }
        
        
    };
    
    
    
    class StyleSheet: public BlocksBase {
    public:
        ~StyleSheet(){};
        StyleSheet(){};
        StyleSheet(string filepath){
            Load(filepath);
        };
        
        
        
        ofEvent<suiStyleSheetArgs> onReload;
        ofBuffer dataBuffer;
        string data;
        time_t fileTime = 0;
        float nextUpdateTime = 0.0;
        
        bool liveReloading = false;
        float updateInterval = 1.0;
        map<string, StyleSelector*> selectors;
        
        
        //
        
        void Load(string filepath){
            this->filepath = filepath;
            ofBuffer buffer = ofBufferFromFile(ofToDataPath(filepath), false);
            blocks = GetBlocks(buffer);
            
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                if ( it->first != "[global]" && it->first != "[elements]" ){
                    if ( selectors.count(it->first) ){
                        selectors[it->first]->blocks = GetBlocks(it->second);
                        selectors[it->first]->ParseBlocks();
                        //selectors[it->first]
                        ofLog() << "reload! " << ofRandomf();
                    } else {
                        StyleSelector* ss = new StyleSelector();
                        ss->Bind(*this);
                        ss->blocks = GetBlocks(it->second);
                        ss->ParseBlocks();
                        selectors[it->first] = ss;
                    }
                    
                }
            }
            
            //data = dataBuffer.getText();
            
            //data = ofJoinString(ofSplitString(data," "), "");
            //data = ofJoinString(ofSplitString(data,"\n"), "");
            //data = ofJoinString(ofSplitString(data,"\t"), "");
            //ofLog() << "StyleSheet.Load(): " << data;
            
            
            
            if ( !liveReloading && SUI::settings.liveReload == true ) {
                nextUpdateTime = ofGetElapsedTimef() + updateInterval;
                liveReloading = true;
                ofAddListener(ofEvents().update, this, &StyleSheet::UpdateHandler);
            }
            
            fileTime = std::filesystem::last_write_time(ofToDataPath(filepath));
        }
        
        StyleSelector& GetSelector(string selector){
            return *selectors[selector];
        }
        
        bool HasSelector(string selector){
            return selectors.count(selector);
        }
        
        //
        
    private:
        
        void UpdateHandler(ofEventArgs & args){
            UpdateHandler();
        }
        
        void UpdateHandler();
        
        
        string filepath;
    };
    
    struct suiStyleSheetArgs {
        StyleSheet& stylesheet;
        
        suiStyleSheetArgs(StyleSheet &stylesheet):stylesheet(stylesheet){}
    };
    
    class Element : public StyleRenderParams, public Style {
    public:
        ~Element(){
            //stylesheet = 0;
            //delete stylesheet;
        };
        
        Element(StyleSelector& styleSelector, string id = ""):styleSelector(styleSelector){
            this->id = id;
            ofAddListener(styleSelector.onUpdate, this, &Element::UpdateStyle);
            Refresh();
        }
        
        string id;
        string selector;
        
        Actions actions;
        CustomParams params;
        StyleSelector& styleSelector;
        
        void CopyInlineStyles(){
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
        }
        
        
        //
        
        /*Element& Style(string selector, StyleSheet& stylesheet, string id = ""){
            this->selector = selector;
            //ParseStyle(stylesheet.SelectBlock(selector));
            //ParseActions(stateStyles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
            
            
            return *this;
        }*/
        
        Element& SetPosition(float x, float y, float z = 0.0){
            this->x = x;
            this->y = y;
            UpdateStyle();
            return *this;
        }
        
        Element& SetSize(float w, float h){
            width = w;
            height = h;
            UpdateStyle();
            return *this;
        }
        
        Element& SetBackgroundColor(string hexString){
            //inlineStyle.SetBackgroundColor(hexString);
            Style::SetBackgroundColor(hexString);
            UpdateStyle();
            return *this;
        }
        
        bool HitTest(Element& element){
            
            float dx = abs(finalX - element.finalX);
            float dy = abs(finalY - element.finalY);
            
            if ( finalY <= element.finalY ) {
                if ( dy <= finalHeight ){
                    if ( finalX <= element.finalX ){
                        if ( dx <= finalWidth ) return true;
                    } else {
                        if ( dx <= element.finalWidth ) return true;
                    }
                }
            } else {
                if ( dy <= element.finalHeight ){
                    if ( finalX <= element.finalX ){
                        if ( dx <= finalWidth ) return true;
                    } else {
                        if ( dx <= element.finalWidth ) return true;
                    }
                }
            }
            
            return false;
        }
        
        bool IsPointInside(int x, int y){
            
            
            return false;
        }
        
        void Refresh(){
            ParseCustomParams(params, styleSelector.blocks );
            ofLog() << "pings:" << params.ints["pings"];
            UpdateStyle();
        }
        
        void UpdateStyle(){
            compiledStyle = styleSelector.Compile(state, inlineStyle);
        }
        
        float finalX;
        float finalY;
        
        float finalWidth;
        float finalHeight;
        
        void Draw(float offsetX = 0.0, float offsetY = 0.0){
            //compiledStyle.DebugLog();
            
            
            ofPushView();
            //ofLog() << isnan(position.x);
            //ofTranslate(position.x, position.y);
            
            
            
            //============================================================
            //============================================================
            
            float tempX;
            float tempY;
            
            if ( isnan(x) ){
                tempX = compiledStyle.x;
            } else {
                tempX = x;
            }
            
            if ( isnan(y) ){
                tempY = compiledStyle.y;
            } else {
                tempY = y;
            }
            
            //-------
            
            if ( isnan(width) ){
                finalWidth = compiledStyle.width;
            } else {
                finalWidth = width;
            }
            
            if ( isnan(height) ){
                finalHeight = compiledStyle.height;
            } else {
                finalHeight = height;
            }
            
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
                    tempY -= finalHeight*.5;
                    break;
                case ANCHOR_LEFT_BOTTOM:
                    tempY -= finalHeight;
                    break;
                    
                case ANCHOR_RIGHT_TOP:
                    tempY -= finalWidth;
                    break;
                case ANCHOR_RIGHT_CENTER:
                    tempX -= finalWidth;
                    tempY -= finalHeight*.5;
                    break;
                case ANCHOR_RIGHT_BOTTOM:
                    tempX -= finalWidth;
                    tempY -= finalHeight;
                    break;
                    
                case ANCHOR_CENTER_TOP:
                    tempX -= finalWidth*.5;
                    break;
                case ANCHOR_CENTER_CENTER:
                    tempX -= finalWidth*.5;
                    tempY -= finalHeight*.5;
                    break;
                case ANCHOR_CENTER_BOTTOM:
                    tempX -= finalWidth*.5;
                    tempY -= finalHeight;
                    break;
            }
            
            finalX = tempX;
            finalY = tempY;
            
            ofTranslate(finalX, finalY);
            
            if ( hasBackgroundColor ) {
                ofSetColor( backgroundColor );
                ofDrawRectangle(0, 0, finalWidth, finalHeight);
            } else {
                if ( compiledStyle.hasBackgroundColor ) {
                    ofSetColor( compiledStyle.backgroundColor );
                    ofDrawRectangle(0, 0, finalWidth, finalHeight);
                }
            }
            
            
            ofSetColor(255);
            if ( compiledStyle.backgroundImage != "" || backgroundImage != "" ) {
                string bgFilepath;
                if ( backgroundImage != "" ) bgFilepath = backgroundImage;
                else bgFilepath = compiledStyle.backgroundImage;
                
                ofImage* img = SUI::images[bgFilepath];
                
                if ( !isnan(backgroundSizeX) && !isnan(backgroundSizeY) ){
                    img->drawSubsection(0,0, finalWidth, finalHeight, 0, 0, img->getWidth()*backgroundSizeX, img->getHeight()*backgroundSizeY);
                } else if ( !isnan(compiledStyle.backgroundSizeX) && !isnan(compiledStyle.backgroundSizeY) ){
                    img->drawSubsection(0,0, finalWidth, finalHeight, 0, 0, img->getWidth()*compiledStyle.backgroundSizeX, img->getHeight()*compiledStyle.backgroundSizeY);
                } else {
                    int w = img->getWidth() < finalWidth ? img->getWidth() : finalWidth;
                    int h = img->getHeight() < finalHeight ? img->getHeight() : finalHeight;
                    
                    if ( compiledStyle.overflow == "hidden" ) img->drawSubsection(0,0, w, h, 0, 0, w, h);
                    else img->draw(0,0);
                }
                
                
                //img->drawSubsection(0, 0, compiledStyle.width, compiledStyle.height, 0,0, img->getWidth(), img->getHeight());
                
                img = 0;
                img = NULL;
                delete img;
            }
            ofPopView();
        }
        
    private:
        void UpdateStyle(suiStyleSelectorArgs& args){
            Refresh();
        }
        
        void ReloadStyle(suiStyleSheetArgs& args){
            //ParseStyle(args.stylesheet.SelectBlock(selector));
            //ParseActions(stateStyles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
        }
        
        void ParseStyle(vector<string> block){
            //stateStyles[state] = style;
            
            //if ( !isnan(style.x) ) position.x = style.x;
            //if ( !isnan(style.y) ) position.y = style.y;
        }
        
        
        
        void ParseActions(SUI::Style style){
            //actions.Parse(style.GetRawActions());
        }
        
    };
    
    
    class Canvas {
    public:
        ~Canvas(){
            stylesheet = 0;
            stylesheet = NULL;
        };
        Canvas(){};
        
        //
        
        vector<Element*> elements;
        StyleSheet* stylesheet = NULL;
        
        CustomParams global;
        
        
        bool ElementExists(string id){
            for (auto element : elements ){
                if ( element->id == id ) return true;
            }
            return false;
        }
        
        Element* GetElementById(string id){
            for (auto element : elements ){
                if ( element->id == id ) return element;
            }
            return NULL;
        }
        
        /*Element& AddElement(){
            Element el;
            elements.push_back(el);
            return elements.back();
        }
        
        Element& AddElement(string id){
            Element el(id);
            elements.push_back(el);
            return elements.back();
        }*/
        
        Element& AddElement(StyleSheet& stylesheet, string selector, string id = ""){
            if ( !stylesheet.HasSelector(selector) ) return;
            Element* el = new Element(stylesheet.GetSelector(selector), id );
            elements.push_back(el);
            return *elements.back();
        }
        
        Element& AddElement(string selector, string id = ""){
            ofLog() << "[Add Element] " << selector;
            if ( !stylesheet->HasSelector(selector) ) return;
            Element* el = new Element(stylesheet->GetSelector(selector), id );
            elements.push_back(el);
            return *elements.back();
        }
        
        //
        
        
        void Update(ofEventArgs & args){}
        
        void Update(){};
        
        void Render(){};
        
        void Load(string filepath){
            stylesheet = new StyleSheet();
            stylesheet->Load(filepath);
            
            ofAddListener(stylesheet->onReload, this, &Canvas::ReloadElements);
            
            ReloadData();
        };
        
        void MakeElements(){
            map<string, vector<string>> blocks = GetBlocks(stylesheet->GetBlock("[elements]"));
            
            for ( auto key : blocks["[[-keys-]]"] ){
                string selector = CleanSelector(key);
                vector<string> nameSelector = ofSplitString(selector, ":");
                
                vector<string> styles = GetStyles(blocks[key]);
                
                ofLog() << "[Make] " << nameSelector[0] << ":" << nameSelector[1];
                
                if ( ElementExists(nameSelector[0]) ) {
                    Element* el = GetElementById(nameSelector[0]);
                    
                    el->Reset();
                    el->inlineStyle.Reset();
                    ParseStyle(el->inlineStyle, styles);
                    el->CopyInlineStyles();
                    
                    el = 0;
                    el = NULL;
                } else {
                    if ( !stylesheet->HasSelector(nameSelector[1]) ) continue;
                    
                    Element& el = AddElement(nameSelector[1], nameSelector[0]);
                    ofLog() << nameSelector[1] << "============";
                    ParseStyle(el.inlineStyle, styles);
                    ofLog() << el.inlineStyle.y;
                    el.CopyInlineStyles();
                    ofLog() << "---- ";
                    ofLog() << " ";
                }
            }
        }
        
        void Draw(){
            //for (auto element : boost::adaptors::reverse(elements) ){
            for (auto element : elements ){
                element->Draw();
            }
        };
        
        void ReloadElements(suiStyleSheetArgs& args){
            ReloadData();
        }
        
        
        
        void ReloadData(){
            MakeElements();
            ParseCustomParams(global, GetBlocks(stylesheet->GetBlock("[global]")));
        }
        
        
        
        
    };
    
}

#endif
