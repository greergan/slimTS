#include <console.h>
#include <console_configuration.h>
#include <console_module.h>
slim::module::module slim::console::module::AddProperties(v8::Isolate* isolate, const std::string level, auto configuration) {
    slim::module::module section_module(isolate, level);
    section_module.AddProperty("precision", &configuration->precision);
    section_module.AddProperty("underline", &configuration->underline);
    section_module.AddProperty("inverse", &configuration->inverse);
    section_module.AddProperty("italic", &configuration->italic);
    section_module.AddProperty("dim", &configuration->dim);
    section_module.AddProperty("bold", &configuration->bold);
    section_module.AddProperty("show_location", &configuration->show_location);
    section_module.AddProperty("expand_object", &configuration->expand_object);
    section_module.AddProperty("background_color", &configuration->background_color);
    section_module.AddProperty("text_color", &configuration->text_color);
    return section_module;
}
slim::module::module slim::console::module::AddConfiguration(v8::Isolate* isolate) {
    slim::module::module configuration_module(isolate, "configuration");
/*     configuration_module.AddFunction("copy",
        [](const v8::FunctionCallbackInfo<v8::Value>& args){slim::console::configuration::copy(args);}); */
    for(auto level: {"log", "dir", "debug", "error", "info", "todo", "trace", "warn"}) {
        auto level_configuration = slim::console::configuration::configurations[level];
        auto section_module = AddProperties(isolate, level, level_configuration);
        for(auto sub_level: {"location", "message_text", "message_value", "remainder"}) {
            auto sub_level_configuration = level_configuration->members[sub_level];
            auto sub_section_module = AddProperties(isolate, sub_level, sub_level_configuration);
            section_module.AddModule(sub_level, &sub_section_module);
        }
        configuration_module.AddModule(level, &section_module);
    }
    return configuration_module;
}
void slim::console::module::ExposeModule(v8::Isolate* isolate) {
    slim::module::module configuration = AddConfiguration(isolate);
    slim::module::module console_module(isolate, "console");
    console_module.AddFunction("assert", &slim::console::console_assert);
    console_module.AddFunction("colors", &slim::console::print_colors);
/*     console_module.AddFunction("configure", 
        [](const v8::FunctionCallbackInfo<v8::Value>& args){slim::console::configuration::configure(args);}); */
    console_module.AddFunction("clear", &slim::console::clear);
    console_module.AddFunction("debug", &slim::console::debug);
    console_module.AddFunction("dir", &slim::console::dir);
    console_module.AddFunction("error", &slim::console::error);
    console_module.AddFunction("info", &slim::console::info);
    console_module.AddFunction("log", &slim::console::log);
    console_module.AddFunction("todo", &slim::console::todo);
    console_module.AddFunction("trace", &slim::console::trace);
    console_module.AddFunction("warn", &slim::console::warn);
    console_module.AddModule("configuration", &configuration);
    console_module.ExposeModule();
}