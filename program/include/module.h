#ifndef __SLIM__MODULE__H
#define __SLIM__MODULE__H
#include <functional>
#include <variant>
#include <v8.h>
namespace slim::module {
    class PropertyPointer {
        std::variant<bool*, std::string*, int*> property;
        public:
            PropertyPointer(auto&& property);
            ~PropertyPointer(void);
            v8::Local<v8::Value> GetV8Value(v8::Isolate* isolate);
            void SetValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    };
};
namespace slim::module {
    struct module {
        module(v8::Isolate* isolate, std::string name);
        template<typename Function, typename Signature = typename std::decay<Function>::type>
        void add_function(std::string name, Function&& function);
        void add_module(std::string name, module* submodule);
        void add_property(std::string name, auto&& property);
        void expose_module(void);
        v8::Local<v8::ObjectTemplate> get_module(void);
        v8::Local<v8::Object> new_instance(void);
        private:
            v8::Isolate* isolate;
            v8::Local<v8::Name> v8_name;
            v8::Local<v8::ObjectTemplate> module_template;
    };
};
#endif