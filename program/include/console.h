#ifndef __SLIM__CONSOLE__H
#define __SLIM__CONSOLE__H
#include <string>
#include <unordered_map>
#include <v8.h>
namespace slim::console {
    struct BaseConfiguration {
        int precision = 4;
        bool dim = false;
        bool bold = false;
        bool italic = false;
        bool inverse = false;
        bool underline = false;
        bool show_location = true;
        bool expand_object = false;
        std::string text_color = "default";
        std::string background_color = "default";
    };
    struct Configuration: BaseConfiguration {
        std::string level_string;
        std::string text_color = "default";
        bool expand_object = false;
        BaseConfiguration location{};
        BaseConfiguration message_text{};
        BaseConfiguration message_value{};
        BaseConfiguration remainder{};
        std::unordered_map<std::string, BaseConfiguration*> members {
            {"location", &location}, {"message_text", &message_text},
            {"message_value", &message_value}, {"remainder", &remainder}
        };
    };
    static std::string colorize(auto* configuration, const std::string string_value);
    static void print(const v8::FunctionCallbackInfo<v8::Value>& args, slim::console::Configuration* configuration);
    extern void print_colors(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void console_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void print_colors(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void error(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void info(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void log(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void todo(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void warn(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif