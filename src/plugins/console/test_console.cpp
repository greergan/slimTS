/* todo: add try catch throw as a measure to catch v8 errors see 'auto result = ' below */
#include <any>
#include <chrono>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <v8.h>
#include <slim/utilities.h>
#include <slim/plugin.hpp>
#include <slim/v8.hpp>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */
namespace slim::console {
    v8::Isolate* global_isolate = slim::gv8::GetIsolate();
    v8::Local<v8::Array> listen_array = v8::Array::New(global_isolate);
    bool listening = false;
    bool output_when_listening = false;
    struct Configuration {
        /* field separator properties */
        struct Field_Separator {
            std::string separator = ":";
            bool trailing_space = false;
            bool dim = false;
            bool bold = false;
            bool italic = false;
            bool inverse = false;
            bool underline = false;
            bool show = false;
            std::string text_color = "default";
            std::string background_color = "default";
        } field_separator;
        /* common printer properties */
        struct Info {
            std::string level_string = "INFO";
            bool dim = false;
            bool bold = true;
            bool italic = false;
            bool inverse = false;
            bool underline = false;
            bool show = true;
            bool expand_objects = false;
            std::string text_color = "default";
            std::string background_color = "default";
            struct TimeStampConfiguration {
                bool dim = false;
                bool bold = true;
                bool italic = false;
                bool inverse = false;
                bool underline = false;
                bool show = false;
                int show_right = 0;
                std::string time_format = "epoch";
            } time_stamp;
        } info;
    } configuration;
    std::vector<std::string> colors {
        "default", "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", "bright black",
        "bright red", "bright green", "bright yellow", "bright blue", "bright magenta", "bright cyan", "bright white"
    };
    std::unordered_map<std::string, int> text_colors {
        {"default", 39}, {"black", 30}, {"red", 31}, {"green", 32}, {"yellow", 33}, {"blue", 34},
        {"magenta", 35}, {"cyan", 36}, {"white", 37}, {"bright black", 90}, {"bright red", 91}, {"bright green", 92},
        {"bright yellow", 93}, {"bright blue", 94}, {"bright magenta", 95}, {"bright cyan", 96}, {"bright white", 97},
    };
    std::unordered_map<std::string, int> background_colors {
        {"default", 49}, {"black", 40}, {"red", 41}, {"green", 42}, {"yellow", 43}, {"blue", 44},
        {"magenta", 45}, {"cyan", 46}, {"white", 47}, {"bright black", 100}, {"bright red", 101}, {"bright green", 102},
        {"bright yellow", 103}, {"bright blue", 104}, {"bright magenta", 105}, {"bright cyan", 106}, {"bright white", 107}
    };
    /********/
    static std::string colorize(auto* configuration, const std::string string_value);
    slim::plugin::plugin CreateSublevelPlugin(v8::Isolate* isolate, const std::string level, auto configuration);
    void local_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    /********/
    void info(const v8::FunctionCallbackInfo<v8::Value>& args);
    /********/
}
std::string slim::console::colorize(auto* configuration, const std::string string_value) {
    std::stringstream return_stream;
    if(configuration->show) {
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
        int console_text_color = text_colors[configuration->text_color];
        int console_background_color = background_colors[configuration->background_color];
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
    }
    return return_stream.str();
}
void slim::console::local_print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    auto context = global_isolate->GetCurrentContext();
    std::stringstream output;
    auto log_time_stamp_epoch = std::chrono::system_clock::now().time_since_epoch();
    std::string log_time_stamp_string = "";
    std::string is_epoch = configuration.info.time_stamp.time_format;
    transform(is_epoch.begin(), is_epoch.end(), is_epoch.begin(), ::tolower);
    if(is_epoch == "epoch") {
        log_time_stamp_string = std::to_string(log_time_stamp_epoch.count());
        if(configuration.info.time_stamp.show_right > 0) {
            log_time_stamp_string = log_time_stamp_string.substr(log_time_stamp_string.length() - configuration.info.time_stamp.show_right);
        }
    }
    else {
        log_time_stamp_string = std::to_string(log_time_stamp_epoch.count());
    }
    auto dressed_field_separator = configuration.field_separator.separator;
    if(configuration.field_separator.trailing_space) {
        dressed_field_separator += " ";
    }
    output << configuration.info.level_string << dressed_field_separator;
    for(int index = 0; index < args.Length(); index++) {
        auto value = args[index];
        if(value->IsObject()) {
            auto json_string_value = v8::JSON::Stringify(isolate->GetCurrentContext(), value);
            std::string string_value = slim::utilities::v8StringToString(isolate, json_string_value.ToLocalChecked());
            output << string_value;
        }
        else {
            output << slim::utilities::v8ValueToString(isolate, value);
        }
        if(index != args.Length() - 1) {
            output << " ";
        }
    }
    std::cerr << output.str() << "\n";
    return;
}
/********/
void slim::console::info(const v8::FunctionCallbackInfo<v8::Value>& args) {
    local_print(args);
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin console_plugin(isolate, "console");
	console_plugin.add_function("info",           &slim::console::info);
    auto createSublevelPlugin = [&isolate](const std::string level, auto* configuration) -> slim::plugin::plugin {
        slim::plugin::plugin sublevel_plugin(isolate, level);
        sublevel_plugin.add_property("background_color", &configuration->background_color);
        sublevel_plugin.add_property("bold",             &configuration->bold);
        sublevel_plugin.add_property("dim",              &configuration->dim);
        sublevel_plugin.add_property("expand_objects",   &configuration->expand_objects);
        sublevel_plugin.add_property("inverse",          &configuration->inverse);
        sublevel_plugin.add_property("italic",           &configuration->italic);      
        sublevel_plugin.add_property("show",             &configuration->show);
        sublevel_plugin.add_property("text_color",       &configuration->text_color);
        sublevel_plugin.add_property("underline",        &configuration->underline);
        return sublevel_plugin;
    };
    slim::plugin::plugin configuration_plugin(isolate, "configuration");
    slim::plugin::plugin field_separator_plugin(isolate, "field_separator");
    field_separator_plugin.add_property("separator",              &slim::console::configuration.field_separator.separator);
    field_separator_plugin.add_property("background_color",       &slim::console::configuration.field_separator.background_color);
    field_separator_plugin.add_property("text_color",             &slim::console::configuration.field_separator.text_color);
    field_separator_plugin.add_property("dim",                    &slim::console::configuration.field_separator.dim);
    field_separator_plugin.add_property("show",                   &slim::console::configuration.field_separator.show);
    field_separator_plugin.add_property("bold",                   &slim::console::configuration.field_separator.bold);
    field_separator_plugin.add_property("trailing_space",         &slim::console::configuration.field_separator.trailing_space);

/*             bool dim = false;
            bool bold = true;
            bool italic = false;
            bool inverse = false;
            bool underline = false;
            bool show = true;
            std::string text_color = "default";
            std::string background_color = "default"; */

    slim::plugin::plugin info_plugin(isolate, "info");
    info_plugin.add_property("level_string",              &slim::console::configuration.info.level_string);

    configuration_plugin.add_plugin("field_separator", &field_separator_plugin);
    configuration_plugin.add_plugin("info", &info_plugin);
    console_plugin.add_plugin("configuration", &configuration_plugin);
	console_plugin.expose_plugin();
	return;
}
