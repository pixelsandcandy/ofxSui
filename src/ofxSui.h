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
    
    class Style {
    public:
        ~Style(){};
        Style(){};
        
        ofColor backgroundColor = ofColor::white;
        float width = 0.0;
        float height = 0.0;
        float x = nanf("");
        float y = nanf("");
        
        void Parse(vector<string> styles){
            for ( auto s : styles ){
                vector<string> keyValue = ofSplitString(s, ":");
                keyValue[1] = ofJoinString(ofSplitString(keyValue[1],";"), "");
                //ofLog() << keyValue[0] << ":" << keyValue[1];
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
        string data;
        time_t fileTime = 0;
        
        bool liveReloading = false;
        
        //
        
        void Load(string filepath){
            this->filepath = filepath;
            ofBuffer buffer = ofBufferFromFile(ofToDataPath(filepath), false);
            data = buffer.getText();
            data = ofJoinString(ofSplitString(data," "), "");
            data = ofJoinString(ofSplitString(data,"\n"), "");
            data = ofJoinString(ofSplitString(data,"\t"), "");
            ofLog() << "StyleSheet.Load(): " << data;
            
            if ( !liveReloading && SUI::settings.liveReload == true ) {
                liveReloading = true;
                ofAddListener(ofEvents().update, this, &StyleSheet::UpdateHandler);
            }
            
            fileTime = std::filesystem::last_write_time(ofToDataPath(filepath));
        }
        
        Style GetStyle(string selector){
            vector<string> styles;
            string block;
            int startpos = data.find(selector);
            if ( startpos != string::npos ){
                block = data.substr(startpos+selector.length());
                int endpos = block.find("}");
                
                if ( endpos != string::npos ){
                    block = block.substr(1,endpos-1);
                }
            }
            
            ofLog() << block;
            
            styles = ofSplitString(block, ";");
            Style style;
            style.Parse(styles);
            return style;
        }
        
        //
        
    private:
        void UpdateHandler(ofEventArgs & args){
            UpdateHandler();
        }
        
        void UpdateHandler(){
             //std::filesystem::last_write_time(ofToDataPath(it.second));
            time_t t = std::filesystem::last_write_time(ofToDataPath(this->filepath));
            if ( t > fileTime ){
                fileTime = t;
                Load(filepath);
                suiStyleSheetArgs args(*this);
                
                ofNotifyEvent(onUpdate, args, this);
            };
        }
        
        
        string filepath;
    };
    
    
    class Element {
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
        
        glm::vec3 position = glm::vec3(0.0);
        glm::vec2 size = glm::vec2(0.0);
        
        //
        
        Element& Style(string selector, StyleSheet& stylesheet){
            this->selector = selector;
            ParseStyle(stylesheet.GetStyle(selector));
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
            //ofLog() << styles[state].width << "x" << styles[state].height;
        }
        
        void ParseStyle(SUI::Style style){
            styles[state] = style;
            if ( !isnan(style.x) ) position.x = style.x;
            if ( !isnan(style.y) ) position.y = style.y;
            
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
