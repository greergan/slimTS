#ifndef __SLIM__CONSOLE__INCLUDES
#define __SLIM__CONSOLE__INCLUDES
#include <unordered_map>
#include <string>
#include <vector>
namespace slim::console::colors {
    std::vector<std::string> colors {
        "default", "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", "bright black",
        "bright red", "bright green", "bright yellow", "bright blue", "bright magenta", "bright cyan", "bright white"
    };
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
    struct Configuration {
        int precision = 10;
        bool dim = false;
        bool bold = false;
        bool italic = false;
        bool inverse = false;
        bool underline = false;
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
#endif