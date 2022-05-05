#include <console_configuration.h>
#include <utilities.h>
namespace slim::console::configuration {
    slim::console::Configuration dir{.expand_object=true}, log{};
    slim::console::Configuration debug{.level_string="DEBUG", .text_color="red"};
    slim::console::Configuration error{.level_string="ERROR", .text_color="red"};
    slim::console::Configuration info{.level_string="INFO", .text_color="default"};
    slim::console::Configuration todo{.level_string="TODO", .text_color="blue"};
    slim::console::Configuration trace{.level_string="TRACE", .text_color="blue"};
    slim::console::Configuration warn{.level_string="WARN", .text_color="yellow"};
    std::unordered_map<std::string, slim::console::Configuration*> configurations {
        {"dir", &dir}, {"log", &log}, {"debug", &debug}, {"error", &error},
        {"info", &info}, {"todo", &todo}, {"trace", &trace}, {"warn", &warn}
    };
};
void slim::console::configuration::copy(const slim::console::BaseConfiguration* source, slim::console::BaseConfiguration* destination) {
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
void slim::console::configuration::copy(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
    copy(from_configuration, to_configuration);
    for(auto sub_level: {"location", "remainder", "message_text", "message_value"}) {
        copy(from_configuration->members[sub_level], to_configuration->members[sub_level]);
    }
}
void slim::console::configuration::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(!args[0]->IsObject()) { return; }
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    auto configurations = slim::utilities::GetObject(isolate, args[0]);
    for(auto level: {"dir", "log" "debug", "error", "info", "todo", "trace", "warn"}) {
        auto configuration = slim::utilities::GetObject(isolate, level, configurations);
        if(slim::utilities::PropertyCount(isolate, configuration) > 0) {
            auto level_configuration = slim::console::configuration::configurations[level];
            configure(isolate, configuration, level_configuration);
            auto propogate = slim::utilities::GetValue(isolate, "propogate", configuration);
            if(propogate->IsBoolean() && propogate->BooleanValue(isolate)) {
                for(auto subsection_name: {"location", "remainder", "message_text", "message_value"}) {
                    auto section = level_configuration->members[subsection_name];
                    copy(level_configuration, section);
                }
            }
            for(auto subsection_name: {"location", "remainder"}) {
                auto subsection_configuration = slim::utilities::GetObject(isolate, subsection_name, configuration);
                if(slim::utilities::PropertyCount(isolate, subsection_configuration) > 0) {
                    auto section = level_configuration->members[subsection_name];
                    auto inherit = slim::utilities::GetValue(isolate, "inherit", subsection_configuration);
                    if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                        copy(level_configuration, section);
                    }
                    configure(isolate, subsection_configuration, section);
                }
            }
            auto sub_configuration = slim::utilities::GetObject(isolate, "message", configuration);
            if(slim::utilities::PropertyCount(isolate, sub_configuration) > 0) {
                auto inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                    copy(level_configuration, &level_configuration->message_text);
                    copy(level_configuration, &level_configuration->message_value);
                }
                else {
                    auto message_configuration = slim::utilities::GetObject(isolate, "text", configuration);
                    if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                        auto section = level_configuration->message_text;
                        inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                        if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                            copy(level_configuration, &section);
                        }
                        configure(isolate, message_configuration, &section);
                    }
                    message_configuration = slim::utilities::GetObject(isolate, "value", configuration);
                    if(slim::utilities::PropertyCount(isolate, message_configuration) > 0) {
                        auto section = level_configuration->message_value;
                        inherit = slim::utilities::GetValue(isolate, "inherit", sub_configuration);
                        if(inherit->IsBoolean() && inherit->BooleanValue(isolate)) {
                            copy(level_configuration, &section);
                        }
                        configure(isolate, message_configuration, &section);
                    }
                }
            }
        }
    }
}
void slim::console::configuration::configure(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::BaseConfiguration* configuration) {
    if(object->IsObject() && slim::utilities::PropertyCount(isolate, object) > 0) {
        configuration->text_color = slim::utilities::GetColorValueOrException(isolate, slim::utilities::GetValue(isolate, "text_color", object));
        configuration->text_color = slim::utilities::GetColorValueOrException(isolate, slim::utilities::GetValue(isolate, "background_color", object));
        configuration->precision  = slim::utilities::GetIntValueOrException(isolate,   slim::utilities::GetValue(isolate, "precision", object));
        configuration->dim = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "dim", object));
        configuration->bold = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "bold", object));
        configuration->italic = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "italic", object));
        configuration->inverse = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "inverse", object));
        configuration->underline = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "underline", object));
        configuration->expand_object = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "expand_object", object));
        configuration->show_location = slim::utilities::GetBoolValueOrException(isolate, slim::utilities::GetValue(isolate, "show_location", object));
    }
}