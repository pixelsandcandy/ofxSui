#include "ofxSui.h"

namespace SUI {
    Settings settings;
    map<string, ofImage*> images;
    vector<Tween*> tweens;
    vector<Tween*> tweensToDestroy;
    
    void LiveReload(bool reload){
        settings.liveReload = reload;
    };
    
    struct suiStyleSelectorArgs {
        StyleSelector& styleSelector;
        suiStyleSelectorArgs(StyleSelector &styleSelector):styleSelector(styleSelector){}
    };
    
    void StyleSelector::Bind(StyleSheet& stylesheet){
        this->stylesheet = &stylesheet;
        ofAddListener(this->stylesheet->onReload, this, &StyleSelector::ReloadStyle);
    }
    
    void StyleSelector::ReloadStyle(suiStyleSheetArgs& args){
        suiStyleSelectorArgs sArgs(*this);
        ofNotifyEvent(onUpdate, sArgs, this);
    }
    
    struct suiTweenArgs {
        Element& element;
        Tween& tween;
        Event eventType;
        suiTweenArgs(Tween &tween, Element &element, Event eventType):tween(tween),element(element),eventType(eventType){}
    };
    
    void Canvas::Update(){
        float currTime = ofGetElapsedTimeMillis();
        
        //ofLog() << "# tweens: " << tweens.size();
        
        int max = tweens.size();
        int i = 0;
        for (vector<Tween*>::iterator it = tweens.begin(); it != tweens.end(); it++){
            //if ( i >= 0 && i < max && (*it) != NULL && (*it) != nullptr && (*it)->el != NULL ) (*it)->Update( currTime );
            //else return;
            if ( i >= 0 && i < max ) {
                if ( (*it) != NULL && (*it)->active && !isnan((*it)->duration) && (*it)->duration != 0.0 ) (*it)->Update( currTime );
            } else {
                return;
            }
            //(*it)->Update( currTime );
            i++;
        }
        
        for (vector<Tween*>::iterator it = tweensToDestroy.begin(); it !=  tweensToDestroy.end(); it++){
            DestroyTween( (*it) );
        }
        
        tweensToDestroy.clear();
    };
    
    //
    
    void StyleSheet::UpdateHandler(){
        //std::filesystem::last_write_time(ofToDataPath(it.second));
        
        if ( ofGetElapsedTimef() < nextUpdateTime ){
            nextUpdateTime = ofGetElapsedTimef() + updateInterval;
            time_t t = std::filesystem::last_write_time(ofToDataPath(this->filepath));
            if ( t > fileTime ){
                fileTime = t;
                Load(filepath);
                suiStyleSheetArgs args(*this);
                
                ofNotifyEvent(onReload, args, this);
            };
        }
        
    }
    
    void Element::StopTween(){
        if ( tween != NULL ) {
            tween->Stop();
            tween = NULL;
        }
    }
    
    void Element::StoreTween(Tween* tween){
        this->tween = tween;
    }
    
    template <typename ArgumentsType, class ListenerClass>
    Tween* Element::Animate( float timeSeconds, string params, ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&) ){
        return SUI::Animate( this, timeSeconds, params, listener, listenerMethod );
        
    }
    
    Tween* Element::Animate( float timeSeconds, string params ){
        return SUI::Animate( this, timeSeconds, params );
    }
    
    Tween* Animate( Element* el, float timeSeconds, string params ){
        Tween *t = new Tween();
        t->Start( el, timeSeconds, params );
        
        tweens.push_back( t );
        return t;
    }
    
    //
    
    void Tween::Start( Element* el, float timeSeconds, string params ){
        firstStep = true;
        active = true;
        this->el = el;
        el->StopTween();
        el->StoreTween(this);
        duration = timeSeconds*1000;
        
        //el->UpdateStyle();
        
        string s(params);
        ofStringReplace(s, "{", " ");
        ofStringReplace(s, "}", " ");
        ofStringReplace(s, " ", "");
        
        cmd = s;
        
        vector<string> split = ofSplitString(s, "," );
        for ( vector<string>::iterator it = split.begin(); it != split.end(); it++){
            //ofLog() << (*it);
            
            vector<string> propVal = ofSplitString( (*it), ":" );
            if ( propVal.size() == 2 ){
                //ofLog() << propVal[0] << " => " << propVal[1];
                
                if ( propVal[0] == "rotation" || propVal[0] == "x" || propVal[0] == "y" || propVal[0] == "width" || propVal[0] == "height" || propVal[0] == "opacity" || propVal[0] == "scale" ){
                    if ( propVal[1].find("+=") != -1 ) {
                        if ( propVal[0] == "x" ) propVal[1] = ofToString(el->x + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "y" ) propVal[1] = ofToString(el->y + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "opacity" ) propVal[1] = ofToString(el->opacity + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "width" ) propVal[1] = ofToString(el->width + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "height" ) propVal[1] = ofToString(el->height + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "scale" ) propVal[1] = ofToString(el->scale + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                        else if ( propVal[0] == "rotation" ) propVal[1] = ofToString(el->rotation + ofToFloat( ofSplitString( propVal[1], "+=" )[1] ));
                    } else if ( propVal[1].find("-=") != -1 ){
                        if ( propVal[0] == "x" ) propVal[1] = ofToString(el->x - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "y" ) propVal[1] = ofToString(el->y - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "opacity" ) propVal[1] = ofToString(el->opacity - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "width" ) propVal[1] = ofToString(el->width - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "height" ) propVal[1] = ofToString(el->height - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "scale" ) propVal[1] = ofToString(el->scale - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                        else if ( propVal[0] == "rotation" ) propVal[1] = ofToString(el->rotation - ofToFloat( ofSplitString( propVal[1], "-=" )[1] ));
                    } else if ( propVal[1].find("*=") != -1 ){
                        if ( propVal[0] == "x" ) propVal[1] = ofToString(el->x * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "y" ) propVal[1] = ofToString(el->y * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "opacity" ) propVal[1] = ofToString(el->opacity * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "width" ) propVal[1] = ofToString(el->width * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "height" ) propVal[1] = ofToString(el->height * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "scale" ) propVal[1] = ofToString(el->scale * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                        else if ( propVal[0] == "rotation" ) propVal[1] = ofToString(el->rotation * ofToFloat( ofSplitString( propVal[1], "*=" )[1] ));
                    } else if ( propVal[1].find("%") != -1 && el->GetParent() != NULL ){
                        if ( propVal[0] == "width" ) propVal[1] = ofToString(el->GetParent()->width * ofToFloat( ofSplitString( propVal[1], "*=" )[0] )*.01 );
                        else if ( propVal[0] == "height" ) propVal[1] = ofToString(el->GetParent()->height * ofToFloat( ofSplitString( propVal[1], "*=" )[0] )*.01 );
                    }
                    //ofLog() << propVal[0] << "=" << propVal[1];
                    StoreValue( propVal[0], propVal[1] );
                } else if ( propVal[0] == "delay" ){
                    delay = ofToFloat( propVal[1] );
                } else if ( propVal[0] == "easing" || propVal[0] == "ease" ){
                    string e = ofToLower(propVal[1]);
                    
                    if ( e == "back.easein" ) ease = ofxeasing::back::easeIn;
                    else if ( e == "back.easeout" ) ease = ofxeasing::back::easeOut;
                    else if ( e == "back.easeinout" ) ease = ofxeasing::back::easeInOut;
                    else if ( e == "bounce.easein" ) ease = ofxeasing::bounce::easeIn;
                    else if ( e == "bounce.easeout" ) ease = ofxeasing::bounce::easeOut;
                    else if ( e == "bounce.easeinout" ) ease = ofxeasing::bounce::easeInOut;
                    else if ( e == "circ.easein" ) ease = ofxeasing::circ::easeIn;
                    else if ( e == "circ.easeout" ) ease = ofxeasing::circ::easeOut;
                    else if ( e == "circ.easeinout" ) ease = ofxeasing::circ::easeInOut;
                    else if ( e == "cubic.easein" ) ease = ofxeasing::cubic::easeIn;
                    else if ( e == "cubic.easeout" ) ease = ofxeasing::cubic::easeOut;
                    else if ( e == "cubic.easeinout" ) ease = ofxeasing::cubic::easeInOut;
                    else if ( e == "elastic.easein" ) ease = ofxeasing::elastic::easeIn;
                    else if ( e == "elastic.easeout" ) ease = ofxeasing::elastic::easeOut;
                    else if ( e == "elastic.easeinout" ) ease = ofxeasing::elastic::easeInOut;
                    else if ( e == "exp.easein" ) ease = ofxeasing::exp::easeIn;
                    else if ( e == "exp.easeout" ) ease = ofxeasing::exp::easeOut;
                    else if ( e == "exp.easeinout" ) ease = ofxeasing::exp::easeInOut;
                    else if ( e == "linear.easein" ) ease = ofxeasing::linear::easeIn;
                    else if ( e == "linear.easeout" ) ease = ofxeasing::linear::easeOut;
                    else if ( e == "linear.easeinout" ) ease = ofxeasing::linear::easeInOut;
                    else if ( e == "linear.easenone" ) ease = ofxeasing::linear::easeNone;
                    else if ( e == "quad.easein" ) ease = ofxeasing::quad::easeIn;
                    else if ( e == "quad.easeout" ) ease = ofxeasing::quad::easeOut;
                    else if ( e == "quad.easeinout" ) ease = ofxeasing::quad::easeInOut;
                    
                }
                else if (propVal[0] == "id") {
                    id = propVal[1];
                }
                
            }
        }
        
        
        startTime = ofGetElapsedTimeMillis() + (delay*1000);
        endTime = startTime + duration;
    }
    
    void Tween::StoreStartValues(){
        for ( vector<string>::iterator it = valueNames.begin(); it != valueNames.end(); it++ ){
            if ( (*it) == "x" ){
                startValues[ (*it) ] = el->x;
            } else if ( (*it) == "y" ){
                startValues[ (*it) ] = el->y;
                //ofLog() << "sy:" << startValues[ (*it) ];
            } else if ( (*it) == "width" ){
                //ofLog() << "width:" << el->GetWidth();
                startValues[ (*it) ] = el->width;
            } else if ( (*it) == "height" ){
                //ofLog() << "height:" << el->GetHeight();
                startValues[ (*it) ] = el->height;
            } else if ( (*it) == "rotation" ){
                startValues[ (*it) ] = el->rotation;
            } else if ( (*it) == "opacity" ){
                startValues[ (*it) ] = el->opacity;
            } else if ( (*it) == "scale" ){
                //ofLog() << "scale:" << el->GetScale();
                startValues[ (*it) ] = el->scale;
            }
        }
        
        //ofLog() << "startValues:  x=" << startValues["x"] << "  y:" << startValues["y"];
    }
    
    void Tween::StoreValue( string param, string val ){
        endValues[ param ] = ofToFloat( val );
        valueNames.push_back( param );
    }
    
    void Tween::StoreValue( string param, float val ){
        endValues[ param ] = val;
        valueNames.push_back( param );
    }
    
    void Tween::Stop(){
        active = false;
    }
    
    
    void Tween::Update(float currTime){
        if ( !active || el == NULL ) return;
        
        if ( currTime >= startTime && currTime < endTime ){
            
            
            if ( firstStep ){
                firstStep = false;
                el->Update();
                StoreStartValues();
                
                suiTweenArgs args(*this, (*el), SUI_EVENT_ANIMATE_START);
                ofNotifyEvent( onStart, args, this );
                
                //ofLog() << cmd;
            }
            
            perc = (currTime - startTime) / duration;
            UpdateValues();
            
            suiTweenArgs args(*this, (*el), SUI_EVENT_ANIMATE_STEP);
            ofNotifyEvent( onStep, args, this );
        } else if ( currTime >= endTime ) {
            active = false;
            
            perc = 1;
            UpdateValues();
            
            suiTweenArgs args(*this, (*el), SUI_EVENT_ANIMATE_STEP);
            ofNotifyEvent( onStep, args, this );
            
            
            suiTweenArgs args2(*this, (*el), SUI_EVENT_ANIMATE_COMPLETE);
            ofNotifyEvent( onComplete, args2, this );
            
            ShouldDestroyTween(this);
        }
    }
    
    void Tween::UpdateValues(){
        if ( valueNames.size() == 0 ) return;
        for ( vector<string>::iterator it = valueNames.begin(); it != valueNames.end(); it++ ){
            if ( (*it) == "x" ){
                //x = el->GetPosition().x;
                
                x = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                //ofLog() << "(" << x << ") " << startValues[(*it)] << "->" << endValues[(*it)];
                el->x = x;
            } else if ( (*it) == "y" ){
                y = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->y = y;
            } else if ( (*it) == "width" ){
                width = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->width = width;
            } else if ( (*it) == "height" ){
                height = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->height = height;
            } else if ( (*it) == "rotation" ){
                rotation = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->rotation = rotation;
            } else if ( (*it) == "opacity" ){
                opacity = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                //if ( el->GetName() == "#ShaveOver" ) ofLog() << opacity;
                
                el->opacity = opacity;
                
            } else if ( (*it) == "scale" ){
                scale = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->scale = scale;
            }
        }
        el->Update();
    }
}
