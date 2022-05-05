#ifndef __SLIM__CONSOLE__INCLUDES
#define __SLIM__CONSOLE__INCLUDES
#include <unordered_map>
#include <string>
namespace slim::console::configuration {
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
}
#endif