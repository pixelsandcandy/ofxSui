#include "ofxSui.h"

namespace SUI {
    Settings settings;
    
    void LiveReload(bool reload){
        settings.liveReload = reload;
    };
}
