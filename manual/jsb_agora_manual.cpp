//
//  jsb_agora_manual.cpp
//  cocos2d_js_bindings
//
//  Created by michael on 2017/10/30.
//

#include "jsb_agora_manual.hpp"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/scripting/js-bindings/manual/jsb_helper.hpp"

#include "cocos2d.h"
#import <AgoraRtcEngineKit/AgoraRtcEngineKit.h>

using namespace cocos2d;
using namespace le;

static se::Object* __jsb_ns_agora_proto = nullptr;
static se::Class* __jsb_ns_agora_class = nullptr;

agora::agora()
:xxx (0)
{
    
}

void agora::foo() {
    printf("agora::foo\n");
    
    Director::getInstance()->getScheduler()->schedule([this](float dt){
        static int counter = 0;
        ++counter;
        if (_cb != nullptr)
            _cb(counter);
    }, this, 1.0f, CC_REPEAT_FOREVER, 0.0f, false, "iamkey");
    
}


static bool js_agora_finalize(se::State& s)
{
    return true;
}
SE_BIND_FINALIZE_FUNC(js_agora_finalize)

static bool js_agora_constructor(se::State& s)
{
    le::agora* cobj = new le::agora();
    s.thisObject()->setPrivateData(cobj);
    return true;
}
SE_BIND_CTOR(js_agora_constructor, __jsb_ns_agora_class, js_agora_finalize)

static bool js_agora_foo(se::State& s)
{
    le::agora* cobj = (le::agora*)s.nativeThisObject();
    cobj->foo();
    return true;
}
SE_BIND_FUNC(js_agora_foo)

static bool js_agora_get_xxx(se::State& s)
{
    le::agora* cobj = (le::agora*)s.nativeThisObject();
    s.rval().setInt32(cobj->xxx);
    return true;
}
SE_BIND_PROP_GET(js_agora_get_xxx)

static bool js_agora_set_xxx(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc > 0)
    {
        le::agora* cobj = (le::agora*)s.nativeThisObject();
        cobj->xxx = args[0].toInt32();
        return true;
    }
    
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
SE_BIND_PROP_SET(js_agora_set_xxx)

const char * agora::getSdkVersion() {
    NSString *version = [AgoraRtcEngineKit getSdkVersion];
    return version.UTF8String;
}

const char * agora::getMediaEngineVersion() {
    NSString *version = [AgoraRtcEngineKit getMediaEngineVersion];
    CCLOG("Media Engine Version : %s", version.UTF8String);
    return version.UTF8String;
}

static bool js_agora_get_sdk_version(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        const char * result = le::agora::getSdkVersion();
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_agora_get_sdk_version : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_agora_get_sdk_version)

static bool js_agora_get_media_engine_version(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        const char * result = le::agora::getMediaEngineVersion();
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_agora_get_media_engine_version : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_agora_get_media_engine_version)

static bool js_agora_setCallback(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc >= 1)
    {
        le::agora* cobj = (le::agora*)s.nativeThisObject();
        
        se::Value jsFunc = args[0];
        se::Value jsTarget = argc > 1 ? args[1] : se::Value::Undefined;
        
        if (jsFunc.isNullOrUndefined())
        {
            cobj->setCallback(nullptr);
        }
        else
        {
            assert(jsFunc.isObject() && jsFunc.toObject()->isFunction());
            
            // 如果当前agora是可以被new出来的类，我们 使用se::Object::attachObject把jsFunc和jsTarget关联到当前对象中
            s.thisObject()->attachObject(jsFunc.toObject());
            s.thisObject()->attachObject(jsTarget.toObject());
            
            // 如果当前agora类是一个单例类，或者永远只有一个实例的类，我们不能用se::Object::attachObject去关联
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
SE_BIND_FUNC(js_agora_setCallback)

bool register_all_agora(se::Object* global)
{
    // 保证namespace对象存在
    se::Value nsVal;
    if (!global->getProperty("le", &nsVal))
    {
        // 不存在则创建一个JS对象，相当于 var ns = {};
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        
        // 将ns对象挂载到global对象中，名称为ns
        global->setProperty("le", nsVal);
    }
    se::Object* ns = nsVal.toObject();
    
    // 创建一个Class对象，开发者无需考虑Class对象的释放，其交由ScriptEngine内部自动处理
    auto cls = se::Class::create("agora", ns, nullptr, _SE(js_agora_constructor)); // 如果无构造函数，最后一个参数可传入nullptr，则这个类在JS中无法被new agora()出来
    
    // 为这个Class对象定义成员函数、属性、静态函数、析构函数
    cls->defineFunction("foo", _SE(js_agora_foo));
    cls->defineFunction("setCallback", _SE(js_agora_setCallback));
    cls->defineProperty("xxx", _SE(js_agora_get_xxx), _SE(js_agora_set_xxx));
    
    cls->defineFinalizeFunction(_SE(js_agora_finalize));
    cls->defineStaticFunction("getSdkVersion", _SE(js_agora_get_sdk_version));
    cls->defineStaticFunction("getMediaEnginVersion", _SE(js_agora_get_media_engine_version));

    // 注册类型到JS VirtualMachine的操作
    cls->install();
    
    // JSBClassType为Cocos引擎绑定层封装的类型注册的辅助函数，此函数不属于ScriptEngine这层
    JSBClassType::registerClass<le::agora>(cls);
    
    // 保存注册的结果，便于其他地方使用，比如类继承
    __jsb_ns_agora_proto = cls->getProto();
    __jsb_ns_agora_class = cls;
    
    // 为每个此Class实例化出来的对象附加一个属性
    __jsb_ns_agora_proto->setProperty("yyy", se::Value("helloyyy"));
    
    // 注册静态成员变量和静态成员函数
    se::Value ctorVal;
    if (ns->getProperty("agora", &ctorVal) && ctorVal.isObject())
    {
        ctorVal.toObject()->setProperty("static_val", se::Value(200));
    }
    
    // 清空异常
    se::ScriptEngine::getInstance()->clearException();
    return true;
}
