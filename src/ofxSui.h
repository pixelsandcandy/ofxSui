#pragma once

#ifndef __ofxSui__
#define __ofxSui__

#include <algorithm>
#include <string>
#include <iostream>
#include "ofxEasing.h"
#include "ofMain.h"
#include <cmath>

using namespace std;

namespace SUI {
    
    struct Settings {
        bool liveReload = false;
    };
    
    extern Settings settings;
    
    void LiveReload(bool reload = true);
    
    class AnimatableParams {
    public:
        ~AnimatableParams(){};
        AnimatableParams(){};
        glm::vec3 position = glm::vec3(0.0);
        glm::vec2 size = glm::vec2(0.0);
        float width = 0.0;
        float height = 0.0;
        float scale = 1.0;
        float rotation = 0.0;
        float opacity = 1.0;
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
    
    class Action : public AnimatableParams , public TweenParams{
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
                    position.x = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "y" ){
                    position.y = ofToInt(keyValue[1]);
                } else if ( keyValue[0] == "duration" ){
                    duration = ofToFloat(keyValue[1]);
                } else if ( keyValue[0] == "easing" ){
                    //duration = ofToFloat(keyValue[1]);
                }  else if ( keyValue[0] == "oncomplete" ){
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
    
    class Style {
    public:
        ~Style(){};
        Style(){};
        
        ofColor backgroundColor = ofColor::white;
        float width = 0.0;
        float height = 0.0;
        float x = nanf("");
        float y = nanf("");
        vector<string> rawStyles;
        vector<string> styles;
        map<string, vector<string>> blocks;
        
        void ParseBlocks(){
            vector <string> strLines;
            string selector;
            bool blockOpen = false;
            int openCount = 0;
            
            for (auto line : rawStyles){
                if ( !blockOpen ){
                    if ( line.find("{") != string::npos ){
                        selector = line.substr(0, line.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    } else {
                        styles.push_back(line);
                    }
                } else {
                    strLines.push_back(line);
                    
                    if ( line.find("{") != string::npos ){
                        openCount++;
                    } else if ( line.find("}") != string::npos ){
                        openCount--;
                        if ( openCount == 0 ){
                            blocks[selector] = vector<string>(strLines);
                            ofLog() << "Style|Block: " << selector << "  lines:" << blocks[selector].size();
                            //ofLog() << ofJoinString(blocks[selector], " ");
                            blockOpen = false;
                        }
                    }
                }
            }
            
        }
        
        void Parse(vector<string> rawStyles){
            this->rawStyles = rawStyles;
            ParseBlocks();
            
            for ( auto s : styles ){
                //ofLog() << "s << " << s;
                if ( s.find("{") == string::npos && s.find("}") == string::npos && s != "" ){
                    vector<string> keyValue = ofSplitString(s, ":");
                    keyValue[1] = ofJoinString(ofSplitString(keyValue[1],";"), "");
                    ofLog() << keyValue[0] << ":" << keyValue[1];
                    //return;
                    if ( keyValue[0] == "background-color" ){
                        if ( keyValue[1].find("#") != string::npos ){
                            string col = ofJoinString(ofSplitString(keyValue[1],"#"), "");
                            float floatColor = stoul(col, nullptr, 16);
                            backgroundColor.setHex(floatColor);
                        }
                    } else if ( keyValue[0] == "width" ){
                        width = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "height" ){
                        height = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "x" ){
                        x = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "y" ){
                        y = ofToInt(keyValue[1]);
                    }
                }
                
            }
        }
        
        vector<string> GetRawActions(){
            return blocks["[actions]"];
        }
        
        
    };
    
    class StyleSheet {
    public:
        ~StyleSheet(){};
        StyleSheet(){};
        StyleSheet(string filepath){
            Load(filepath);
        };
        
        struct suiStyleSheetArgs {
            StyleSheet& stylesheet;
            
            suiStyleSheetArgs(StyleSheet &stylesheet):stylesheet(stylesheet){}
        };
        
        ofEvent<suiStyleSheetArgs> onUpdate;
        ofBuffer dataBuffer;
        string data;
        time_t fileTime = 0;
        float nextUpdateTime = 0.0;
        
        bool liveReloading = false;
        float updateInterval = 1.0;
        map<string, vector<string>> blocks;
        
        //
        
        void Load(string filepath){
            this->filepath = filepath;
            ofBuffer buffer = ofBufferFromFile(ofToDataPath(filepath), false);
            ParseBlocks(buffer);
            
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
        
        
        
        Style GetStyle(string selector){
            vector<string> styles;
            Style style;
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                if ( it->first == selector ){
                    style.Parse(it->second);
                }
            }
            
            /*string block;
            int startpos = data.find(selector);
            if ( startpos != string::npos ){
                block = data.substr(startpos+selector.length());
                int endpos = block.find("}");
                
                if ( endpos != string::npos ){
                    block = block.substr(1,endpos-1);
                }
            }*/
            
            //ofLog() << block;
            
            //styles = ofSplitString(block, ";");
            
            
            return style;
        }
        
        //
        
    private:
        
        void ParseBlocks(ofBuffer buffer){
            vector <string> strLines;
            string selector;
            bool blockOpen = false;
            int openCount = 0;
            
            for (auto line : buffer.getLines()){
                string l = ofJoinString(ofSplitString(line," "), "");
                l = ofJoinString(ofSplitString(l,"\n"), "");
                l = ofJoinString(ofSplitString(l,"\t"), "");
                //strLines.push_back(l);
                
                if ( !blockOpen ){
                    
                    if ( l.find("{") != string::npos ){
                        selector = l.substr(0, l.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines = vector<string>();
                    }
                    
                    /*if ( l.find("]{") != string::npos ){
                        selector = l.substr(0, l.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    } else if ( l.find("]:{") != string::npos ){
                        selector = l.substr(0, l.find(":"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    } else if ( l.find("&:") != string::npos && l.find("{") != string::npos){
                        selector = l.substr(0, l.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    } else if ( l.find("&:") != string::npos && l.find("{") != string::npos){
                        selector = l.substr(0, l.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    } else if ( l.find("{") != string::npos ){
                        selector = l.substr(0, l.find("{"));
                        blockOpen = true;
                        openCount = 1;
                        strLines.empty();
                    }*/
                } else {
                    strLines.push_back(l);
                    
                    if ( l.find("{") != string::npos ){
                        openCount++;
                    } else if ( l.find("}") != string::npos ){
                        openCount--;
                        if ( openCount == 0 ){
                            blocks[selector] = vector<string>(strLines);
                            ofLog() << "StyleSheet|Block: " << selector << "  lines:" << blocks[selector].size();
                            ofLog() << ofJoinString(blocks[selector], " ");
                            blockOpen = false;
                        }
                    }
                }
            }
        }
        
        void UpdateHandler(ofEventArgs & args){
            UpdateHandler();
        }
        
        void UpdateHandler(){
             //std::filesystem::last_write_time(ofToDataPath(it.second));
            
            if ( ofGetElapsedTimef() < nextUpdateTime ){
                nextUpdateTime = ofGetElapsedTimef() + updateInterval;
                time_t t = std::filesystem::last_write_time(ofToDataPath(this->filepath));
                if ( t > fileTime ){
                    fileTime = t;
                    Load(filepath);
                    suiStyleSheetArgs args(*this);
                    
                    ofNotifyEvent(onUpdate, args, this);
                };
            }
            
        }
        
        
        string filepath;
    };
    
    
    class Element : public AnimatableParams {
    public:
        ~Element(){
            //stylesheet = 0;
            //delete stylesheet;
        };
        Element(){};
        Element(string id){
            this->id = id;
            SUI::Style s;
            styles[0] = s;
        }
        
        string id;
        string selector;
        
        int state = 0;
        map<int, Style> styles;
        Actions actions;
        
        //
        
        Element& Style(string selector, StyleSheet& stylesheet){
            this->selector = selector;
            ParseStyle(stylesheet.GetStyle(selector));
            ParseActions(styles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
            ofAddListener(stylesheet.onUpdate, this, &Element::UpdateStyle);
            
            return *this;
        }
        
        Element& SetPosition(float x, float y, float z = 0.0){
            position = glm::vec3(x, y, z);
            return *this;
        }
        
        Element& SetSize(float w, float h){
            size = glm::vec2(w,h);
            return *this;
        }
        
        void Draw(float x = 0.0, float y = 0.0){
            ofSetColor( styles[state].backgroundColor );
            ofPushView();
            ofTranslate(position.x, position.y);
            ofDrawRectangle(0, 0, styles[state].width, styles[state].height);
            ofPopView();
        }
        
    private:
        
        void UpdateStyle(StyleSheet::suiStyleSheetArgs& args){
            ParseStyle(args.stylesheet.GetStyle(selector));
            ParseActions(styles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
        }
        
        void ParseStyle(SUI::Style style){
            styles[state] = style;
            if ( !isnan(style.x) ) position.x = style.x;
            if ( !isnan(style.y) ) position.y = style.y;
        }
        
        void ParseActions(SUI::Style style){
            actions.Parse(style.GetRawActions());
        }
        
    };
    
    
    class Window {
    public:
        ~Window(){};
        Window(){};
        
        //
        
        vector<Element> elements;
        
        Element& AddElement(){
            Element el;
            elements.push_back(el);
            return elements.back();
        }
        
        Element& AddElement(string id){
            Element el(id);
            elements.push_back(el);
            return elements.back();
        }
        
        //
        
        
        void Update(ofEventArgs & args){}
        
        void Update(){};
        
        void Render(){};
        
        void Draw(){
            for (auto element : elements ){
                element.Draw();
            }
        };
        
        
        
        
    };
    
}

#endif
