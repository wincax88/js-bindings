//
//  jsb_agora_manual.hpp
//  cocos2d_js_bindings
//
//  Created by michael on 2017/10/30.
//

#ifndef jsb_agora_manual_hpp
#define jsb_agora_manual_hpp

#include <stdio.h>
#include <functional>

namespace se {
    class Object;
    class Value;
}

namespace le {
    class agora
    {
    public:
        agora();
        
        void foo();
        
        // Get the version of Agora SDK.
        static const char * getSdkVersion();
        // Get the version of Media Engine.
        static const char * getMediaEngineVersion();

        void setCallback(const std::function<void(int)>& cb) {
            _cb = cb;
            if (_cb != nullptr)
            {
                printf("setCallback(cb)\n");
            }
            else
            {
                printf("setCallback(nullptr)\n");
            }
        }
        
        int xxx;
    private:
        std::function<void(int)> _cb;
    };
} // namespace le {

bool register_all_agora(se::Object* global);

#endif /* jsb_agora_manual_hpp */
