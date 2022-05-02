#ifndef __SLIM__CONSOLE__HPP
#define __SLIM__CONSOLE__HPP
#include <mutex>
#include <any>
#include <unordered_map>
#include <utilities.hpp>
#include <v8.h>
#include <v8pp/json.hpp>
#include <console.h>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */

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
namespace slim::console {
    void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    void copy_configuration(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        destination->expand_object = source->expand_object;
    }
    void configure_console(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::Configuration* configuration) {
        if(object->IsObject() && slim::utilities::PropertyCount(isolate, object) > 0) {
            configuration->text_color = slim::utilities::SlimColorValue(isolate, slim::utilities::GetValue(isolate, "text_color", object));
            configuration->text_color = slim::utilities::SlimColorValue(isolate, slim::utilities::GetValue(isolate, "background_color", object));
            configuration->precision  = slim::utilities::SlimIntValue(isolate,   slim::utilities::GetValue(isolate, "precision", object));
            configuration->dim = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "dim", object));
            configuration->bold = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "bold", object));
            configuration->italic = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "italic", object));
            configuration->underline = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "underline", object));
            configuration->expand_object = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "expand_object", object));
        }
    }
    void configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if(!args[0]->IsObject()) { return; }
        auto isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        auto configurations = slim::utilities::GetObject(isolate, args[0]);
        for(auto level: {"dir", "log"}) {
            auto level_configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, level_configuration) > 0) {
                configure_console(isolate, level_configuration, std::any_cast<slim::console::Configuration*>(slim::console::configuration::configurations[level]));
            }
        }
        for(auto level: {"debug", "error", "info", "todo", "trace", "warn"}) {
            auto configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                auto level_configuration = std::any_cast<slim::console::ExtendedConfiguration*>(slim::console::configuration::configurations[level]);
                configure_console(isolate, configuration, level_configuration);
                auto propogate = slim::utilities::GetValue(isolate, "propogate", configuration);
                if(propogate->IsBoolean() && propogate->BooleanValue(isolate)) {
                    for(auto subsection_name: {"location", "remainder", "message_text", "message_value"}) {
                        auto section = level_configuration->sub_configurations[subsection_name];
                        copy_console_configuration(level_configuration, section);
                    }
                }
                for(auto subsection_name: {"location", "remainder"}) {
                    auto subsection_configuration = slim::utilities::GetObject(isolate, subsection_name, configuration);
                    if(slim::utilities::PropertyCount(isolate, subsection_configuration) > 0) {
                        auto section = level_configuration->sub_configurations[subsection_name];
                        auto inherit = slim::utilities::GetValue(isolate, "inherit", subsection_configuration);
                        if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                            copy_console_configuration(level_configuration, section);
                        }
                        configure_console(isolate, subsection_configuration, section);
                    }
                }
                auto sub_configuration = slim::utilities::GetObject(isolate, "message", configuration);
                if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                    auto inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        copy_console_configuration(level_configuration, &level_configuration->message_text);
                        copy_console_configuration(level_configuration, &level_configuration->message_value);
                    }
                    else {
                        auto message_configuration = slim::utilities::GetObject(isolate, "text", configuration);
                        if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                            auto section = &level_configuration->message_text;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(level_configuration, section);
                            }
                            configure_console(isolate, message_configuration, section);
                        }
                        message_configuration = slim::utilities::GetObject(isolate, "value", configuration);
                        if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                            auto section = &level_configuration->message_value;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(level_configuration, section);
                            }
                            configure_console(isolate, message_configuration, section);
                        }
                    }
                }
            }
        }
    }
    void copy_configuration(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();
        auto copy_expects = "copy expects exactly 2 string arguments";
        if(args.Length() != 2) {
            isolate->ThrowException(slim::utilities::StringToString(isolate, copy_expects));
        }
        if(!args[0]->IsString() || !args[1]->IsString()) {
            isolate->ThrowException(slim::utilities::StringToString(isolate, copy_expects));
        }
        auto from = slim::utilities::StringValue(isolate, args[0]);
        auto to = slim::utilities::StringValue(isolate, args[1]);
        if(from == to) {
            isolate->ThrowException(slim::utilities::StringToString(isolate, "cannot copy to self"));
        }
        for(const std::string configuration: {to, from}) {
            if(!slim::console::configuration::configurations.contains(configuration)) {
                isolate->ThrowException(slim::utilities::StringToString(isolate, "level not found: " + configuration));
            }
        }
        std::vector<std::string> to_from_basic {"dir", "log"};
        auto find_result = std::find(std::begin(to_from_basic), std::end(to_from_basic), from);
        if(find_result != std::end(to_from_basic)) {
            auto find_result = std::find(std::begin(to_from_basic), std::end(to_from_basic), to);
            if(find_result != std::end(to_from_basic)) {
                auto from_configuration = std::any_cast<slim::console::Configuration*>(slim::console::configuration::configurations[from]);
                auto to_configuration = std::any_cast<slim::console::Configuration*>(slim::console::configuration::configurations[to]);
                copy_console_configuration(from_configuration, to_configuration);
            }
            else {
                isolate->ThrowException(slim::utilities::StringToString(isolate, "level types must match"));
            }
        }
        else {
            auto from_configuration = std::any_cast<slim::console::ExtendedConfiguration*>(slim::console::configuration::configurations[from]);
            auto to_configuration = std::any_cast<slim::console::ExtendedConfiguration*>(slim::console::configuration::configurations[to]);
            copy_console_configuration(from_configuration, to_configuration);
            for(auto sub_level: {"location", "remainder", "message_text", "message_value"}) {
                copy_console_configuration(from_configuration->sub_configurations[sub_level], to_configuration->sub_configurations[sub_level]);
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
            else if(args[i]->IsBoolean()) { output << args[i]->BooleanValue(isolate); }
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