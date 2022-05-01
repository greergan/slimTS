#ifndef __SLIM__CONSOLE__HPP
#define __SLIM__CONSOLE__HPP
#include <mutex>
#include <any>
#include <unordered_map>
#include <utilities.hpp>
#include <v8.h>
#include <v8pp/json.hpp>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */

namespace slim::console {
    struct Configuration {
        int precision = 10;
        bool dim = false;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool slow_blink = false;
        bool fast_blink = false;
        bool expand_object = false;
        std::string text_color = "default";
        std::string background_color = "default";
    };
    struct ExtendedConfiguration: Configuration {
        std::string level_string;
        Configuration location{};
        Configuration message_text{};
        Configuration message_value{};
        Configuration remainder{};
        std::unordered_map<std::string, slim::console::Configuration*> sub_configurations {
            {"location", &location}, {"message_text", &message_text},
            {"message_value", &message_value}, {"remainder", &remainder}
        };
        ExtendedConfiguration(const std::string level, const std::string color="default") : level_string{level} {
            text_color = color;
        }
    };
}
namespace slim::console::configuration {
    slim::console::Configuration dir{.expand_object=true}, log{};
    slim::console::ExtendedConfiguration debug("DEBUG", "red");
    slim::console::ExtendedConfiguration error("ERROR", "red");
    slim::console::ExtendedConfiguration info("INFO", "default");
    slim::console::ExtendedConfiguration todo("TODO", "blue");
    slim::console::ExtendedConfiguration trace("TRACE", "blue");
    slim::console::ExtendedConfiguration warn("WARN", "yellow");
    std::unordered_map<std::string, std::any> configurations {
        {"dir", &dir}, {"log", &log}, {"debug", &debug}, {"error", &error},
        {"info", &info}, {"todo", &todo}, {"trace", &trace}, {"warn", &warn}
    };
}
namespace slim::console::colors {
    std::unordered_map<std::string, int> text {
        {"default", 39}, {"black", 30}, {"red", 31}, {"green", 32}, {"yellow", 33}, {"blue", 34},
        {"magenta", 35}, {"cyan", 36}, {"white", 37}, {"bright black", 90}, {"bright red", 91}, {"bright green", 92},
        {"bright yellow", 93}, {"bright blue", 94}, {"bright magenta", 95}, {"bright cyan", 96}, {"bright white", 97},
    };
    std::unordered_map<std::string, int> background {
        {"default", 49}, {"black", 40}, {"red", 41}, {"green", 42}, {"yellow", 43}, {"blue", 44},
        {"magenta", 45}, {"cyan", 46}, {"white", 47}, {"bright black", 100}, {"bright red", 101}, {"bright green", 102},
        {"bright yellow", 103}, {"bright blue", 104}, {"bright magenta", 105}, {"bright cyan", 106}, {"bright white", 107}
    };
}
namespace slim::console {
    void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    void dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    void debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    //void error(const std::string error_string);
    void error(const v8::FunctionCallbackInfo<v8::Value>& args);
    void info(const v8::FunctionCallbackInfo<v8::Value>& args);
    void log(const v8::FunctionCallbackInfo<v8::Value>& args);
    void todo(const v8::FunctionCallbackInfo<v8::Value>& args);
    void trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    void warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    void copy_console_configuration(const slim::console::Configuration* source, slim::console::Configuration* destination) {
        destination->dim = source->dim;
        destination->bold = source->bold;
        destination->italic = source->italic;
        destination->underline = source->underline;
        destination->precision = source->precision;
        destination->text_color = source->text_color;
        destination->background_color = source->background_color;
    }
    void configure_console(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::Configuration* configuration) {
        if(object->IsObject() && slim::utilities::PropertyCount(isolate, object) > 0) {
            auto text_color = slim::utilities::GetValue(isolate, "text_color", object);
            if(!text_color->IsNull()) {
                configuration->text_color = slim::utilities::StringValue(isolate, text_color);
            }
            auto background_color = slim::utilities::StringValue(isolate, "background_color", object);
            if(background_color != "undefined") {
                configuration->background_color = background_color;
            }
            auto precision = slim::utilities::GetValue(isolate, "precision", object);
            if(precision->IsInt32()) {
                configuration->precision = precision->Int32Value(isolate->GetCurrentContext()).FromJust();
            }
            auto dim = slim::utilities::GetValue(isolate, "dim", object);
            if(dim->IsBoolean()) {
                configuration->dim = dim->BooleanValue(isolate);
            }
            auto bold = slim::utilities::GetValue(isolate, "bold", object);
            if(bold->IsBoolean()) {
                configuration->bold = bold->BooleanValue(isolate);
            }
            auto italic = slim::utilities::GetValue(isolate, "italic", object);
            if(italic->IsBoolean()) {
                configuration->italic = italic->BooleanValue(isolate);
            }
            auto underline = slim::utilities::GetValue(isolate, "underline", object);
            if(underline->IsBoolean()) {
                configuration->underline = underline->BooleanValue(isolate);
            }
            auto expand_object = slim::utilities::GetValue(isolate, "underline", object);
            if(underline->IsBoolean()) {
                configuration->expand_object = expand_object->BooleanValue(isolate);
            }
        }
    }
    void configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if(!args[0]->IsObject()) { return; }
        auto isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        auto configurations = slim::utilities::GetObject(isolate, args[0]);
        for(auto level: {"dir", "log"}) {
            auto configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                configure_console(isolate, configuration, std::any_cast<slim::console::Configuration*>(slim::console::configuration::configurations[level]));
            }
        }
        for(auto level: {"debug", "error", "info", "todo", "trace", "warn"}) {
            auto configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                auto section_configuration = std::any_cast<slim::console::ExtendedConfiguration*>(slim::console::configuration::configurations[level]);
                configure_console(isolate, configuration, section_configuration);
                for(auto subsection_name: {"location", "remainder"}) {
                    auto sub_configuration = slim::utilities::GetObject(isolate, subsection_name, configuration);
                    if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                        auto section = &section_configuration->location;
                        auto inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                        if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                            copy_console_configuration(section_configuration, section);
                        }
                        else {    
                            configure_console(isolate, sub_configuration, section);
                        }
                    }
                }
                auto sub_configuration = slim::utilities::GetObject(isolate, "message", configuration);
                if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                    auto inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        auto section = &section_configuration->message_text;
                        copy_console_configuration(section_configuration, section);
                        section = &section_configuration->message_value;
                        copy_console_configuration(section_configuration, section);
                    }
                    else {
                        auto message_configuration = slim::utilities::GetObject(isolate, "text", configuration);
                        if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                            auto section = &section_configuration->message_text;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(section_configuration, section);
                            }
                            else {
                                configure_console(isolate, message_configuration, section);
                            }
                        }
                        message_configuration = slim::utilities::GetObject(isolate, "value", configuration);
                        if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                            auto section = &section_configuration->message_value;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(section_configuration, section);
                            }
                            else {
                                configure_console(isolate, message_configuration, section);
                            }
                        }
                    }
                }
            }
        }
    }
    struct out {
        std::string text_color;
        std::string background_color;
        std::string bold;
        std::string dim;
        std::string italic;
        std::string underline;
        std::string level_string;
        template<typename Thing>
        out(Thing& configuration) {
            text_color = "\33[" + std::to_string(slim::console::colors::text[configuration.text_color]) + "m";
            background_color = "\33[" + std::to_string(slim::console::colors::background[configuration.background_color]) + "m";
            bold = (configuration.bold) ? "\33[1m" : "";
            dim = (configuration.dim) ? "\33[2m" : "";
            italic = (configuration.italic) ? "\33[3m" : "";
            underline = (configuration.underline) ? "\33[4m" : "";
/*             if(configuration.custom_text_color > -1) {
                text_color = "\33[38;5;" + std::to_string(configuration.custom_text_color) + "m";
            }
            if(configuration.custom_background_color > -1) {
                background_color = "\33[48;5;" + std::to_string(configuration.custom_background_color) + "m";
            } */
        };
        template<typename Thing>
        out &operator<<(const Thing& thingy) {
            std::cerr << underline << italic << dim << bold << text_color << background_color << thingy << "\33[0m";
            return *this;
        }
    };
    void print(const v8::FunctionCallbackInfo<v8::Value>& args, slim::console::Configuration& configuration) {
        if(args.Length() == 0) { return; }
        auto isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        std::mutex mtx;
        std::cerr.precision(configuration.precision);
        auto output = out(configuration);
        std::lock_guard<std::mutex> lk(mtx);
        for(int i = 0; i < args.Length(); i++) {
            if(args[i]->IsNumber()) { output << slim::utilities::NumberValue(isolate, args[i]); }
            else if(args[i]->IsString()) { output << slim::utilities::StringValue(isolate, args[i]); }
            else if(args[i]->IsFunction()) { output << "Function " << slim::utilities::StringFunction(isolate, args[i]); }
            else if(args[i]->IsArray()) { output << "Array(" << slim::utilities::ArrayCount(args[i]) << ") " << v8pp::json_str(isolate, args[i]); }
            else if(args[i]->IsObject()) { output << "Object " << v8pp::json_str(isolate, args[i]); }
            else { output << "Typeof " << slim::utilities::StringValue(isolate, args[i]->TypeOf(isolate)); }
            if(i != args.Length() - 1) { std::cerr << " "; }
        }
        output << "\n";
    }
    void print(const v8::FunctionCallbackInfo<v8::Value>& args, slim::console::ExtendedConfiguration& configuration) {
        auto output = out(configuration);
        output << configuration.level_string << ": ";
        print(args, configuration.remainder);
    }
    void dir(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::dir); }
    void debug(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::debug); }
    //void error(const std::string error_string) { print(error_string, slim::console::configuration::error); }
    void error(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::error); }
    void info(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::info); }
    void log(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::log); }
    void todo(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::todo); }
    void trace(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::trace); }
    void warn(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, slim::console::configuration::warn); }
    void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args) {}
    void clear(const v8::FunctionCallbackInfo<v8::Value>& args) { std::cerr << "\x1B[2J\x1B[H"; }
}
#endif