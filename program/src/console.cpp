#include <iostream>
#include <sstream>
#include <regex>
#include <v8.h>
#include <console.h>
#include <console_colors.h>
#include <console_configuration.h>
#include <utilities.h>
#include <v8pp/json.hpp>
std::string slim::console::colorize(auto* configuration, const std::string string_value) {
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
void slim::console::print(const v8::FunctionCallbackInfo<v8::Value>& args, slim::console::Configuration* configuration) {
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
        else if(value->IsFunction()) { value_stream << "Function " << slim::utilities::GetFunctionString(isolate, value); }
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
void slim::console::print_colors(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
void slim::console::dir(const v8::FunctionCallbackInfo<v8::Value>& args)   { print(args, &slim::console::configuration::dir); }
void slim::console::debug(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::debug); }
void slim::console::error(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::error); }
void slim::console::info(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::info); }
void slim::console::log(const v8::FunctionCallbackInfo<v8::Value>& args)   { print(args, &slim::console::configuration::log); }
void slim::console::todo(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::todo); }
void slim::console::trace(const v8::FunctionCallbackInfo<v8::Value>& args) { print(args, &slim::console::configuration::trace); }
void slim::console::warn(const v8::FunctionCallbackInfo<v8::Value>& args)  { print(args, &slim::console::configuration::warn); }
void slim::console::console_assert(const v8::FunctionCallbackInfo<v8::Value>& args) {}
void slim::console::clear(const v8::FunctionCallbackInfo<v8::Value>& args) { std::cerr << "\x1B[2J\x1B[H"; }