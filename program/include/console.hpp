#ifndef __SLIM__CONSOLE__HPP
#define __SLIM__CONSOLE_HPP
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
namespace slim::console::configuration_templates {
    struct Configuration {
        bool dim = false;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        std::string text_color = "default";
        std::string background_color = "default";
    };
    struct ExtendedConfiguration: Configuration {
        std::string level_string;
        Configuration log_level{};
        Configuration location{};
        struct message {
            Configuration text{};
            Configuration value{};
        };
        Configuration remainder{};
        ExtendedConfiguration(const std::string level, const std::string color) : level_string{level} {
            text_color = color;
        }
    };
}
namespace slim::console::configuration {
    int precision = 10;

    slim::console::configuration_templates::Configuration dir{};
    slim::console::configuration_templates::Configuration log{};
    slim::console::configuration_templates::ExtendedConfiguration todo("TODO", "blue");
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
    void clear() {
        std::cerr << "\x1B[2J\x1B[H";
    }
    void configure(const FunctionCallbackInfo<Value>& args) {
        if(!args[0]->IsObject()) { return; }
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Object> configurations = slim::utilities::GetObject(isolate, args[0]);
        v8::Local<v8::Object> log_configuration = slim::utilities::GetObject(isolate, "log", configurations);
        if(log_configuration->IsObject() && slim::utilities::PropertyCount(isolate, log_configuration) > 0) {
            std::string text_color = slim::utilities::StringValue(isolate, "text", log_configuration);
            if(text_color != "undefined") {
                slim::console::configuration::log.text_color = text_color;
            }
            std::string background_color = slim::utilities::StringValue(isolate, "background", log_configuration);
            if(background_color != "undefined") {
                slim::console::configuration::log.background_color = background_color;
            }
            v8::Local<v8::Value> dim = slim::utilities::GetValue(isolate, "dim", log_configuration);
            if(dim->IsBoolean()) {
                slim::console::configuration::log.dim = dim->BooleanValue(isolate);
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
        template<typename T>
        out(T &config) {
            text = slim::console::colors::text[config.text_color];
            background = slim::console::colors::background[config.background_color];
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
        if(args.Length() == 0) { return; }
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        std::mutex mtx;
        out print = out(slim::console::configuration::log);
        mtx.lock();
        std::cerr.precision(slim::console::configuration::precision);
        for(int i = 0; i < args.Length(); i++) {
            if(args[i]->IsNumber()) {
                print << slim::utilities::NumberValue(isolate, args[i]);
            }
            else if(args[i]->IsString()) {
                print << slim::utilities::StringValue(isolate, args[i]);
            }
            else if(args[i]->IsFunction()) {
                print << "Function " << slim::utilities::StringFunction(isolate, args[i]);
            }
            else if(args[i]->IsArray()) {
                print << "Array " << v8pp::json_str(isolate, args[i]);
            }
            else if(args[i]->IsObject()) {
                print << "Object " << v8pp::json_str(isolate, args[i]);
            }
            else{
                print << "Typeof " << slim::utilities::StringValue(isolate, args[i]->TypeOf(isolate));
            }
            if(i != args.Length() - 1) {
                std::cerr << " ";
            }
        }
        std::cerr << "\n";
        mtx.unlock();
    }
    void todo(const FunctionCallbackInfo<Value>& args) {
        if(args.Length() == 0) { return; }
        if(args[0]->IsString()) {
            v8::Isolate* isolate = args.GetIsolate();
            v8::HandleScope scope(isolate);
            out print = out(slim::console::configuration::todo);
            print << slim::utilities::StringValue(isolate, args[0]);
        }
    }
    void expose(v8::Isolate* isolate) {
        v8pp::module console_module(isolate);
        console_module.set("configure", &configure);
        console_module.set("clear", &clear);
        console_module.set("log", &log);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "console"), console_module.new_instance()).ToChecked();
    }
}
#endif