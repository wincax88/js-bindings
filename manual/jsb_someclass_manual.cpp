//
//  jsb_someclass_manual.cpp
//  cocos2d_js_bindings
//
//  Created by michael on 2017/10/30.
//

#include "jsb_someclass_manual.hpp"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/scripting/js-bindings/manual/jsb_helper.hpp"

#include "cocos2d.h"

using namespace cocos2d;

static se::Object* __jsb_ns_SomeClass_proto = nullptr;
static se::Class* __jsb_ns_SomeClass_class = nullptr;

void ns::SomeClass::foo() {
    printf("SomeClass::foo\n");
    
    Director::getInstance()->getScheduler()->schedule([this](float dt){
        static int counter = 0;
        ++counter;
        if (_cb != nullptr)
            _cb(counter);
    }, this, 1.0f, CC_REPEAT_FOREVER, 0.0f, false, "iamkey");

}
static bool js_SomeClass_finalize(se::State& s)
{
    return true;
}
SE_BIND_FINALIZE_FUNC(js_SomeClass_finalize)

static bool js_SomeClass_constructor(se::State& s)
{
    ns::SomeClass* cobj = new ns::SomeClass();
    s.thisObject()->setPrivateData(cobj);
    return true;
}
SE_BIND_CTOR(js_SomeClass_constructor, __jsb_ns_SomeClass_class, js_SomeClass_finalize)

static bool js_SomeClass_foo(se::State& s)
{
    ns::SomeClass* cobj = (ns::SomeClass*)s.nativeThisObject();
    cobj->foo();
    return true;
}
SE_BIND_FUNC(js_SomeClass_foo)

static bool js_SomeClass_get_xxx(se::State& s)
{
    ns::SomeClass* cobj = (ns::SomeClass*)s.nativeThisObject();
    s.rval().setInt32(cobj->xxx);
    return true;
}
SE_BIND_PROP_GET(js_SomeClass_get_xxx)

static bool js_SomeClass_set_xxx(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc > 0)
    {
        ns::SomeClass* cobj = (ns::SomeClass*)s.nativeThisObject();
        cobj->xxx = args[0].toInt32();
        return true;
    }
    
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
SE_BIND_PROP_SET(js_SomeClass_set_xxx)

static bool js_SomeClass_static_func(se::State& s)
{
    ns::SomeClass::static_func();
    return true;
}
SE_BIND_FUNC(js_SomeClass_static_func)

static bool js_SomeClass_setCallback(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc >= 1)
    {
        ns::SomeClass* cobj = (ns::SomeClass*)s.nativeThisObject();
        
        se::Value jsFunc = args[0];
        se::Value jsTarget = argc > 1 ? args[1] : se::Value::Undefined;
        
        if (jsFunc.isNullOrUndefined())
        {
            cobj->setCallback(nullptr);
        }
        else
        {
            assert(jsFunc.isObject() && jsFunc.toObject()->isFunction());
            
            // 如果当前SomeClass是可以被new出来的类，我们 使用se::Object::attachObject把jsFunc和jsTarget关联到当前对象中
            s.thisObject()->attachObject(jsFunc.toObject());
            s.thisObject()->attachObject(jsTarget.toObject());
            
            // 如果当前SomeClass类是一个单例类，或者永远只有一个实例的类，我们不能用se::Object::attachObject去关联
            // 必须使用se::Object::root，开发者无需关系unroot，unroot的操作会随着lambda的销毁触发jsFunc的析构，在se::Object的析构函数中进行unroot操作。
            // js_cocos2dx_EventDispatcher_addCustomEventListener的绑定代码就是使用此方式，因为EventDispatcher始终只有一个实例，
            // 如果使用s.thisObject->attachObject(jsFunc.toObject);会导致对应的func和target永远无法被释放，引发内存泄露。
            
            // jsFunc.toObject()->root();
            // jsTarget.toObject()->root();
            cobj->setCallback([jsFunc, jsTarget](int counter){
                
                // CPP回调函数中要传递数据给JS或者调用JS函数，在回调函数开始需要添加如下两行代码。
                se::ScriptEngine::getInstance()->clearException();
                se::AutoHandleScope hs;
                
                se::ValueArray args;
                args.push_back(se::Value(counter));
                
                se::Object* target = jsTarget.isObject() ? jsTarget.toObject() : nullptr;
                jsFunc.toObject()->call(args, target);
            });
        }
        
        return true;
    }
    
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
SE_BIND_FUNC(js_SomeClass_setCallback)

bool register_all_someclass(se::Object* global)
{
    // 保证namespace对象存在
    se::Value nsVal;
    if (!global->getProperty("ns", &nsVal))
    {
        // 不存在则创建一个JS对象，相当于 var ns = {};
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        
        // 将ns对象挂载到global对象中，名称为ns
        global->setProperty("ns", nsVal);
    }
    se::Object* ns = nsVal.toObject();
    
    // 创建一个Class对象，开发者无需考虑Class对象的释放，其交由ScriptEngine内部自动处理
    auto cls = se::Class::create("SomeClass", ns, nullptr, _SE(js_SomeClass_constructor)); // 如果无构造函数，最后一个参数可传入nullptr，则这个类在JS中无法被new SomeClass()出来
    
    // 为这个Class对象定义成员函数、属性、静态函数、析构函数
    cls->defineFunction("foo", _SE(js_SomeClass_foo));
    cls->defineFunction("setCallback", _SE(js_SomeClass_setCallback));
    cls->defineProperty("xxx", _SE(js_SomeClass_get_xxx), _SE(js_SomeClass_set_xxx));
    
    cls->defineFinalizeFunction(_SE(js_SomeClass_finalize));
    
    // 注册类型到JS VirtualMachine的操作
    cls->install();
    
    // JSBClassType为Cocos引擎绑定层封装的类型注册的辅助函数，此函数不属于ScriptEngine这层
    JSBClassType::registerClass<ns::SomeClass>(cls);
    
    // 保存注册的结果，便于其他地方使用，比如类继承
    __jsb_ns_SomeClass_proto = cls->getProto();
    __jsb_ns_SomeClass_class = cls;
    
    // 为每个此Class实例化出来的对象附加一个属性
    __jsb_ns_SomeClass_proto->setProperty("yyy", se::Value("helloyyy"));
    
    // 注册静态成员变量和静态成员函数
    se::Value ctorVal;
    if (ns->getProperty("SomeClass", &ctorVal) && ctorVal.isObject())
    {
        ctorVal.toObject()->setProperty("static_val", se::Value(200));
        ctorVal.toObject()->defineFunction("static_func", _SE(js_SomeClass_static_func));
    }
    
    // 清空异常
    se::ScriptEngine::getInstance()->clearException();
    return true;
}
