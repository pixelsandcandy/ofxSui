#include "ofxSui.h"

namespace SUI {
    Settings settings;
    
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
}
