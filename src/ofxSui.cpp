#include "ofxSui.h"

namespace SUI {
    Settings settings;
    map<string, ofImage*> images;
    map<string, ofSoundPlayer*> sounds;
    map<string, ofTrueTypeFont*> fonts;
    vector<Tween*> tweens;
    vector<Tween*> tweensToDestroy;
    bool tweenUpdate = false;
    
    
    void liveReload(bool reload){
        //ofLog() << "============================";
        ofLog() << "liveReload -----------------";
        //ofLog() << "============================";
        settings.liveReload = reload;
    };
    
    struct suiStyleSelectorArgs {
        StyleSelector& styleSelector;
        suiStyleSelectorArgs(StyleSelector &styleSelector):styleSelector(styleSelector){}
    };
    
    void StyleSelector::bind(StyleSheet& stylesheet){
        this->stylesheet = &stylesheet;
        ofAddListener(this->stylesheet->onReload, this, &StyleSelector::reloadStyle);
    }
    
    void StyleSelector::reloadStyle(suiStyleSheetArgs& args){
        suiStyleSelectorArgs sArgs(*this);
        ofNotifyEvent(onupdate, sArgs, this);
    }
    
    void playSound(Canvas& canvas, string id){
        if ( canvas.sounds.count(id) != 0 ) playSound(canvas.sounds[id]);
    }
    
    ofSoundPlayer* getSound(Canvas& canvas, string id){
        return getSound(canvas.sounds[id]);
    }
    
    
    
    void update(){
        //ofLog() << "# tweens: " << tweens.size();
        
        //ofLog() << "update!!!";
        
        if (!tweenUpdate){
            tweenUpdate = true;
            ofAddListener( ofEvents().update, update, OF_EVENT_ORDER_BEFORE_APP );
        }
        
        //
        
        float currTime = ofGetElapsedTimeMillis();
        
        int max = tweens.size();
        int i = 0;
        for (vector<Tween*>::iterator it = tweens.begin(); it != tweens.end(); it++){
            //if ( i >= 0 && i < max && (*it) != NULL && (*it) != nullptr && (*it)->el != NULL ) (*it)->update( currTime );
            //else return;
            if ( i >= 0 && i < max ) {
                if ( (*it) == NULL ) continue;
                if ( (*it)->shouldDestroy == true ) continue;
                if ( (*it)->active == true && (*it)->duration != 0.0 ) (*it)->update( currTime );
            } else {
                return;
            }
            //(*it)->update( currTime );
            i++;
        }
        
    }
    
    void Canvas::update(){
        /*if (!ready){
            ready = true;
            if (shouldEmitLoadedEvent) {
                shouldEmitLoadedEvent = false;
                emitloadedEvent();
                ofLog() << "YAYYY";
            }
        }*/
    };
    
    //
    
    void StyleSheet::updateHandler(){
        //std::filesystem::last_write_time(ofToDataPath(it.second));
        //ofLog() << ofgetElapsedTimef() << " <?" << nextupdateTime;
        if ( ofGetElapsedTimef() > nextupdateTime ){
            //ofLog() << "check file... " << ofRandomf();
            nextupdateTime = ofGetElapsedTimef() + updateInterval;
            time_t t = std::filesystem::last_write_time(ofToDataPath(this->filepath));
            //ofLog() << t << "  >?  " << fileTime;
            if ( t > fileTime ){
                //ofLog() << "RELOAD FILE--------" << ofRandomf();
                
                fileTime = t;
                load(filepath);
                suiStyleSheetArgs args(*this);
                
                ofNotifyEvent(onReload, args, this);
            };
        }
        
    }
    
    void Element::stopTween(){
        if ( this->tween != NULL ) {
            this->tween->stop();
            this->tween = NULL;
        }
    }
    
    void Element::storeTween(Tween* tween){
        this->tween = tween;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    bool attachedToElement = false;
    
    //
    
    void Tween::start( Element* el, float timeSeconds, string params, bool attachToElement ){
        if ( el == NULL ) {
            shouldDestroyTween(this);
            return;
        }
        
        firstStep = true;
        active = true;
        shouldDestroy = false;
        this->el = el;
        if ( attachToElement ) {
            attachedToElement = true;
            if ( el->tween != NULL ){
                shouldDestroyTween(el->tween);
            }
            el->storeTween(this);
        }
        duration = timeSeconds*1000;
        
        //el->updateStyle();
        
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
                    } else if ( propVal[1].find("%") != -1 && el->getParent() != NULL ){
                        if ( propVal[0] == "width" ) propVal[1] = ofToString(el->getParent()->width * ofToFloat( ofSplitString( propVal[1], "*=" )[0] )*.01 );
                        else if ( propVal[0] == "height" ) propVal[1] = ofToString(el->getParent()->height * ofToFloat( ofSplitString( propVal[1], "*=" )[0] )*.01 );
                    }
                    //ofLog() << propVal[0] << "=" << propVal[1];
                    storeValue( propVal[0], propVal[1] );
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
    
    void Tween::storeStartValues(){
        for ( vector<string>::iterator it = valueNames.begin(); it != valueNames.end(); it++ ){
            if ( (*it) == "x" ){
                startValues[ (*it) ] = el->x;
            } else if ( (*it) == "y" ){
                startValues[ (*it) ] = el->y;
                //ofLog() << "sy:" << startValues[ (*it) ];
            } else if ( (*it) == "width" ){
                //ofLog() << "width:" << el->getWidth();
                startValues[ (*it) ] = el->width;
            } else if ( (*it) == "height" ){
                //ofLog() << "height:" << el->getHeight();
                startValues[ (*it) ] = el->height;
            } else if ( (*it) == "rotation" ){
                startValues[ (*it) ] = el->rotation;
            } else if ( (*it) == "opacity" ){
                startValues[ (*it) ] = el->opacity;
            } else if ( (*it) == "scale" ){
                //ofLog() << "scale:" << el->getScale();
                startValues[ (*it) ] = el->scale;
            }
        }
        
        //ofLog() << "startValues:  x=" << startValues["x"] << "  y:" << startValues["y"];
    }
    
    void Tween::storeValue( string param, string val ){
        endValues[ param ] = ofToFloat( val );
        valueNames.push_back( param );
    }
    
    void Tween::storeValue( string param, float val ){
        endValues[ param ] = val;
        valueNames.push_back( param );
    }
    
    void Tween::stop(){
        active = false;
    }
    
    
    void Tween::update(float currTime){
        if ( active == false ) return;
        if ( el == NULL ) return;
        
        if ( currTime >= startTime && currTime < endTime ){
            
            
            if ( firstStep ){
                firstStep = false;
                el->update();
                storeStartValues();
                
                //suiTweenArgs args(*this, (*el), SUI_EVENT_ANIMATE_START);
                suiTweenArgs args;
                args.tween = this;
                args.element = el;
                args.eventType = SUI_EVENT_ANIMATE_START;
                args.id = id;
                ofNotifyEvent( onstart, args, this );
                
                //ofLog() << cmd;
            }
            
            perc = (currTime - startTime) / duration;
            updateValues();
            
            suiTweenArgs args;
            args.tween = this;
            args.element = el;
            args.eventType = SUI_EVENT_ANIMATE_STEP;
            args.id = id;
            ofNotifyEvent( onStep, args, this );
        } else if ( currTime >= endTime ) {
            active = false;
            
            perc = 1;
            updateValues();
            
            suiTweenArgs args;
            args.tween = this;
            args.element = el;
            args.eventType = SUI_EVENT_ANIMATE_STEP;
            args.id = id;
            ofNotifyEvent( onStep, args, this );
            
            
            suiTweenArgs args2;
            args2.tween = this;
            args2.element = el;
            args2.eventType = SUI_EVENT_ANIMATE_COMPLETE;
            args2.id = id;
            ofNotifyEvent( onComplete, args2, this );
            
            if ( attachedToElement ) shouldDestroyTween(this);
        }
    }
    
    void Tween::updateValues(){
        if ( valueNames.size() == 0 ) return;
        for ( vector<string>::iterator it = valueNames.begin(); it != valueNames.end(); it++ ){
            if ( (*it) == "x" ){
                //x = el->getPosition().x;
                
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
                //if ( el->getName() == "#ShaveOver" ) ofLog() << opacity;
                
                el->opacity = opacity;
                
            } else if ( (*it) == "scale" ){
                scale = ofxeasing::map(perc, 0.0, 1.0, startValues[(*it)], endValues[(*it)], ease);
                el->scale = scale;
            }
        }
        el->update();
    }
    
    
    void Action::run(Element& el){
        ofLog() << "Action::run()";
        for (auto it=subactions.begin(); it!=subactions.end(); ++it){
            ofLog() << "[" << it->first << "]";
            if (it->first == "animate"){
                
                int index = it->second.find(",");
                float duration = ofToFloat(it->second.substr(0, index ));
                string params = it->second.substr(index+1);
                params = ofJoinString( ofSplitString(params, ";"), "" );
                
                ofLog() << "animate > " << duration << " > " << params;
                el.animate(duration, params);
            } else if (it->first == "commands"){
                vector<string> commands = ofSplitString(cleanArrayBrackets(it->second), ",");
                for (auto& command : commands){
                    ofLog() << "command > " << command;
                    if ( command.find("actions.") != string::npos ){
                        if ( command.find("(") == string::npos && command.find(")") == string::npos ){
                            el.styleSelector->runAction(command.substr(command.find(".")+1), el);
                        } else if (ofToString(command[0]) != "(" && command.find("(") != string::npos && command.find(")") != string::npos) {
                            string selectorId = command.substr( command.find("(")+1 );
                            selectorId = ofJoinString( ofSplitString(selectorId, ")"), "" );
                            
                            string actionId = ofSplitString(ofSplitString(command, ".")[1], "(")[0];
                            
                            el.styleSelector->runAction(actionId, *el.canvas->getElementById(selectorId));
                        }
                    } else if ( command.find("trigger.") != string::npos ){
                        el.canvas->emitTriggerEvent( command.substr( command.find(".")+1 ) );
                    }
                }
            }
        }
    }
}
