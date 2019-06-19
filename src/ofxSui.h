#pragma once

#ifndef __ofxSui__
#define __ofxSui__

#include <algorithm>
#include <string>
#include <iostream>
#include "ofxEasing.h"
#include "ofEvents.h"

using namespace std;

namespace SUI {
    
    struct Settings {
        bool liveReload = false;
    };
    
    extern Settings settings;
    
    void LiveReload(bool reload = true);
    
    
    
    class StyleSheet {
    public:
        ~StyleSheet(){};
        StyleSheet(){};
        StyleSheet(string filepath){
            Load(filepath);
        };
        
        //
        
        void Load(string filepath){
            this->filepath = filepath;
        }
        
        string filepath;
    };
    
    class Element {
    public:
        ~Element(){};
        Element(){};
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
        
        //
        
        
        void Update(ofEventArgs & args){}
        
        void Update(){};
        
        void Render(){};
        
        void Draw(){};
        
        
        
        
    };
    
}

#endif
