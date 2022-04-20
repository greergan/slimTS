#include <any>
#include <mutex>
#include <unordered_map>
#include <utilities.hpp>
#include <v8pp/convert.hpp>
#include <v8pp/module.hpp>
#include <v8pp/json.hpp>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */
namespace slim::console::configuration {
    int precision = 10;
    struct Config {
        bool dim = false;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        std::string text = "default";
        std::string background = "default";
    };
    Config log{};
    Config dir{};
}
namespace slim::console::colors {
    std::unordered_map<std::string, int> text {
        {"default", 39},
        {"black", 30},
        {"red", 31},
        {"green", 32},
        {"yellow", 33},
        {"blue", 34},
        {"magenta", 35},
        {"cyan", 36},
        {"white", 37},
        {"bright black", 90},
        {"bright red", 91},
        {"bright green", 92},
        {"bright yellow", 93},
        {"bright blue", 94},
        {"bright magenta", 95},
        {"bright cyan", 96},
        {"bright white", 97}
    };
    std::unordered_map<std::string, int> background {
        {"default", 49},
        {"black", 40},
        {"red", 41},
        {"green", 42},
        {"yellow", 43},
        {"blue", 44},
        {"magenta", 45},
        {"cyan", 46},
        {"white", 47},
        {"bright black", 100},
        {"bright red", 101},
        {"bright green", 102},
        {"bright yellow", 103},
        {"bright blue", 104},
        {"bright magenta", 105},
        {"bright cyan", 106},
        {"bright white", 107}
    };
}
namespace slim::console {
    std::string ValueToString(v8::Isolate* isolate, v8::Local<v8::Value> value) {
        v8::String::Utf8Value utf8_value(isolate, value);
        return std::string(*utf8_value);
    }
    void clear() {
        std::cerr << "\x1B[2J\x1B[H";
    }
    void configure(const FunctionCallbackInfo<Value>& args) {
        if(!args[0]->IsObject()) { return; }
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Object> configurations = args[0]->ToObject(context).ToLocalChecked();
        v8::Local<v8::Name> log_name = v8::String::NewFromUtf8(isolate, "log").ToLocalChecked();
        v8::Local<v8::Value> log_configuration = configurations->Get(context, log_name).ToLocalChecked();
        if(log_configuration->IsObject()) {
            v8::Local<v8::Object> config = log_configuration->ToObject(context).ToLocalChecked();
            v8::Local<v8::Name> text_name = v8::String::NewFromUtf8(isolate, "text").ToLocalChecked();
            std::string text = slim::utilies::ValueToString(isolate, config->Get(context, text_name).ToLocalChecked());           
            if(text != "undefined") {
                slim::console::configuration::log.text = text;
            }
            v8::Local<v8::Name> background_name = v8::String::NewFromUtf8(isolate, "background").ToLocalChecked();
            std::string background = slim::utilies::ValueToString(isolate, config->Get(context, background_name).ToLocalChecked());
            if(background != "undefined") {
                slim::console::configuration::log.background = background;
            }
            v8::Local<v8::Name> dim_name = v8::String::NewFromUtf8(isolate, "port").ToLocalChecked();
            v8::Local<v8::Value> dim_value = config->Get(context, dim_name).ToLocalChecked();
            if(dim_value->IsBoolean()) {
                slim::console::configuration::log.dim = dim_value->BooleanValue(isolate);
            }
        }
    }
    struct out {
        int text;
        int background;
        std::string bold;
        std::string dim;
        std::string italic;
        std::string underline;
        out(struct slim::console::configuration::Config &config) {
            text = slim::console::colors::text[config.text];
            background = slim::console::colors::background[config.background];
            bold = (config.bold) ? "1;" : "";
            dim = (config.dim) ? "2;" : "";
            italic = (config.italic) ? "3;" : "";
            underline = (config.underline) ? "4;" : "";
        };
        template<typename T>
        out &operator<<(const T& _t) {
            std::cerr << "\x1B[" << italic << underline << dim << bold << text << ";" << background << "m" << _t << "\x1B[0m";
            return *this;
        }
        out &operator<<(std::ostream& (*fp)(std::ostream&)) {
            std::cerr << "\x1B[" << italic << underline << dim << bold << text << ";" << background << "m" << fp << "\x1B[0m";
            return *this;
        }
    };
    void log(const FunctionCallbackInfo<Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        std::mutex mtx;
        out print = out(slim::console::configuration::log);
        mtx.lock();
        std::cerr.precision(slim::console::configuration::precision);
        for(int i = 0; i < args.Length(); i++) {
            if(args[i]->IsNumber()) {
                print << args[i]->NumberValue(context).FromJust();
            }
            else if(args[i]->IsString()) {
                print << ValueToString(isolate, args[i]->ToString(context).ToLocalChecked());
            }
            else if(args[i]->IsFunction()) {
                print << "Function " << ValueToString(isolate, args[i]->ToString(context).ToLocalChecked());
            }
            else if(args[i]->IsArray()) {
                print << "Array " << v8pp::json_str(isolate, args[i]);
            }
            else if(args[i]->IsObject()) {
                print << "Object " << v8pp::json_str(isolate, args[i]);
            }
            else{
                print << "Typeof " << ValueToString(isolate, args[i]->TypeOf(isolate));
            }
            if(i != args.Length() - 1) {
                std::cerr << " ";
            }
        }
        std::cerr << "\n";
        mtx.unlock();
    }
    void expose(v8::Isolate* isolate, v8::Local<v8::Context> context) {
        v8pp::module console_module(isolate);
        console_module.set("configure", &configure);
        console_module.set("clear", &clear);
        console_module.set("log", &log);
        v8::Maybe result = context->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "console"), console_module.new_instance());
    }
}