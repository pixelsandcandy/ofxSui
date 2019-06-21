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
    
    
    //========================================================================================================
    //========================================================================================================
    
    static string CleanSelector(string selector){
        selector = ofJoinString(ofSplitString(selector,"("), "");
        selector = ofJoinString(ofSplitString(selector,")"), "");
        selector = ofJoinString(ofSplitString(selector,"["), "");
        selector = ofJoinString(ofSplitString(selector,"]"), "");
        return selector;
    }
    
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
    
    //========================================================================================================
    //========================================================================================================
    
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
    
    class CustomParams {
    public:
        ~CustomParams(){};
        CustomParams(){};
        
        map<string,bool> bools;
        map<string,string> strings;
        map<string,int> ints;
        map<string,float> floats;
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
        
        void Copy(Style& copyStyle){
            backgroundColor = copyStyle.backgroundColor;
            width = copyStyle.width;
            height = copyStyle.height;
        }
        
        void DebugLog(){
            ofLog() << "<<-------------";
            ofLog() << "background-color: " << ofToString(backgroundColor.getHex());
            ofLog() << "width: " << width;
            ofLog() << "height: " << height;
            
            ofLog() << "------------->>";
        }
        
        ofColor backgroundColor = ofColor::white;
        float width = nanf("");
        float height = nanf("");
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
    
    class StyleBaseParams: public Style {
    public:
        ~StyleBaseParams(){};
        StyleBaseParams(){};
        
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
    
    class StyleSelector: public StyleBaseParams {
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
        map<string, vector<string>> blocks;
        
        void Bind(StyleSheet& stylesheet);
        
        void ReloadStyle(suiStyleSheetArgs& args);
        
        
        ofEvent<suiStyleSelectorArgs> onUpdate;
        
        Style Compile(int state, Style& inlineStyles ){
            Style style;
            //baseStyle.DebugLog();
            style.Copy(baseStyle);
            style.DebugLog();
            
            if ( state > 0 ) {
                Style& stateStyle = stateStyles[state];
                
                if ( !isnan(stateStyle.width) ) style.width = stateStyle.width;
                if ( !isnan(stateStyle.height) ) style.height = stateStyle.height;
            }
            
            return style;
        }
        
        
        
        void ParseBlocks(){
            ParseStyle( GetStyles(blocks["-render"]) );
        }
        
        void ParseStyle(vector<string> lines){
            for ( auto line : lines ){
                //ofLog() << "s << " << s;
                if ( line.find("{") == string::npos && line.find("}") == string::npos && line != "" ){
                    vector<string> keyValue = ofSplitString(line, ":");
                    keyValue[1] = ofJoinString(ofSplitString(keyValue[1],";"), "");
                    //ofLog() << keyValue[0] << ":" << keyValue[1];
                    //return;
                    if ( keyValue[0] == "background-color" ){
                        if ( keyValue[1].find("#") != string::npos ){
                            string col = ofJoinString(ofSplitString(keyValue[1],"#"), "");
                            float floatColor = stoul(col, nullptr, 16);
                            baseStyle.backgroundColor.setHex(floatColor);
                        }
                    } else if ( keyValue[0] == "width" ){
                        baseStyle.width = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "height" ){
                        baseStyle.height = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "x" ){
                        baseStyle.x = ofToInt(keyValue[1]);
                    } else if ( keyValue[0] == "y" ){
                        baseStyle.y = ofToInt(keyValue[1]);
                    }
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
        
        
        
        ofEvent<suiStyleSheetArgs> onReload;
        ofBuffer dataBuffer;
        string data;
        time_t fileTime = 0;
        float nextUpdateTime = 0.0;
        
        bool liveReloading = false;
        float updateInterval = 1.0;
        map<string, vector<string>> blocks;
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
        
        
        vector<string> SelectBlock(string selector){
            vector<string> vecStr;
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                if ( it->first == selector ){
                    return it->second;
                }
            }
            
            return vecStr;
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
    
    class Element : public AnimatableParams, public StyleRenderParams {
    public:
        ~Element(){
            //stylesheet = 0;
            //delete stylesheet;
        };
        
        Element(StyleSelector& styleSelector, string id = ""):styleSelector(styleSelector){
            this->id = id;
            ofAddListener(styleSelector.onUpdate, this, &Element::UpdateStyle);
            
            UpdateStyle();
        }
        
        string id;
        string selector;
        
        Actions actions;
        CustomParams customParams;
        StyleSelector& styleSelector;
        
        //
        
        /*Element& Style(string selector, StyleSheet& stylesheet, string id = ""){
            this->selector = selector;
            //ParseStyle(stylesheet.SelectBlock(selector));
            //ParseActions(stateStyles[state]);
            //ofLog() << styles[state].width << "x" << styles[state].height;
            
            
            return *this;
        }*/
        
        Element& SetPosition(float x, float y, float z = 0.0){
            position = glm::vec3(x, y, z);
            return *this;
        }
        
        Element& SetSize(float w, float h){
            size = glm::vec2(w,h);
            return *this;
        }
        
        void UpdateStyle(){
            compiledStyle = styleSelector.Compile(state, inlineStyle);
        }
        
        void Draw(float x = 0.0, float y = 0.0){
            //compiledStyle.DebugLog();
            
            ofSetColor( compiledStyle.backgroundColor );
            ofPushView();
            ofTranslate(position.x, position.y);
            ofDrawRectangle(0, 0, compiledStyle.width, compiledStyle.height);
            ofPopView();
        }
        
    private:
        void UpdateStyle(suiStyleSelectorArgs& args){
            UpdateStyle();
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
            actions.Parse(style.GetRawActions());
        }
        
    };
    
    
    class Window {
    public:
        ~Window(){
            stylesheet = 0;
            stylesheet = NULL;
        };
        Window(){};
        
        //
        
        vector<Element*> elements;
        StyleSheet* stylesheet = NULL;
        map<string, vector<string>> blocks;
        
        bool ElementExists(string id){
            for (auto element : elements ){
                if ( element->id == id ) return true;
            }
            return false;
        }
        
        Element& GetElementById(string id){
            for (auto element : elements ){
                if ( element->id == id ) return *element;
            }
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
            
            ofAddListener(stylesheet->onReload, this, &Window::ReloadElements);
            
            MakeElements();
        };
        
        void MakeElements(){
            blocks = GetBlocks(stylesheet->SelectBlock("[elements]"));
            
            for (auto it=blocks.begin(); it!=blocks.end(); ++it){
                string selector = CleanSelector(it->first);
                vector<string> nameSelector = ofSplitString(selector, ":");
                
                vector<string> styles = GetStyles(it->second);
                
                
                
                if ( ElementExists(nameSelector[0]) ) {
                    Element& el = GetElementById(nameSelector[0]);
                    for (auto style : styles){
                        vector<string> keyValue = ofSplitString(style, ":");
                        if ( keyValue[0] == "x" ) el.position.x = ofToInt(keyValue[1]);
                        else if ( keyValue[0] == "y" ) el.position.y = ofToInt(keyValue[1]);
                    }
                    continue;
                }
                if ( !stylesheet->HasSelector(nameSelector[1]) ) continue;
                
                Element& el = AddElement(nameSelector[1], nameSelector[0]);
                for (auto style : styles){
                    vector<string> keyValue = ofSplitString(style, ":");
                    if ( keyValue[0] == "x" ) el.position.x = ofToInt(keyValue[1]);
                    else if ( keyValue[0] == "y" ) el.position.y = ofToInt(keyValue[1]);
                }
            }
        }
        
        void Draw(){
            for (auto element : elements ){
                element->Draw();
            }
        };
        
        void ReloadElements(suiStyleSheetArgs& args){
            MakeElements();
        }
        
        
        
        
    };
    
}

#endif
