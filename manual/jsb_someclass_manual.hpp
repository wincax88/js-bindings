//
//  jsb_someclass_manual.hpp
//  cocos2d_js_bindings
//
//  Created by michael on 2017/10/30.
//

#ifndef jsb_someclass_manual_hpp
#define jsb_someclass_manual_hpp

#include <stdio.h>
#include <functional>

namespace se {
    class Object;
    class Value;
}

namespace ns {
    class SomeClass
    {
    public:
        SomeClass()
        : xxx(0)
        {}
        
        void foo();
        
        static void static_func() {
            printf("SomeClass::static_func\n");
        }
        
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
} // namespace ns {

bool register_all_someclass(se::Object* global);

#endif /* jsb_someclass_manual_hpp */
