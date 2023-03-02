#ifndef __SLIM__CONSOLE__HPP
#define __SLIM__CONSOLE__HPP
#include <mutex>
#include <any>
#include <iostream>
#include <unordered_map>
#include <utilities.hpp>
#include <v8.h>
#include <v8pp/json.hpp>
#include <console_temp.h>
#include <console_configuration.h>
#include <sstream>
#include <regex>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */
namespace slim::console {
    void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    void copy_configuration(const v8::FunctionCallbackInfo<v8::Value>& args);
    void print_colors(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    void copy_console_configuration(const slim::console::BaseConfiguration* source, slim::console::BaseConfiguration* destination) {
        destination->dim = source->dim;
        destination->bold = source->bold;
        destination->italic = source->italic;
        destination->inverse = source->inverse;
        destination->underline = source->underline;
        destination->precision = source->precision;
        destination->text_color = source->text_color;
        destination->background_color = source->background_color;
        destination->expand_object = source->expand_object;
        destination->show_location = source->show_location;
    }
    void configure_console(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::BaseConfiguration* configuration) {
        if(object->IsObject() && slim::utilities::PropertyCount(isolate, object) > 0) {
            configuration->text_color = slim::utilities::SlimColorValue(isolate, slim::utilities::GetValue(isolate, "text_color", object));
            configuration->text_color = slim::utilities::SlimColorValue(isolate, slim::utilities::GetValue(isolate, "background_color", object));
            configuration->precision  = slim::utilities::SlimIntValue(isolate,   slim::utilities::GetValue(isolate, "precision", object));
            configuration->dim = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "dim", object));
            configuration->bold = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "bold", object));
            configuration->italic = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "italic", object));
            configuration->inverse = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "inverse", object));
            configuration->underline = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "underline", object));
            configuration->expand_object = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "expand_object", object));
            configuration->show_location = slim::utilities::SlimBoolValue(isolate, slim::utilities::GetValue(isolate, "show_location", object));
        }
    }
    void configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if(!args[0]->IsObject()) { return; }
        auto isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        auto configurations = slim::utilities::GetObject(isolate, args[0]);
        for(auto level: {"dir", "log" "debug", "error", "info", "todo", "trace", "warn"}) {
            auto configuration = slim::utilities::GetObject(isolate, level, configurations);
            if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
                auto level_configuration = slim::console::configuration::configurations[level];
                configure_console(isolate, configuration, level_configuration);
                auto propogate = slim::utilities::GetValue(isolate, "propogate", configuration);
                if(propogate->IsBoolean() && propogate->BooleanValue(isolate)) {
                    for(auto subsection_name: {"location", "remainder", "message_text", "message_value"}) {
                        auto section = level_configuration->members[subsection_name];
                        copy_console_configuration(level_configuration, section);
                    }
                }
                for(auto subsection_name: {"location", "remainder"}) {
                    auto subsection_configuration = slim::utilities::GetObject(isolate, subsection_name, configuration);
                    if(slim::utilities::PropertyCount(isolate, subsection_configuration) > 0) {
                        auto section = level_configuration->members[subsection_name];
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
                            auto section = level_configuration->message_text;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(level_configuration, &section);
                            }
                            configure_console(isolate, message_configuration, &section);
                        }
                        message_configuration = slim::utilities::GetObject(isolate, "value", configuration);
                        if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                            auto section = level_configuration->message_value;
                            inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                            if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                                copy_console_configuration(level_configuration, &section);
                            }
                            configure_console(isolate, message_configuration, &section);
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
        auto from_configuration = slim::console::configuration::configurations[from];
        auto to_configuration = slim::console::configuration::configurations[to];
        copy_console_configuration(from_configuration, to_configuration);
        for(auto sub_level: {"location", "remainder", "message_text", "message_value"}) {
            copy_console_configuration(from_configuration->members[sub_level], to_configuration->members[sub_level]);
        }
    }
    std::string colorize(auto* configuration, const std::string string_value) {
        std::stringstream return_stream;
        if(configuration->bold) {
            return_stream << "\33[1m";
        }
        if(configuration->dim) {
            return_stream << "\33[2m";
        }
        if(configuration->italic) {
            return_stream << "\33[3m";
        }
        if(configuration->underline) {
            return_stream << "\33[4m";
        }
        if(configuration->inverse) {
            return_stream << "\33[7m";
        }
        int console_text_color = slim::console::colors::text[configuration->text_color];
        int console_background_color = slim::console::colors::background[configuration->background_color];
        if(console_text_color > 29) {
            return_stream << "\33[" + std::to_string(console_text_color) << "m";
        }
        else {
            if(stoi(configuration->text_color) > -1) {
                return_stream << "\33[38;5;" + configuration->text_color << "m";
            }
            else if(std::regex_match(configuration->text_color, std::regex("[0-9]{1,3};[0-9]{1,3};[0-9]{1,3}"))) {
                return_stream << "\33[38;2;" + configuration->text_color << "m";
            }
        }
        if(console_background_color > 38) {
            return_stream << "\33[" + std::to_string(console_background_color) << "m";
        }
        else {
            if(stoi(configuration->background_color) > -1) {
                return_stream << "\33[48;5;" + configuration->background_color << "m";
            }
            else if(std::regex_match(configuration->text_color, std::regex("[0-9]{1,3};[0-9]{1,3};[0-9]{1,3}"))) {
                return_stream << "\33[48;2;" + configuration->background_color << "m";
            }
        }
        return_stream << string_value << "\33[0m";
        return return_stream.str();
    }
    void print(const v8::FunctionCallbackInfo<v8::Value>& args, slim::console::Configuration* configuration) {
        if(args.Length() == 0) { return; }
        auto isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        std::stringstream output;
        int precision = configuration->precision;
        if(configuration->level_string.length() > 0) {
            output << colorize(configuration, configuration->level_string + ": ");
        }
        if(configuration->show_location) {
            v8::TryCatch try_catch(isolate);
            isolate->ThrowException(slim::utilities::StringToString(isolate, ""));
            if(try_catch.HasCaught()) {
                auto context = isolate->GetCurrentContext();
                auto message = try_catch.Message();
                auto file_name = slim::utilities::ScriptFileName(message);
                auto line_number = slim::utilities::ScriptLineNumber(message);
                file_name = file_name.substr(file_name.find_last_of('/')+1);
                output << colorize(&configuration->location, file_name + ":" + std::to_string(line_number) + "\t");
            }
        }
        std::stringstream value_stream;
        int index = 0;
        if(args[index]->IsString()) {
            auto printf_string = slim::utilities::StringValue(isolate, args[index]);
            std::regex format("(%s)|(%d)|(%f)|(%i)|(%o)|(%O)");
            for(auto i = std::sregex_iterator(printf_string.begin(), printf_string.end(), format); i != std::sregex_iterator(); i++) {
                std::cerr << "now in position " << i->position() << "\n";
                index++;
                if(i->str() == "%s") {
                    printf_string = std::regex_replace(printf_string, std::regex("(%s)"),
                        slim::utilities::StringValue(isolate, args[index]), std::regex_constants::format_first_only);
                }
                else if(i->str() == "%d" || i->str() == "%i") {
                    if(args[index]->IsInt32()) {
                        printf_string = std::regex_replace(printf_string, std::regex("(" + i->str() + ")"),
                            std::to_string(slim::utilities::IntValue(isolate, args[index])), std::regex_constants::format_first_only);
                    }
                    else {
                        printf_string = std::regex_replace(printf_string, std::regex("(" + i->str() + ")"),
                            "NaN", std::regex_constants::format_first_only);
                    }
                }
                else if(i->str() == "%f") {
                    if(args[index]->IsNumber()) {
                        auto number_string = std::to_string(slim::utilities::NumberValue(isolate, args[index]));
                        if(number_string.length() > precision) {
                            precision = number_string.length();
                        }
                        printf_string = std::regex_replace(printf_string, std::regex("(%f)"),
                            number_string, std::regex_constants::format_first_only);
                    }
                }
            }
            index++;
            value_stream << printf_string;
        }
        for(; index < args.Length(); index++) {
            auto value = args[index];
            if(value->IsNumber()) { value_stream << slim::utilities::NumberValue(isolate, value); }
            else if(value->IsBoolean()) { auto bool_value = (value->BooleanValue(isolate)) ? "true" : "false"; value_stream << bool_value; }
            else if(value->IsString()) { value_stream << slim::utilities::StringValue(isolate, value); }
            else if(value->IsFunction()) { value_stream << "Function " << slim::utilities::StringFunction(isolate, value); }
            else if(value->IsArray()) { value_stream << "Array(" << slim::utilities::ArrayCount(value) << ") " << v8pp::json_str(isolate, value); }
            else if(value->IsObject()) { value_stream << "Object " << v8pp::json_str(isolate, value); }
            else { value_stream << "Typeof " << slim::utilities::StringValue(isolate, value->TypeOf(isolate)); }
            if(index != args.Length() - 1) { value_stream << " "; }
        }
        std::cerr.precision(precision);
        if(configuration->level_string.length() > 0) {
            output << colorize(&configuration->remainder, value_stream.str());
        }
        else {
            output << colorize(configuration, value_stream.str());
        }
        std::cerr << output.str() << "\n";
    }
    void print_colors(const v8::FunctionCallbackInfo<v8::Value>& args) {
        slim::console::Configuration colors;
        auto print_color = [&colors](auto property) {
            int index = 0;
            for(auto color: slim::console::colors::colors) {
                *property = color;
                std::cerr << colorize(&colors, *property) << " ";
                index++;
                if(index == 5 || index == 10 || index == 13 || index == 16) {
                    std::cerr << "\n\t";
                }
            }
            *property = "default";
        };
        std::cerr << ".text_color\n\t";
        print_color(&colors.text_color);
        std::cerr << "\n\n";
        std::cerr << ".background_color\n\t";
        print_color(&colors.background_color);
        std::cerr << "\n";
    }
    void dir(const v8::FunctionCallbackInfo<v8::Value>& args)   { print(args, &slim::console::configuration::dir); }
    void debug(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::debug); }
    //void error(const std::string error_string) { print(error_string, slim::console::configuration::error); }
    void error(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::error); }
    void info(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::info); }
    void log(const v8::FunctionCallbackInfo<v8::Value>& args)   { print(args, &slim::console::configuration::log); }
    void todo(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::todo); }
    void trace(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::trace); }
    void warn(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::warn); }
    void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args) {}
    void clear(const v8::FunctionCallbackInfo<v8::Value>& args) { std::cerr << "\x1B[2J\x1B[H"; }
}
#endif