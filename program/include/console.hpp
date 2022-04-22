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
        bool expand_object = false;
        std::string rgb_text = "";
        std::string rbg_background = "";
        int custom_text_color = -1;
        int custom_background_color = -1;
        std::string text_color = "default";
        std::string background_color = "default";
    };
    struct ExtendedConfiguration: Configuration {
        std::string level_string;
        Configuration location{};
        Configuration message_text{};
        Configuration message_value{};
        Configuration remainder{};
        ExtendedConfiguration(const std::string level, const std::string color="default") : level_string{level} {
            text_color = color;
        }
    };
}
namespace slim::console::configuration {
    int precision = 10;
}
namespace slim::console::configuration {
    slim::console::configuration_templates::Configuration dir{};
    slim::console::configuration_templates::Configuration log{};
    slim::console::configuration_templates::ExtendedConfiguration debug("DEBUG", "red");
    slim::console::configuration_templates::ExtendedConfiguration error("ERROR", "red");
    slim::console::configuration_templates::ExtendedConfiguration info("INFO", "default");
    slim::console::configuration_templates::ExtendedConfiguration todo("TODO", "blue");
    slim::console::configuration_templates::ExtendedConfiguration trace("TRACE", "blue");
    slim::console::configuration_templates::ExtendedConfiguration warn("WARN", "yellow");
    std::unordered_map<std::string, slim::console::configuration_templates::Configuration*> configurations {
        {"dir", &dir},
        {"log", &log}
    };
    std::unordered_map<std::string, slim::console::configuration_templates::ExtendedConfiguration*> extended_configurations {
        {"debug", &debug},
        {"error", &error},
        {"info", &info},
        {"todo", &todo},
        {"trace", &trace},
        {"warn", &warn}
    };
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
        {"bright white", 97},
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
    void copy_console_configuration(const slim::console::configuration_templates::Configuration* source, slim::console::configuration_templates::Configuration* destination) {
        destination->dim = source->dim;
        destination->bold = source->bold;
        destination->italic = source->italic;
        destination->underline = source->underline;
        destination->rgb_text = source->rgb_text;
        destination->rbg_background = source->rbg_background;
        destination->custom_text_color = source->custom_text_color;
        destination->custom_background_color = source->custom_background_color;
        destination->text_color = source->text_color;
        destination->background_color = source->background_color;
    }
    void configure_console(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::configuration_templates::Configuration* configuration) {
        if(object->IsObject() && slim::utilities::PropertyCount(isolate, object) > 0) {
            int custom_text_color = slim::utilities::IntValuePositive(isolate, "custom_text_color", object);
            if(custom_text_color > -1 && custom_text_color <= 255) {
                configuration->custom_text_color = custom_text_color;
            }
            int custom_background_color = slim::utilities::IntValuePositive(isolate, "custom_background_color", object);
            if(custom_background_color > -1 && custom_background_color <= 255) {
                configuration->custom_background_color = custom_background_color;
            }
            std::string text_color = slim::utilities::StringValue(isolate, "text_color", object);
            if(text_color != "undefined") {
                configuration->text_color = text_color;
            }
            std::string background_color = slim::utilities::StringValue(isolate, "background_color", object);
            if(background_color != "undefined") {
                configuration->background_color = background_color;
            }
            v8::Local<v8::Value> dim = slim::utilities::GetValue(isolate, "dim", object);
            if(dim->IsBoolean()) {
                configuration->dim = dim->BooleanValue(isolate);
            }
            v8::Local<v8::Value> bold = slim::utilities::GetValue(isolate, "bold", object);
            if(bold->IsBoolean()) {
                configuration->bold = bold->BooleanValue(isolate);
            }
            v8::Local<v8::Value> italic = slim::utilities::GetValue(isolate, "italic", object);
            if(italic->IsBoolean()) {
                configuration->italic = italic->BooleanValue(isolate);
            }
            v8::Local<v8::Value> underline = slim::utilities::GetValue(isolate, "underline", object);
            if(underline->IsBoolean()) {
                configuration->underline = underline->BooleanValue(isolate);
            }
            v8::Local<v8::Value> expand_object = slim::utilities::GetValue(isolate, "underline", object);
            if(underline->IsBoolean()) {
                configuration->expand_object = expand_object->BooleanValue(isolate);
            }
        }
    }
    void configure(const FunctionCallbackInfo<Value>& args) {
        if(!args[0]->IsObject()) { return; }
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Object> configurations = slim::utilities::GetObject(isolate, args[0]);
        v8::Local<v8::Object> configuration;
        for(auto level: {"dir", "log"}) {
            configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                configure_console(isolate, configuration, slim::console::configuration::configurations[level]);
            }
        }
        for(auto level: {"debug", "error", "info", "todo", "trace", "warn"}) {
            configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                v8::Local<v8::Value> inherit;
                v8::Local<v8::Object> sub_configuration;
                v8::Local<v8::Object> sub_sub_configuration;
                slim::console::configuration_templates::Configuration* section;
                slim::console::configuration_templates::ExtendedConfiguration* section_configuration = slim::console::configuration::extended_configurations[level];
                configure_console(isolate, configuration, section_configuration);
                sub_configuration = slim::utilities::GetObject(isolate, "location", configuration);
                if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                    section = &section_configuration->location;
                    inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        copy_console_configuration(section_configuration, section);
                    }
                    else {    
                        configure_console(isolate, sub_configuration, section);
                    }
                }
                sub_configuration = slim::utilities::GetObject(isolate, "message", configuration);
                if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                    inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        section = &section_configuration->message_text;
                        copy_console_configuration(section_configuration, section);
                        section = &section_configuration->message_value;
                        copy_console_configuration(section_configuration, section);
                    }
                    else {
                        sub_sub_configuration = slim::utilities::GetObject(isolate, "text", configuration);
                        if(slim::utilities::PropertyCount(isolate, sub_sub_configuration) > 0) {
                            section = &section_configuration->message_text;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(section_configuration, section);
                            }
                            else {
                                configure_console(isolate, sub_sub_configuration, section);
                            }
                        }
                        sub_sub_configuration = slim::utilities::GetObject(isolate, "value", configuration);
                        if(slim::utilities::PropertyCount(isolate, sub_sub_configuration) > 0) {
                            section = &section_configuration->message_value;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(section_configuration, section);
                            }
                            else {
                                configure_console(isolate, sub_sub_configuration, section);
                            }
                        }
                    }
                }
                sub_configuration = slim::utilities::GetObject(isolate, "remainder", configuration);
                if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                    section = &section_configuration->remainder;
                    inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        copy_console_configuration(section_configuration, section);
                    }
                    else {    
                        configure_console(isolate, sub_configuration, section);
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
            if(configuration.custom_text_color > -1) {
                text_color = "\33[38;5;" + std::to_string(configuration.custom_text_color) + "m";
            }
            if(configuration.custom_background_color > -1) {
                background_color = "\33[48;5;" + std::to_string(configuration.custom_background_color) + "m";
            }
        };
        template<typename Thing>
        out &operator<<(const Thing& thingy) {
            std::cerr << underline << italic << dim << bold << text_color << background_color << thingy << "\33[0m";
            return *this;
        }
    };
    void print(const FunctionCallbackInfo<Value>& args, slim::console::configuration_templates::Configuration& configuration, bool expand_object=false) {
        if(args.Length() == 0) { return; }
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        std::mutex mtx;
        std::cerr.precision(slim::console::configuration::precision);
        out output = out(configuration);
        mtx.lock();
        for(int i = 0; i < args.Length(); i++) {
            if(args[i]->IsNumber()) { output << slim::utilities::NumberValue(isolate, args[i]); }
            else if(args[i]->IsString()) { output << slim::utilities::StringValue(isolate, args[i]); }
            else if(args[i]->IsFunction()) { output << "Function " << slim::utilities::StringFunction(isolate, args[i]); }
            else if(args[i]->IsArray()) { output << "Array(" << slim::utilities::ArrayCount(args[i]) << ") " << v8pp::json_str(isolate, args[i]); }
            else if(args[i]->IsObject()) { output << "Object " << v8pp::json_str(isolate, args[i]); }
            else { output << "Typeof " << slim::utilities::StringValue(isolate, args[i]->TypeOf(isolate)); }
            if(i != args.Length() - 1) { std::cerr << " "; }
        }
        std::cerr << "\n";
        mtx.unlock();
    }
    void print(const FunctionCallbackInfo<Value>& args, slim::console::configuration_templates::ExtendedConfiguration& configuration, bool expand_object=false) {
        out output = out(configuration);
        output << configuration.level_string << ": ";
        print(args, configuration.remainder, true);
    }
    void dir(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::dir, true); }
    void debug(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::debug); }
    void error(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::error); }
    void info(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::info); }
    void log(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::log); }
    void todo(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::todo); }
    void trace(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::trace); }
    void warn(const FunctionCallbackInfo<Value>& args) { print(args, slim::console::configuration::warn); }
    void console_assert() {}
    void clear() { std::cerr << "\x1B[2J\x1B[H"; }
    void expose(v8::Isolate* isolate) {
        v8pp::module console_module(isolate);
        console_module.set("assert", &console_assert);
        console_module.set("configure", &configure);
        console_module.set("clear", &clear);
        console_module.set("debug", &debug);
        console_module.set("dir", &dir);
        console_module.set("error", &error);
        console_module.set("info", &info);
        console_module.set("log", &log);
        console_module.set("todo", &todo);
        console_module.set("trace", &trace);
        console_module.set("warn", &warn);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "console"), console_module.new_instance()).ToChecked();
    }
}
#endif